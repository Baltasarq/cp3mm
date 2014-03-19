// cp3tds.cpp
/*
    Implementación de la tabla de símbolos del preprocesador Cp3mm
*/

#include "cp3tds.h"
#include "fileman.h"

#include <stdexcept>
#include <stack>

#include <iostream>
void chkTellMe(const char *str)
{
    std::cout << "\nChk: level "
              << Cp3mm::Tds::Entity::getStrictnessLevel()
              << " executing "
              << str
              << std::endl
    ;
}


namespace Cp3mm {

namespace Tds {

const std::string Module::RWordInclude   = "include";
const std::string Module::RWordImport    = "import";
const std::string Module::RWordUsing     = "using";
const std::string Module::RWordClass     = "class";
const std::string Module::RWordExtern    = "extern";
const std::string Module::RWordNamespace = "namespace";
const std::string Module::RWordTypedef   = "typedef";
const std::string Module::ModulePrefix   = "__MODULE__";

const std::string Module::DirectiveMark  = "#";
const std::string Module::DoubleQuote    = "\"";
const std::string Member::Semicolon      = ";";
const std::string Method::OpenBrace      = "{";
const std::string Member::AccessOperator = "::";
const std::string Member::DotOperator    = ".";
const std::string Class::Colon           = ":";

// Modifiers
const std::string Member::MdfConst       = "const";
const std::string Member::MdfVirtual     = "virtual";
const std::string Member::MdfUnsigned    = "unsigned";
const std::string Member::MdfExplicit    = "explicit";
const std::string Member::MdfIsPointer   = "*";
const std::string Member::MdfIsReference = "&";
const std::string * Member::Modifier[] = {
        &MdfVirtual, &MdfConst, &MdfUnsigned, &MdfExplicit,
        &MdfIsPointer, &MdfIsReference, NULL
};

// Visibility
const std::string Member::PrivateVisibility = "private";
const std::string Member::PublicVisibility = "public";
const std::string Member::ProtectedVisibility = "protected";
const std::string * Member::Visibility[] = {
        &PrivateVisibility, &ProtectedVisibility, &PublicVisibility, NULL
};

// Storage
const std::string Member::StaticStorage = "static";
const std::string Member::InlineStorage = "inline";
const std::string Member::VolatileStorage = "volatile";
const std::string Member::AutoStorage = "auto";
const std::string Member::FriendStorage = "friend";
const std::string * Member::Storage[] = {
        &StaticStorage, &InlineStorage, &VolatileStorage, &AutoStorage,
        &FriendStorage, NULL
};

const std::string Member::MainFunctionId = "main";

// Type
const std::string Member::VoidType = "void";
const std::string Member::IntType = "int";
const std::string Member::LongIntType = "long";
const std::string Member::DoubleType = "double";
const std::string Member::FloatType = "float";
const std::string Member::CharType = "char";
const std::string Member::BoolType = "bool";
const std::string * Member::Type[] = {
    &VoidType, &IntType, &LongIntType,
    &DoubleType, &FloatType, &CharType, &BoolType, NULL
};

// --------------------------------------------------------------------- Entity
Entity::Strictness Entity::strictness = Entity::MediumStrictness;

const std::string Entity::StrStrictness[] = {
    "Low strictness", "Medium strictness", "High strictness", "Erroneous Strictness",
};

const std::string &Entity::cnvtStrictnessToString(Strictness strictness)
{
    static const unsigned int MaxStrictness = (unsigned int) ErroneousStrictness;
    unsigned int i = (unsigned int) strictness;

    if ( i > MaxStrictness ) {
        i = MaxStrictness;
    }

    return  StrStrictness[ i ];
}

// ------------------------------------------------------------------ Container
Container::~Container()
{
    ContainerList::iterator it = containerList.begin();

    for(; it != containerList.end(); ++it) {
        delete *it;
    }

    containerList.clear();
}

void Container::setCurrentVisibility(const std::string *v)
{
    Member::lookForVisibilityKeyword( *v );

    if ( v != NULL )
            currentVisibility = v;
    else    throw SemanticError( "not a visibility keyword" );
}

Container * Container::addContainer(Container *c)
{
    currentContainer = c;
    containerList.push_back( c );
    return c;
}

void Container::removeVectorOfMembers(MembersList &l)
{
    MembersList::iterator it = l.begin();

    for(; it != l.end(); ++it) {
        delete *it;
    }

    l.clear();
}

// --------------------------------------------------------------------- Member
const std::string * Member::lookForModifier(const std::string &mdf) const
{
    const std::string * toret = NULL;
    const Modifiers &mdfs = getModifiers();

    for(unsigned int i  = 0; i < mdfs.size(); ++i) {
        if ( *mdfs[ i ] == mdf ) {
            toret = mdfs[ i ];
            break;
        }
    }

    return toret;
}

std::string Member::getInterfaceModifiersAsString() const
{
    std::string toret;

    const Modifiers &mdfs = getModifiers();
    Modifiers::const_iterator it = mdfs.begin();
    for(; it != mdfs.end(); ++it) {
        toret += ' ';
        toret += **it;
    }

    return toret;
}

std::string Member::getImplementationModifiersAsString() const
{
    std::string toret;

    const Modifiers &mdfs = getModifiers();
    Modifiers::const_iterator it = mdfs.begin();
    for(; it != mdfs.end(); ++it) {
        if ( *it != &Member::MdfExplicit ) {
            toret += ' ';
            toret += **it;
        }
    }

    return toret;
}

std::string Member::buildQualifiedName(Container * container)
{
    std::string toret;
    std::stack<std::string> ids;

    // Get the ids of all containers
    while( container != NULL ) {
        ids.push( container->getName() );
        container = container->getContainer();
    }

    // Compose the name
    while ( !ids.empty() ) {
        toret += ids.top() + AccessOperator;
        ids.pop();
    }

    toret += this->getName();

    return toret;
}

// --------------------------------------------------------------- ClassRelated
void ClassRelated::chk(const Member &m)
{
    const Class * cl = dynamic_cast<const Class *>( m.getContainer() );

    if ( cl == NULL ) {
        throw SemanticError( ( "class member "
                            + m.getName()
                            + " does not pertain to a class" ).c_str()
        );
    }
}

// ------------------------------------------------------------------ Attribute
void Attribute::chkBasic() const
{
    // Check it is inside a class
    ClassRelated::chk( *this );

    // Check legality of initial value
    if ( getSystemStorage() != &Member::StaticStorage
      && !( getInitialValue().empty() ) )
    {
        throw SemanticError( "attribute must be static in order to have an inital value" );
    }
}

void Attribute::chkLow() const
{
    chkTellMe( "Attribute::chkLow" );
}

void Attribute::chkMedium() const
{
}

void Attribute::chkHigh() const
{
}

std::string Attribute::getPrototype()
{
    std::string toret;

    toret.clear();

    // Add visibility
    toret += getVisibility() + ": ";

    // Add storage
    if ( getSystemStorage() != NULL ) {
        toret += getStorage();
    }

    // Add modifiers
    toret += getInterfaceModifiersAsString();

    // Add type
    toret += ' ';
    toret += getType();
    toret += ' ';

    // Add pointer or reference marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += getName();
    toret += Method::Semicolon;

    return toret;
}

void Attribute::setInitialValue(const std::string &v)
{
    if ( getSystemStorage() != &Member::StaticStorage ) {
        throw SemanticError( "attribute must be static to have an inital value" );
    }

    Data::setInitialValue( v );
}

std::string Attribute::getImplementation()
{
    std::string toret;

    if ( isStatic() ) {
        const std::string &initValue = getInitialValue();

        // Add modifiers
        toret += getImplementationModifiersAsString();

        // Add type
        toret += ' ';
        toret += getType();

        // Add name
        toret += ' ';
        toret += getQualifiedName();

        // Add init value
        if ( !initValue.empty() ) {
            toret += " = " + initValue;
        }

        toret += Method::Semicolon;
    }

    return toret;
}

// --------------------------------------------------------------------- Method
void Method::chkBasic() const
{
    // Chk it is inside a class
    ClassRelated::chk( *this );

    // Chk various
    if ( isPureVirtual()
      && !getBody().empty() )
    {
        throw SemanticError( "a pure virtual function cannot have instructions" );
    }

    if ( isPureVirtual()
      && (  isConstructor()
         || isDestructor() ) )
    {
        throw SemanticError( "constructors/destructors cannot be pure virtual" );
    }

    if ( isVirtual()
      && isConstructor() ) {
        throw SemanticError( "constructors cannot be virtual" );
    }

    if ( isInline()
      && isPureVirtual() )
    {
        throw SemanticError( "inline functions cannot be pure virtual" );
    }
}

void Method::chkLow() const
{
}

void Method::chkMedium() const
{
}

void Method::chkHigh() const
{
}

bool Method::isConstructor() const
{
    if ( getContainer() == NULL ) {
        throw SemanticError( "member not assigned to any class" );
    }

    return ( getName() == getContainer()->getName() );
}

bool Method::isDestructor() const
{
    std::string name = getName().substr( 1 );

    if ( getContainer() == NULL ) {
        throw SemanticError( "member not assigned to any class" );
    }

    return ( getName()[ 0 ] == '~'
          && name == getContainer()->getName() );
}

bool Method::isPureVirtual() const
{
    return pureVirtualFunction;
}

void Method::setPureVirtual()
{
    if ( !getBody().empty() ) {
        throw SemanticError( "pure virtual functions cannot have implementation" );
    }

    pureVirtualFunction = true;

    if ( !isVirtual() ) {
        Modifiers mdfs = this->getModifiers();
        mdfs.push_back( &Member::MdfVirtual );
        this->setModifiers( mdfs );
    }
}

void Method::setBody(const std::string &b)
{
    if ( !getBody().empty() ) {
        throw SemanticError( "pure virtual functions cannot have implementation" );
    }

    Code::setBody( b );
}

void Method::processParameters(std::string &parameters)
{
    unsigned int pos = parameters.find( '=' );
    unsigned int posComma;

    // Remove the initializers
    while( pos != std::string::npos
        && pos < parameters.length() )
    {
        posComma = pos + 1;

        // Find the ',' next to the '='
        while( parameters[ posComma ] != ','
            && posComma < parameters.length() )
        {
            ++posComma;
        }

        // Remove '=' to ','
        parameters.erase( pos, posComma - pos );
    }

    return;
}

std::string Method::getPrototype()
{
    std::string toret;

    // Add visibility
    toret += getVisibility() + ": ";

    // Add storage
    if ( getSystemStorage() != NULL ) {
        toret += getStorage();
    }

    // Add modifiers
    toret += getInterfaceModifiersAsString();

    // Add type
    if ( !isConstructor()
      && !isDestructor() )
    {
        toret += ' ';
        toret += getType();
    }

    // Add reference or pointer marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += ' ';
    toret += getName();

    // Add args
    toret += ' ';
    toret += '(';
    toret += getParameters();
    toret += ')';

    // Add const mark
    if ( isConstMethod() ) {
        toret += ' ';
        toret += Member::MdfConst;
    }

    // Add virtual mark if needed
    if ( isPureVirtual() ) {
        toret += " = 0";
    }

    toret += Method::Semicolon;

    return toret;
}

std::string Method::getInline()
{
    std::string toret;

    if ( isPureVirtual() ) {
        throw SemanticError( "inline applied to pure virtual method" );
    }

    // Add visibility
    toret += getVisibility() + ':';
    toret += ' ';

    // Add modifiers
    toret += getImplementationModifiersAsString();

    // Add type
    if ( !isConstructor()
      && !isDestructor() )
    {
        toret += ' ';
        toret += getType();
    }

    // Add reference or pointer marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += ' ';
    toret += getName();

    // Add parameters
    toret += ' ';
    toret += '(';
    toret += getParameters();
    toret += ')';

    // Add const mark
    if ( isConstMethod() ) {
        toret += ' ';
        toret += Member::MdfConst;
    }

    // Add quicklist
    if ( !getQuickInitList().empty() )
    {
        toret += ' ';
        toret += ':';
        toret += getQuickInitList();
    }

    // Add body
    toret += '\n';
    toret += '{';
    toret += getBody();
    toret += '}';

    return toret;
}

std::string Method::getImplementation()
{
    std::string toret;
    std::string parameters;

    toret.clear();

    if ( isPureVirtual() ) {
        goto End;
    }

    // Add modifiers
    toret += getImplementationModifiersAsString();

    // Add type
    if ( !isConstructor()
      && !isDestructor() )
    {
        toret += ' ';
        toret += getType();
    }

    // Add reference or pointer marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += ' ';
    toret += getQualifiedName();

    // Add parameters
    parameters = getParameters();
    processParameters( parameters );

    toret += ' ';
    toret += '(';
    toret += parameters;
    toret += ')';

    // Add const mark
    if ( isConstMethod() ) {
        toret += ' ';
        toret += Member::MdfConst;
    }

    // Add quicklist
    if ( !getQuickInitList().empty() )
    {
        toret += ' ';
        toret += ':';
        toret += getQuickInitList();
    }

    // Add body
    toret += '\n';
    toret += '{';
    toret += getBody();
    toret += '}';

    End:
    return toret;
}

// ----------------------------------------------------------- NamespaceRelated
void NamespaceRelated::chk(const Member &m)
{
    const Namespace * ns = dynamic_cast<const Namespace *>( m.getContainer() );

    if ( ns == NULL ) {
        throw SemanticError(
            ( "namespace member '"
             + m.getName()
             + "' does not pertain to a namespace" ).c_str()
        );
    }

    if ( m.getSystemVisibility() == &Member::ProtectedVisibility ) {
        throw SemanticError( "member of namespace cannot have protected access" );
    }

    if ( ( m.getSystemVisibility() == &Member::PrivateVisibility
        || m.isStatic() )
      && m.getSystemStorage() == &Member::InlineStorage )
    {
        throw SemanticError( ( "Namespace member " + m.getName()
                + "cannot have both inline storage "
                "and private visibility (static)" ).c_str()
        );
    }
}

// ------------------------------------------------------------------- Constant
void Constant::chkBasic() const
{
    // Chk it is inside a namespace
    NamespaceRelated::chk( *this );

    // Chk presence of initial value
    if ( getInitialValue().empty() ) {
        throw SemanticError( ( getName() + " constant should have an initial value" ).c_str() );
    }
}

void Constant::chkLow() const
{
}

void Constant::chkMedium() const
{
}

void Constant::chkHigh() const
{
}

std::string Constant::getPrototype()
{
    std::string toret;

    toret = Module::RWordExtern;

    // Add storage
    if ( getSystemStorage() != NULL ) {
        toret += getStorage();
    }

    // Add modifiers
    toret += getInterfaceModifiersAsString();

    // Add type
    toret += ' ';
    toret += getType();
    toret += ' ';

    // Add pointer or reference marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += getName() + Method::Semicolon;

    return toret;
}

std::string Constant::getImplementation()
{
    std::string toret;

    // Add storage
    if ( getSystemStorage() != NULL ) {
        toret += getStorage();
    }

    // Add modifiers
    toret += getInterfaceModifiersAsString();

    // Add type
    toret += ' ';
    toret += getType();
    toret += ' ';

    // Add pointer or reference marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += getQualifiedName();

    // Add initial value
    const std::string &init = getInitialValue();
    if ( !init.empty() ) {
        toret += '=';
        toret += init;
    }

    toret += Method::Semicolon;

    return toret;
}

// ------------------------------------------------------------------- Function
void Function::chkBasic() const
{
    const std::string * storage = getSystemStorage();

    NamespaceRelated::chk( *this );

    if ( storage != NULL
      && !isStatic()
      && !isInline() )
    {
        throw SemanticError( std::string( "namespace function "
                        + getName()
                        + " should be static or inline, not "
                        + *storage ).c_str()
        );
    }
}

void Function::chkLow() const
{
}

void Function::chkMedium() const
{
}

void Function::chkHigh() const
{
}

std::string Function::getImplementation()
{
    std::string toret;
    const std::string * storage = getSystemStorage();

    toret.clear();

    // Add visbility
    if ( getSystemVisibility() == &Member::PrivateVisibility ) {
        setStorage( &Member::StaticStorage );
    }

    // Add storage
    if ( storage != NULL ) {
        toret += *storage;
    }

    // Add modifiers
    toret += getImplementationModifiersAsString();

    // Add type
    toret += ' ';
    toret += getType();

    // Add reference or pointer marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += ' ';
    toret += getQualifiedName();

    // Add parameters
    toret += ' ';
    toret += '(';
    toret += getParameters();
    toret += ')';

    // Add body
    toret += '\n';
    toret += '{';
    toret += getBody();
    toret += '}';

    return toret;
}

std::string Function::getPrototype()
{
    const std::string * storage;
    std::string toret;

    // Maybe it is the entry point
    if ( getContainer() == NULL
      && getName() == Member::MainFunctionId )
    {
        goto End;
    }

    // Add visibility
    if ( getSystemVisibility() == &Member::PrivateVisibility ) {
        setStorage( &Member::StaticStorage );
    }

    // Add storage
    storage = getSystemStorage();
    if ( storage != NULL ) {
        toret += *storage;
    }

    // Add modifiers
    toret += getInterfaceModifiersAsString();

    // Add type
    toret += ' ';
    toret += getType();

    // Add reference or pointer marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += ' ';
    toret += getName();

    // Add args
    toret += ' ';
    toret += '(';
    toret += getParameters();
    toret += ')';
    toret += Method::Semicolon;

    End:
    return toret;
}

std::string Function::getInline()
{
    std::string toret;

    // It's maybe the entry point
    if ( getContainer() == NULL
      && getName() == Member::MainFunctionId )
    {
        goto End;
    }

    // Add storage
    if ( getSystemStorage() != NULL ) {
        toret += getStorage();
    }

    // Add modifiers
    toret += getImplementationModifiersAsString();

    // Add type
    toret += ' ';
    toret += getType();

    // Add reference or pointer marks
    if ( isPointer() ) {
        toret += Member::MdfIsPointer;
    }
    else
    if ( isReference() ) {
        toret += Member::MdfIsReference;
    }

    // Add name
    toret += ' ';
    toret += getName();

    // Add parameters
    toret += ' ';
    toret += '(';
    toret += getParameters();
    toret += ')';

    // Add body
    toret += '\n';
    toret += '{';
    toret += getBody();
    toret += '}';

    End:
    return toret;
}

// ----------------------------------------------------------------- EntryPoint
EntryPoint::EntryPoint(unsigned int l) : Entity( Member::MainFunctionId )
{
        fn.reset( new Function( l, Member::MainFunctionId, Member::IntType ) );
}


std::string EntryPoint::getImplementation()
{
    return fn->getImplementation();
}

void EntryPoint::chkLow() const
{
}

void EntryPoint::chkMedium() const
{
}

void EntryPoint::chkHigh() const
{
}

void EntryPoint::chkBasic() const
{
    const std::string * storage = fn->getSystemStorage();

    if ( storage != NULL ) {
        if ( fn->isStatic()
          || fn->isInline() )
        {
            throw SemanticError( "main function should not be either static or inline" );
        }
        else throw SemanticError( "main function should not have any storage modifier" );
    }
}

// ---------------------------------------------------------------------- Class

Class::Parent::Parent(const std::string & n,const std::string * v)
            : parentName( n ), inheritanceVisibility( v )
{
    if ( v == NULL
      || n.empty() )
    {
        throw SemanticError(
            "parent name implies inheritance visibility and viceversa"
        );
    }

    if ( !Member::lookForVisibilityKeyword( *v ) ) {
        throw SemanticError( "invalid inheritance visibility" );
    }
}

Class::~Class()
{
    removeVectorOfMembers( attributes );
    removeVectorOfMembers( methods );
}

void Class::chkLow() const
{
}

void Class::chkMedium() const
{
}

void Class::chkHigh() const
{
}

void Class::chkBasic() const
{
}

std::string Class::getDeclaration()
{
    std::string toret;

    if ( dynamic_cast<Namespace *>( getContainer() ) == NULL ) {
        toret = *getClassVisibility();
        toret.push_back( ':' );
    }

    toret += Module::RWordClass + ' ' + getName();

    if ( parentList.size() > 0 ) {
        toret.append( Class::Colon );

        for( ParentList::const_iterator it = parentList.begin(); it != parentList.end(); ++it)
        {
            toret.append( it->toString() );

            if ( ( it + 1 ) != parentList.end() ) {
                toret.push_back( ',' );
            }
        }
    }
    toret.append( Method::OpenBrace );
    toret.push_back( '\n' );

    return toret;
}

void Class::setClassVisibility(const std::string * v)
{
    if ( Member::lookForVisibilityKeyword( *v ) != NULL ) {
        visibility =  v;
    }
}


Attribute &Class::addAttribute(Attribute &atr)
{
    atr.myContainer = this;
    attributes.push_back( &atr );
    return atr;
 }

Method &Class::addMethod(Method &mth)
{
    mth.myContainer = this;
    methods.push_back( &mth );
    return mth;
}

Class * Class::addClass(const std::string &ncl)
{
    Class * newClass = new Class( this, getModule(), ncl );
    addContainer( newClass );

    return newClass;
}

// ------------------------------------------------------------------ Namespace
Namespace::~Namespace()
{
    removeVectorOfMembers( constants );
    removeVectorOfMembers( functions );
}

Constant &Namespace::addConstant(Constant &cnst)
{
    cnst.myContainer = this;
    constants.push_back( &cnst );
    return cnst;
}

Class * Namespace::addClass(const std::string &n)
{
    if ( currentClass == NULL ) {
        ++numberOfClasses;
        currentClass = new Class( this, getModule(), n );
        addContainer( currentClass );
    }
    else {
        currentClass = currentClass->addClass( n );
    }

    return currentClass;
}

Container * Namespace::setOuterAsCurrentContainer()
{
    Container * toret = NULL;

    if ( currentClass != NULL ) {
        toret = currentClass->getContainer();

        if ( toret == this )
                currentClass = NULL;
        else    currentClass = (Class *) toret;
    }
    else toret = Container::setOuterAsCurrentContainer();

    return toret;
}

Namespace * Namespace::addNamespace(const std::string &n)
{
    Namespace * toret = new Namespace( this, getModule(), n );
    addContainer( toret );

    return toret;
}

Function &Namespace::addFunction(Function &f)
{
    f.myContainer = this;
    functions.push_back( &f );
    return f;
}

void Namespace::chkBasic() const
{
    const ContainerList & cntList = getContainerList();
    ContainerList::const_iterator it = cntList.begin();

    for(; it != cntList.end(); ++it) {
        (*it)->chk();
    }
}


void Namespace::chkLow() const
{
}

void Namespace::chkMedium() const
{
    if ( hasFunctions()
      && hasClass() )
    {
        throw StrictnessError( "functions and classes cannot share the same namespace" );
    }

    if ( getContainerList().size() > 1 ) {
        throw StrictnessError( "only one namespace allowed per namespace" );
    }
}

void Namespace::chkHigh() const
{
    unsigned int numContainers = getContainerList().size();

    if ( hasFunctions()
      && hasClass() )
    {
        throw StrictnessError( "functions and classes cannot share the same namespace" );
    }

    if ( numberOfClasses > 1 ) {
        throw StrictnessError( "no more than one class allowed per namespace" );
    }

    if ( numContainers > 0
      && numberOfClasses > 0
      && ( numContainers - numberOfClasses ) > 0
    )
    {
        throw StrictnessError( "one namespace can only hold another namespace or a class" );
    }

    if ( numContainers > 1 ) {
        throw StrictnessError( "only one namespace allowed per namespace" );
    }
}

bool Namespace::isInnerNamespace() const
{
    bool toret = true;
    const unsigned int numCnts = getContainerList().size();

    if ( numCnts > 0 ) {
        if ( numCnts > numberOfClasses ) {
            toret = false;
        }
    }

    return toret;
}


// --------------------------------------------------------------------- Module
const std::string Module::StandardHeaders =
    " algorithm bitset cassert assert.h cctype ctype.h cerrno errno.h cfloat float.h "
    " ciso646 iso646.h climits limits.h clocale locale.h cmath math.h complex csetjmp setjmp.h "
    " csignal signal.h cstdarg stdarg.h cstddef stddef.h cstdio stdio.h cstdlib stdlib.h "
    " cstring string.h ctime time.h deque exception fstream functional iomanip ios iosfwd "
    " iostream istream iterator limits list locale map memory new numeric ostream queue "
    " set sstream stack stddexcept strstream streambuf string typeinfo utility valarray vector "
;

Module::~Module()
{
    delete mainNamespace;
}

void Module::setName(const std::string &n)
{
    std::string name = n;

    StringMan::cambiarCaracteresCnvt( name, Member::DotOperator[ 0 ], '_' );
    StringMan::cambiarCaracteresCnvt( name, Member::AccessOperator[ 0 ], '_' );

    this->Entity::setName( name );
}

Namespace * Module::addNamespace(const std::string &nsName)
{
    if ( getCurrentNamespace() == NULL ) {
        mainNamespace = new Namespace( NULL, this, nsName );
        currentNamespace = mainNamespace;
    }
    else {
        currentNamespace = currentNamespace->addNamespace( nsName );
    }

    return currentNamespace;
}

Namespace * Module::setOuterAsCurrentNamespace()
{
    if ( currentNamespace != NULL ) {
        currentNamespace = currentNamespace->getContainer();
    }

    return currentNamespace;
}

void Module::chkBasic() const
{
    if ( getEntryPoint() == NULL
      && getCurrentNamespace() == NULL )
    {
        throw StrictnessError( "empty module?" );
    }
}

void Module::chkNamespace(const Namespace * ns) const
{
    if ( !ns->isInnerNamespace()
      && !ns->isEmpty() )
    {
        throw StrictnessError(
                ( "outer namespaces must be empty: "
                 + ns->getName() ).c_str()
        );
    }

    ns->chk();
}

void Module::chkLow() const
{

}

void Module::chkMedium() const
{

}

void Module::chkHigh() const
{
    const Namespace * ns = getMainNamespace();

    // Check all modules are okay. Semantic errors.
    if ( ns != NULL ) {
        // Check that outer namespaces are empty
        chkNamespace( ns );

        const Container::ContainerList &cntList = ns->getContainerList();
        Container::ContainerList::const_iterator it = cntList.begin();

        for (;it != cntList.end(); ++it)
        {
            if ( cntList.size() > 1 ) {
                throw StrictnessError(  "only one inner namespace allowed per namespace" );
            }

            ns = dynamic_cast<Namespace *>( *cntList.begin() );

            if ( ns != NULL ) {
                chkNamespace( ns );
            }
        }
    }

    return;
}

void Module::addDependency(const std::string &fileName)
{
    // Is it a standard header ?
    std::string lookForHeader = ' ' + fileName + ' ';

    if ( StandardHeaders.find( lookForHeader ) == std::string::npos ) {
        // Store it
        dependencies.push_back( fileName );
    }
}

}

}
