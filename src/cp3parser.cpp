// cp3parser.cpp
/*
    Implementation of the parser for the Cp3mm processor
*/

#include "cp3parser.h"
#include "fileman.h"
#include "appinfo.h"

#include <stdexcept>

namespace Cp3mm {

namespace Parser {

Cp3Parser::Cp3Parser(InputFile &fin, OutputFile &foutH, OutputFile &foutC, Tds::Entity::Strictness levelChk)
        : inputFile( &fin ), outputHeader( &foutH ),
        outputImpl( &foutC )
{
    if ( !fin.isOpen() ) {
        throw std::runtime_error( fin.getFileName() + " is not open" );
    }

    if ( !foutH.isOpen() ) {
        throw std::runtime_error( foutH.getFileName() + " is not open" );
    }

    if ( !foutC.isOpen() ) {
        throw std::runtime_error( foutC.getFileName() + " is not open" );
    }

    const std::string & inputPath = inputFile->getFileName();

    std::string fileName = FileMan::getOnlyFileName( inputPath );
    module.setName( fileName );
    Tds::Entity::setStrictness( levelChk );
    onlyFileName = fileName + FileMan::getExt( inputPath );
    fin.close();
    lex.reset( new FileLexer( inputPath ) );
    writePreambles();
}

void Cp3Parser::writePreambles()
{
    // Write the include of the header
    outputImpl->writeLn(
            "#include \""
            + FileMan::getFileName( outputHeader->getFileName()  )
            + '"' )
    ;

    // Write the protection for the header
    outputHeader->writeLn(
        "#ifndef " + Tds::Module::ModulePrefix
        + StringMan::mays( module.getName() )
        + '_'
    );
    outputHeader->writeLn(
        "#define " + Tds::Module::ModulePrefix
        + StringMan::mays( module.getName() )
        + '_'
    );

    outputImpl->flush();
    outputHeader->flush();
}

void Cp3Parser::writeColophons()
{
    Tds::EntryPoint * fMain = module.getEntryPoint();

    // Write the header endif's
    outputHeader->writeLn( "#endif // module " + module.getName() );

    // Write the entry point (if any)
    if ( fMain != NULL ) {
        writeNumLineInfo( outputImpl, *lex, fMain->getLineNumber() );
        outputImpl->writeLn( fMain->getImplementation() );
    }

    outputHeader->writeLn();
    outputImpl->writeLn();
    outputHeader->close();
    outputImpl->close();
}

void Cp3Parser::processComments()
{
    lex->advance();

    if ( lex->getCurrentChar() == '/' )
    {
        lex->skipLine();
    }
    else
    if ( lex->getCurrentChar() == '*' )
    {
        lex->advance();
        lex->getLiteral( "*/" );
    }
}

bool Cp3Parser::isKeyword(const std::string &token)
{
    return (    Tds::Member::lookForStorageKeyword( lex->getCurrentToken() ) != NULL
             || Tds::Member::lookForTypeKeyword( lex->getCurrentToken() ) != NULL
             || Tds::Member::lookForStorageKeyword( lex->getCurrentToken() ) != NULL
             || Tds::Member::lookForModifierKeyword( lex->getCurrentToken() ) != NULL )
           ;
}

void Cp3Parser::processEndings()
{
    Tds::Namespace * ns = module.getCurrentNamespace();

    if ( ns == NULL ) {
        throwSyntaxError( "unexpected }, nothing to close" );
    }

    // Check for class ending
    if ( lex->getCurrentChar() == '}' ) {
        if ( module.getState() == Tds::Module::ClassLevel )
        {
            Tds::Class * cl = ns->getCurrentClass();

            if ( cl != NULL ) {
                // Prepare
                Tds::Container * cnt = ns->setOuterAsCurrentContainer();

                if ( dynamic_cast<Tds::Namespace *>( cnt ) ) {
                    module.resetState( Tds::Module::NamespaceLevel );
                }

                // Output
                lex->advance();
                outputHeader->writeLn( "}; // class " + cl->getName() );

                // Skip the ';', provided it is there
                lex->skipDelim();
                if ( lex->getCurrentChar() == Tds::Member::Semicolon[ 0 ] ) {
                    lex->advance();
                    lex->skipDelim();
                }
            }
            else throwSyntaxError( "unexpected } for closing class" );
        }
        else
        // Check for namespace endings
        if ( module.getState() == Tds::Module::NamespaceLevel )
        {
            Tds::Namespace * outerNS = ns->getContainer();

            if ( outerNS == NULL )
                    module.resetState();
            else    module.setOuterAsCurrentNamespace();

            lex->advance();

            outputHeader->writeLn( "} // namespace " + ns->getName() );
        }
        else throwSyntaxError( "unexpected '}'" );
    } else throwSyntaxError( "expected '}' to process" );
}

void Cp3Parser::processSpecialCharacter()
{
    if ( lex->getCurrentChar() == '/' )
    {
        processComments();
    }
    else
    // Check for directives
    if ( lex->getCurrentChar() == '#' )
    {
        updateNumLineInfo( outputHeader );
        processDirective();
    }
    else
    // Check for class endings
    if ( lex->getCurrentChar() == '}' ) {
        processEndings();
        updateNumLineInfo( outputHeader );
    }
    // Check for destructor
    else
    // Check for class endings
    if ( lex->getCurrentChar() == '~'
      && module.getState() == Tds::Module::ClassLevel )
    {
        processClassMember();
    }
    else throwSyntaxError(
                ( std::string( "bug character " )
                + lex->getCurrentChar()
                ).c_str()
    );

    return;
}

void Cp3Parser::process()
/**
    Reads each token of the file and processes it
*/
{
    while ( !lex->isEnd() )
    {
        // Check for comments and directives
        if ( lex->getCurrentTokenType() == Lexer::SpecialCharacter )
        {
            processSpecialCharacter();
            continue;
        }

        const std::string &token = lex->getToken();

        // Is it an import ?
        if ( token == Tds::Module::RWordImport ) {
            updateNumLineInfo( outputHeader );
            processImport();
            continue;
        }
        else
        // Is it a typedef ?
        if ( token == Tds::Module::RWordTypedef ) {
            updateNumLineInfo( outputHeader );
            processTypedef();
            continue;
        }
        else
        // Is it an using ?
        if ( token == Tds::Module::RWordUsing )
        {
            updateNumLineInfo( outputHeader );
            processUsing();
            continue;
        }
        else
        // Is it a class ?
        if ( token == Tds::Module::RWordClass )
        {
            updateNumLineInfo( outputHeader );
            processClass();
            continue;
        }
        else
        // Is it a namespace ?
        if ( token == Tds::Module::RWordNamespace )
        {
            updateNumLineInfo( outputHeader );
            processNamespace();
            continue;
        }
        else
        // Is it a visibility keyword ?
        if ( Tds::Member::lookForVisibilityKeyword( token ) )
        {
            processVisibility();
            continue;
        }
        else
        // Is it a function ?
        if ( module.getState() == Tds::Module::ClassLevel ) {
            processClassMember();
            continue;
        }
        else
        if ( module.getState() == Tds::Module::NamespaceLevel
          || module.getState() == Tds::Module::TopLevel )
        {
            processNamespaceMember();
            continue;
        }
        else throwSyntaxError( ( "Nonsense: " + token ).c_str() );
    }

    // Finish writing
    writeColophons();

    // Was the module correctly parsed? Do strictness corrections
    if ( module.getState() != Tds::Module::TopLevel )
            throwSyntaxError( "unexpected module end... missing '}'?" );
    else    module.chk();

    return;
}

void Cp3Parser::processField(Tds::Class &cl, Tds::Attribute &atr)
{
    std::string initValue;

    // Get init string
    if ( lex->getCurrentChar() == '=' )
    {
        lex->advance();
        lex->skipDelim();
        initValue = lex->getLiteral( Tds::Member::Semicolon );

        // Check availability of initialization
        if ( atr.getSystemStorage() != &Tds::Member::StaticStorage )
        {
            throwSyntaxError( "member field must be static to be initialized" );
        }

        atr.setInitialValue( initValue );
    } else {
        // Skip semicolon
        skipDelimiter( Tds::Member::Semicolon );
    }

    // Store
    cl.addAttribute( atr );
}

void Cp3Parser::processQuickList(Tds::Method & mth)
{
    std::string init;

    init.clear();

    // Skip colon
    lex->advance();

    do {
        if ( !init.empty() ) {
            lex->advance();
            lex->skipDelim();
            init += ',';
            init += ' ';
        }

        // Get variable
        init += lex->getToken();
        lex->skipDelim();

        // Get init
        if ( lex->getCurrentChar() == '(' ) {
            lex->advance();
            init += '(';
            init += lex->getLiteral( ")" );
            init += ')';
            lex->skipDelim();
        }
        else throwSyntaxError( "expected an init value in brackets" );
    } while( lex->getCurrentChar() == ',' );

    mth.setQuickInitList( init );
}

std::string Cp3Parser::readBody(FileLexer &l)
{
    std::string toret;
    int nestingLevel = 0;
    int ch;

    ch = l.getCurrentChar();
    while(  !( ch == '}'
         && nestingLevel == 0 )
         && !l.isEnd() )
    {
        if ( l.wasEol() ) {
            for(unsigned int i = 0; i < l.getNumBlankLinesSkipped(); ++i) {
                toret += '\n';
            }
        }
        toret += ch;

        if ( ch == '{' ) {
            ++nestingLevel;
        }
        else
        if ( ch == '}' ) {
            --nestingLevel;
        }

        l.advance();
        ch = l.getCurrentChar();
    }

    // Skip ending curl
    l.advance();

    return toret;
}

void Cp3Parser::processMethod(Tds::Class &cl, Tds::Method &mth)
{
    std::string body;
    std::string args;

    // Get params
    if ( lex->getCurrentChar() == '(' )
    {
        lex->advance();
        args = lex->getLiteral( ")" );
        lex->skipDelim();

        // Process const, if exists
        if ( lex->getToken() == Tds::Member::MdfConst ) {
            mth.setConstMethod();
        }

        // Process ';', if exists
        lex->skipDelim();
        if ( lex->getCurrentChar() == Tds::Member::Semicolon[ 0 ] ) {
            lex->advance();
            lex->skipDelim();
            mth.setPureVirtual();
        }
        // Process '=0' if exists
        else
        if ( lex->getCurrentChar() == '=' ) {
            lex->advance();
            lex->skipDelim();

            if ( lex->getCurrentChar() == '0' ) {
                lex->advance();
                skipDelimiter( Tds::Member::Semicolon );
                mth.setPureVirtual();
            }
            else throwSyntaxError( "'=0;' expected" );
        }
        else {
            // Process quick init list, if exists
            lex->skipDelim();
            if ( lex->getCurrentChar() == ':' ) {
                processQuickList( mth );
            }

            // Process body
            lex->skipDelim();
            if ( lex->getCurrentChar() == '{' ) {
                lex->advance();
                body = readBody( *lex );
                lex->skipDelim();
            } else throwSyntaxError( "expected '{' for body" );
        }
    } else throwSyntaxError( "expected '(' for parameters" );

    // Store
    mth.setBody( body );
    mth.setParameters( args );
    cl.addMethod( mth );
}


void Cp3Parser::processClassMember()
/// Add a new member to the current class
{
    const unsigned int numLine = getNumLine();
    const std::string * storage = NULL;
    const std::string * modifier = NULL;
    const std::string * type = NULL;
    std::string userType;
    std::string name;
    bool isPointer = false;
    bool isReference = false;
    bool isDestructor = false;
    Tds::Member::Modifiers mdfs;
    std::string & token = lex->getCurrentToken();

    if ( module.getState() == Tds::Module::ClassLevel )
    {
        Tds::Namespace * ns = module.getCurrentNamespace();
        if ( ns == NULL ) {
            throwSyntaxError( "there is still no namespace here" );
        }

        Tds::Class * cl = ns->getCurrentClass();
        if ( cl == NULL ) {
            throwSyntaxError( "there is still no class here" );
        }

        // Read storage
        storage = Tds::Member::lookForStorageKeyword( token );
        if ( storage != NULL )
        {
            lex->getToken();
        }

        // Determine whether it is a destructor
        if ( lex->getCurrentChar() == '~' ) {
            lex->advance();
            isDestructor = true;
            lex->getToken();

            if ( token != cl->getName() ) {
                throwSyntaxError( "invalid destructor" );
            }
        }
        else {
            // Read modifiers
            mdfs.clear();
            modifier = Tds::Member::lookForModifierKeyword( token );
            while ( modifier != NULL )
            {
                mdfs.push_back( modifier );
                modifier = Tds::Member::lookForModifierKeyword( lex->getToken() );
            }
        }

        if ( token != cl->getName() ) {
            // Read type
            type = Tds::Member::lookForTypeKeyword( token );
            if ( type == NULL )
            {
                // Go backwards to the beginning and take type
                lex->advance( token.length() * -1 );
                userType = getReference();
            }

            // Read member name
            lex->skipDelim();

            if ( lex->getCurrentChar() == Tds::Member::MdfIsPointer[ 0 ] ) {
                lex->advance();
                isPointer = true;
            }

            if ( lex->getCurrentChar() == Tds::Member::MdfIsReference[ 0 ] ) {
                lex->advance();
                isReference = true;
            }

            name = lex->getToken();
        } else {
            if ( isDestructor ) {
                name += '~';
            }

            name += token;

            type = &Tds::Member::VoidType;
        }

        if ( name.empty() )
        {
            throwSyntaxError( "invalid id: missing member name" );
        }
        else
        if ( isKeyword( name ) )
        {
            throwSyntaxError( "invalid id: keyword chosen" );
        }

        // Now, decide whether it is a member function or member field.
        lex->skipDelim();
        Tds::Member * member = NULL;
        if ( lex->getCurrentChar() == '='
          || lex->getCurrentChar() == ';' )
        {
            Tds::Attribute * atr;

            if ( type == NULL )
                    atr = new Tds::Attribute( numLine, name, userType );
            else    atr = new Tds::Attribute( numLine, name, type );

            atr->setModifiers( mdfs );
            atr->setStorage( storage );
            atr->setVisibility( cl->getCurrentVisibility() );

            processField( *cl, *atr );
            member = atr;
        }
        else
        if ( lex->getCurrentChar() == '(' )
        {
            Tds::Method * mth;

            if ( type == NULL )
                    mth = new Tds::Method( numLine, name, userType );
            else    mth = new Tds::Method( numLine, name, type );

            mth->setModifiers( mdfs );
            mth->setStorage( storage );
            mth->setVisibility( cl->getCurrentVisibility() );

            processMethod( *cl, *mth );
            member = mth;
        }
        else throwSyntaxError( "invalid member: expected function or field" );

        // Write it
        member->setIsPointer( isPointer );
        member->setIsReference( isReference );
        member->chk();

        if ( member->getSystemStorage() == &Tds::Member::InlineStorage ) {
            writeNumLineInfo( outputHeader, *lex, member->getLineNumber() );
            outputHeader->writeLn( member->getInline() );
        } else {
            const std::string &impl = member->getImplementation();

            if ( !impl.empty() ) {
                writeNumLineInfo( outputImpl, *lex, member->getLineNumber() );
                outputImpl->writeLn( impl );
            }

            writeNumLineInfo( outputHeader, *lex, member->getLineNumber() );
            outputHeader->writeLn( member->getPrototype() );
        }
    }
    else throwSyntaxError( "Misplaced member beginning" );
}

void Cp3Parser::processNamespaceMember()
{
    const unsigned int numLine = getNumLine();
    const std::string * storage;
    const std::string * modifier;
    const std::string * type;
    std::string userType;
    std::string name;
    bool isPointer = false;
    bool isReference = false;

    if ( module.getState() == Tds::Module::NamespaceLevel
      || module.getState() == Tds::Module::TopLevel )
    {
        // Look for storage
        Tds::Member::Modifiers mdfs;

        // Read storage
        std::string & token = lex->getCurrentToken();
        storage = Tds::Member::lookForStorageKeyword( token );
        if ( storage != NULL )
        {
            lex->getToken();
        }

        // Read modifiers
        mdfs.clear();
        modifier = Tds::Member::lookForModifierKeyword( token );
        while ( modifier != NULL )
        {
            mdfs.push_back( modifier );
            modifier = Tds::Member::lookForModifierKeyword( lex->getToken() );
        }

        // Read type
        type = Tds::Member::lookForTypeKeyword( token );
        if ( type == NULL )
        {
            lex->advance( token.length() * -1 );
            userType = getReference();
        }

        // Read member name
        lex->skipDelim();

        if ( lex->getCurrentChar() == Tds::Member::MdfIsPointer[ 0 ] ) {
            lex->advance();
            isPointer = true;
        }

        if ( lex->getCurrentChar() == Tds::Member::MdfIsReference[ 0 ] ) {
            lex->advance();
            isReference = true;
        }

        name = lex->getToken();

        if ( name.empty() )
        {
            throwSyntaxError( "invalid id: missing member name" );
        }
        else
        if ( isKeyword( name ) )
        {
            throwSyntaxError( "invalid id: keyword chosen" );
        }

        // Now, decide whether it is a function or constant field.
        lex->skipDelim();
        Tds::Member * member = NULL;
        if ( lex->getCurrentChar() == '='
          || lex->getCurrentChar() == ';' )
        {
            Tds::Constant * cnst = NULL;

            if ( module.getState() == Tds::Module::NamespaceLevel ) {
                if ( type == NULL )
                        cnst = new Tds::Constant( numLine, name, userType );
                else    cnst = new Tds::Constant( numLine, name, type );

                cnst->setModifiers( mdfs );
                cnst->setStorage( storage );

                processConstant( *cnst );
                member = cnst;
            } else throwSyntaxError(
                    "constants must be defined at least inside a namespace"
            );
        }
        else
        if ( lex->getCurrentChar() == '(' )
        {
            Tds::Function * f = NULL;

            if ( type == &Tds::Member::IntType
              && name == Tds::Member::MainFunctionId
              && module.getState() == Tds::Module::TopLevel )
            {
                processMainFunction();
                goto End;
            }
            else {
                 if ( module.getState() == Tds::Module::NamespaceLevel ) {
                    if ( type == NULL )
                            f = new Tds::Function( numLine, name, userType );
                    else    f = new Tds::Function( numLine, name, type );

                    f->setModifiers( mdfs );
                    f->setStorage( storage );

                    processRegularFunction( *f );
                } else throwSyntaxError(
                    "functions must be defined at least inside a namespace"
                );
            }

            member = f;
        }
        else throwSyntaxError( "invalid member: expected function or constant" );

        // Write it
        member->setIsPointer( isPointer );
        member->setIsReference( isReference );
        member->chk();

        if ( member->getSystemStorage() == &Tds::Member::InlineStorage ) {
            writeNumLineInfo( outputHeader, *lex, member->getLineNumber() );
            outputHeader->writeLn( member->getInline() );
        }
        else {
            const std::string &impl = member->getImplementation();

            if ( !impl.empty() ) {
                writeNumLineInfo( outputImpl, *lex, member->getLineNumber() );
                outputImpl->writeLn( member->getImplementation() );
            }

            writeNumLineInfo( outputHeader, *lex, member->getLineNumber() );
            outputHeader->writeLn( member->getPrototype() );
        }
    }
    else throwSyntaxError( "misplaced member beginning" );

    End:
    return;
}

void Cp3Parser::processConstant(Tds::Constant &cnst)
{
    Tds::Namespace &ns = *module.getCurrentNamespace();

    std::string initValue;

    // Get init string
    if ( lex->getCurrentChar() == '=' )
    {
        lex->advance();
        lex->skipDelim();
        initValue = lex->getLiteral( Tds::Member::Semicolon );

        // Check availability of initialization
        if ( cnst.lookForModifier( Tds::Member::MdfConst ) == NULL )
        {
            throwSyntaxError( "namespace member field must be constant" );
        }

        cnst.setInitialValue( initValue );
    } else {
        // Skip semicolon
        skipDelimiter( Tds::Member::Semicolon );
    }

    // Store
    ns.addConstant( cnst );
}

void Cp3Parser::processRegularFunction(Tds::Function &f)
{
    Tds::Namespace &ns = *module.getCurrentNamespace();

    std::string body;
    std::string args;

    // Set visibility
    f.setVisibility( ns.getCurrentVisibility() );

    // Get params
    if ( lex->getCurrentChar() == '(' )
    {
        lex->advance();
        args = lex->getLiteral( ")" );
        lex->skipDelim();

        // Process body
        lex->skipDelim();
        if ( lex->getCurrentChar() == '{' ) {
            lex->advance();
            body = readBody( *lex );
            lex->skipDelim();
        }
        else throwSyntaxError( "expected '{' for body" );
    } else throwSyntaxError( "expected '(' for parameters" );

    // Store
    f.setBody( body );
    f.setParameters( args );
    ns.addFunction( f );
}

void Cp3Parser::processMainFunction()
{
    const unsigned int numLine = getNumLine();

    if ( lex->getCurrentChar() == '(' ) {
        lex->advance();
        std::string params = lex->getLiteral( ")" );

        lex->skipDelim();
        if ( lex->getCurrentChar() == '{' ) {
            lex->advance();
            std::string body = readBody( *lex );

            lex->skipDelim();

            // Register function main
            Tds::EntryPoint * fMain = new Tds::EntryPoint( numLine);
            fMain->setBody( body );
            fMain->setParameters( params );
            module.setEntryPoint( fMain );
            module.chk();
        }
        else throwSyntaxError( "missing main() body" );
    } else throwSyntaxError( "missing main() parameters" );
}

void Cp3Parser::processVisibility()
{
    Tds::Container * container;
    Tds::Namespace * ns = module.getCurrentNamespace();
    const std::string * vk = Tds::Member::lookForVisibilityKeyword( lex->getCurrentToken() );

    if ( vk != NULL )
    {
        if ( module.getState() == Tds::Module::NamespaceLevel
          || module.getState() == Tds::Module::ClassLevel )
        {
            if ( module.getState() == Tds::Module::NamespaceLevel )
                    container = ns;
            else    container = ns->getCurrentClass();

            // Change visibility
            container->setCurrentVisibility( vk );
            skipDelimiter( Tds::Class::Colon );
        }
        else throwSyntaxError( "misplaced visibility keyword" );
    }
    else throwSyntaxError( "visibility keyword expected" );
}

std::string Cp3Parser::getId()
{
    std::string toret = lex->getToken();
    lex->skipDelim();

    // Pass over '::'
    while ( lex->getCurrentChar() == Tds::Member::AccessOperator[ 0 ] )
    {
        lex->advance();
        if ( lex->getCurrentChar() == Tds::Member::AccessOperator[ 1 ] )
        {
            lex->advance();
            toret += Tds::Member::AccessOperator;
        }
        else throwSyntaxError( "\"::\" expected" );

        // Get next token
        toret += lex->getToken();
        lex->skipDelim();
    }

    return toret;
}

void Cp3Parser::processUsing()
{
    unsigned int pos;
    std::string &token = lex->getCurrentToken();

    // Start on 'using'
    if ( token != Tds::Module::RWordUsing )
    {
        lex->getToken();
    }

    if ( token == Tds::Module::RWordUsing )
    {
        if ( module.getState() == Tds::Module::TopLevel )
        {
            pos = lex->getCurrentPos();
            lex->getToken();

            if ( token == Tds::Module::RWordNamespace )
            {
                outputImpl->writeLn(
                    Tds::Module::RWordUsing
                    + ' '
                    + Tds::Module::RWordNamespace + ' '
                    + getId() + Tds::Member::Semicolon
                );
            }
            else
            {
                lex->reset( pos );

                outputImpl->writeLn(
                    Tds::Module::RWordUsing + " "
                    + getId()
                    + Tds::Member::Semicolon
                );
            }
        }
        skipDelimiter( Tds::Member::Semicolon );
    }
    else throwSyntaxError( "expected \"using\"" );
}

void Cp3Parser::processImport()
{
    if ( lex->getCurrentToken() == Tds::Module::RWordImport ) {

        if ( module.getState() != Tds::Module::TopLevel ) {
            throwSyntaxError( "misplaced import" );
        }

        std::string fileName = getReference();

        if ( !fileName.empty() ) {
            // Prepare filename
            fileName += AppInfo::CHeaderFilesExt;

            // Store it as a dependency
            module.addDependency( fileName );

            // Output it
            std::string outputLine =
                            Tds::Module::DirectiveMark
                          + Tds::Module::RWordInclude + ' '
            ;
            outputLine += Tds::Module::DoubleQuote + fileName + Tds::Module::DoubleQuote;
            outputHeader->writeLn( outputLine );

            skipDelimiter( Tds::Member::Semicolon );
        }
        else throwSyntaxError( "malformed import: file name expected" );
    }
}

void Cp3Parser::processTypedef()
{
    if ( lex->getCurrentToken() == Tds::Module::RWordTypedef ) {

        if ( module.getState() != Tds::Module::NamespaceLevel
          && module.getState() != Tds::Module::ClassLevel )
        {
            throwSyntaxError( "misplaced typedef" );
        }

        // Get it
        lex->skipDelim();
        std::string typedefContents = lex->getLiteral( Tds::Member::Semicolon );
        lex->skipDelim();

        // Output it
        std::string outputLine = Tds::Module::RWordTypedef + ' ';
        outputLine += typedefContents + Tds::Member::Semicolon;
        outputHeader->writeLn( outputLine );
    }
}

void Cp3Parser::processDirective()
{
    lex->advance( 1 );
    std::string token = lex->getToken();
    char delim[ 2 ];
    delim[ 1 ] = 0;

    // Is it an include ?
    if ( token == Tds::Module::RWordInclude )
    {
        if ( module.getState() != Tds::Module::TopLevel ) {
            throwSyntaxError( "misplaced include" );
        }

        lex->skipDelim();
        *delim = lex->getCurrentChar();

        // Look for the file name
        if ( *delim == '"'
          || *delim == '<' )
        {
            if ( *delim == '<' )
            {
                *delim = '>';
            }

            // Get the filename
            lex->advance( 1 );
            std::string iFileName = lex->getLiteral( std::string( delim ) );

            // Store it
            if ( *delim == '"' )
            {
                module.addDependency( iFileName );
            }

            // Change .cp3 for .h, if needed
            if ( FileMan::getExt( iFileName ) == AppInfo::Cp3FilesExt )
            {
                FileMan::replaceExtCnvt( iFileName, AppInfo::CHeaderFilesExt );
            }

            // Recreate the include in output file
            std::string outputLine = '#' + Tds::Module::RWordInclude + ' ';

            if ( *delim == '>' )
                    outputLine += '<';
            else    outputLine += '"';

            outputHeader->writeLn( outputLine + iFileName + delim );
        }
        else throwSyntaxError( "malformed include: file name expected" );
    }
    else throwSyntaxError( "no other directives than include allowed" );
}

void Cp3Parser::skipDelimiter(const std::string &delim)
{
    std::string::const_iterator it = delim.begin();
    std::string::const_iterator end = delim.end();

    lex->skipDelim();

    for(; it != end; ++it) {
        if ( lex->getCurrentChar() != *it ) {
            throwSyntaxError( ( std::string( "expected: " ) + delim ).c_str() );
        }

        lex->advance();
    }

    lex->skipDelim();
}

std::string Cp3Parser::getTypeReference()
{
    std::string toret;

    lex->skipDelim();

    // It can have a leading '::'
    if ( lex->getCurrentTokenType() == Lexer::SpecialCharacter
      && lex->getCurrentChar() == Tds::Member::AccessOperator[ 0 ] )
    {
        skipDelimiter( Tds::Member::AccessOperator );
        toret += Tds::Member::AccessOperator;
    }

    // Get first token
    lex->skipDelim();
    toret += lex->getToken();
    lex->skipDelim();

    // Read the remaining tokens
    while ( lex->getCurrentTokenType() == Lexer::SpecialCharacter ) {
        if ( lex->getCurrentChar() == ':' )
        {
            skipDelimiter( Tds::Member::AccessOperator );
            toret += Tds::Member::AccessOperator;
            lex->skipDelim();
        }
        else
        if ( lex->getCurrentChar() == '<' )
        {
            skipDelimiter( "<" );
            toret += "<";
            toret += lex->getLiteral( '>' );
        }
        else break;

        toret += lex->getToken();
        lex->skipDelim();
    }

    return toret;
}

std::string Cp3Parser::getReference()
{
    std::string toret;

    lex->skipDelim();

    // It can have a leading '::'
    if ( lex->getCurrentTokenType() == Lexer::SpecialCharacter
      && lex->getCurrentChar() == Tds::Member::AccessOperator[ 0 ] )
    {
        skipDelimiter( Tds::Member::AccessOperator );
        toret += Tds::Member::AccessOperator;
    }

    // Get first token
    lex->skipDelim();
    toret += lex->getToken();
    lex->skipDelim();

    // Read the remaining tokens
    while ( lex->getCurrentTokenType() == Lexer::SpecialCharacter ) {
        if ( lex->getCurrentChar() == Tds::Member::DotOperator[0] ) {
            skipDelimiter( Tds::Member::DotOperator );
            toret += '.';
            lex->skipDelim();
        }
        else
        if ( lex->getCurrentChar() == ':' )
        {
            skipDelimiter( Tds::Member::AccessOperator );
            toret += Tds::Member::AccessOperator;
            lex->skipDelim();
        }
        else break;

        toret += lex->getToken();
        lex->skipDelim();
    }

    return toret;
}

void Cp3Parser::processNamespace()
{
    if ( lex->getCurrentToken() != Tds::Module::RWordNamespace )
    {
        lex->getToken();
    }

    if ( lex->getCurrentToken() == Tds::Module::RWordNamespace )
    {
        if ( module.getState() == Tds::Module::TopLevel
          || module.getState() == Tds::Module::NamespaceLevel )
        {
            Tds::Namespace * currentNS = module.getCurrentNamespace();
            const std::string * nsVisibility;

            Tds::Namespace * ns = module.addNamespace( getReference() );

            if ( currentNS != NULL ) {
                nsVisibility = currentNS->getCurrentVisibility();
                ns->setCurrentVisibility( nsVisibility );
            }

            outputHeader->writeLn( Tds::Module::RWordNamespace + ' '
                                   + ns->getName()
                                   + ' ' + Tds::Method::OpenBrace
                                 );
            skipDelimiter( Tds::Method::OpenBrace );
            module.resetState( Tds::Module::NamespaceLevel );
        }
        else throwSyntaxError( "misplaced namespace" );
    }
    else throwSyntaxError( "namespace expected" );
}

void Cp3Parser::processClass()
{
    Tds::Class::ParentList parentList;
    std::string visibility;
    const std::string * systemInhVisibility = NULL;
    const std::string * systemClsVisibility = NULL;
    Tds::Namespace * ns = module.getCurrentNamespace();

    if ( ns == NULL ) {
        throwSyntaxError( "misplaced class: namespace not yet defined" );
    }

    // Stablish correct visbility
    Tds::Container * cnt = ns->getCurrentClass();

    if ( cnt == NULL ) {
        cnt = ns;
    }

    systemClsVisibility = cnt->getCurrentVisibility();

    // Move on to the 'class' keyword
    if ( lex->getCurrentToken() != Tds::Module::RWordClass )
    {
        lex->getToken();
    }

    if ( lex->getCurrentToken() == Tds::Module::RWordClass )
    {
        // Read class name and parent class
        std::string currentClassName = lex->getToken();
        std::string currentClassParentName;
        lex->skipDelim();

        if ( lex->getCurrentChar() == Tds::Member::Semicolon[ 0 ] )
        {
            module.addDependency( currentClassName );
            currentClassName.clear();
            goto end;
        }
        else
        if ( lex->getCurrentChar() == ':' )
        {
            // Process inheritance parents
            do {
                lex->advance();
                visibility = lex->getToken();
                systemInhVisibility = Tds::Member::lookForVisibilityKeyword( visibility );
                if ( systemInhVisibility != NULL ) {
                    currentClassParentName = lex->getToken();

                    parentList.push_back( Tds::Class::Parent( currentClassParentName, systemInhVisibility ) );
                }
                else throwSyntaxError( "expected parent class visibility" );

                lex->skipDelim();
            } while( lex->getCurrentChar() == ',' );
        }

        // Finish class input
        skipDelimiter( Tds::Method::OpenBrace );

        // Store metaclass
        Tds::Class * currentClass = ns->addClass( currentClassName );
        module.resetState( Tds::Module::ClassLevel );

        currentClass->setClassVisibility( systemClsVisibility );
        if ( parentList.size() > 0 ) {
            currentClass->setParentList( parentList );
        }

        // Produce class
        outputHeader->write( currentClass->getDeclaration() );
    }
    else throwSyntaxError( ( "expected: " + Tds::Module::RWordClass ).c_str() );

    end:
    return;
}

void Cp3Parser::throwSyntaxError(const char * msg)
{
    throw SyntaxError( msg, getNumLine() );
}

void Cp3Parser::saveToFile(const std::vector<std::string> &v, const std::string &f)
{
    if ( !v.empty() )
    {
        std::vector<std::string>::const_iterator it = v.begin();
        OutputFile file( f );

        if ( file.isOpen() )
        {
            for (; it != v.end(); ++it)
            {
                file.writeLn( *it );
            }
        }
        else throw std::runtime_error( "unable to create '" + f + " 'file" );
    }
}

void Cp3Parser::updateNumLineInfo(FileLexer &lex, OutputFile *f)
{
    f->writeLn( getNumLineInfo( lex ) );
}

std::string Cp3Parser::getNumLineInfo(FileLexer &lex, unsigned int numLine)
{
    static std::string onlyFileName = FileMan::getFileName( lex.getFileName() );

    if ( numLine == 0 ) {
        numLine = lex.getLineNumber();
    }

    std::string toret = "#line "
                               + StringMan::toString( numLine )
                               + " \""
                               + onlyFileName
                               + '"'
    ;

    return toret;
}

void Cp3Parser::writeNumLineInfo(OutputFile *f, FileLexer &lex, unsigned int l)
{
        f->writeLn( getNumLineInfo( lex, l ) );
}

}

}
