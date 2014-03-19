#ifndef CP3PARSER_H_INCLUDED
#define CP3PARSER_H_INCLUDED

#include "fileio.h"
#include "lex.h"
#include "cp3tds.h"

#include <vector>
#include <string>
#include <stdexcept>

namespace Cp3mm {

namespace Parser {

/// Exceptions to be thrown by the parser
class ParserError : public std::runtime_error {
private:
    unsigned int numLine;
    const char * type;
public:
    /// Returns the line number for this error
    unsigned int getNumLine() const
        { return numLine; }

    /// Constructor for parser errors
    /// @param t The type of the error, as a char * string
    /// @param msg The string representing the message to show
    /// @param n The line number in which the error appeared
    ParserError(const char *t, const char * msg, unsigned int n)
        : std::runtime_error( msg ), numLine( n ), type( t )
        {}

    const char *getType() const
        { return type; }
};

/// Exceptions to be thrown for syntax errors
class SyntaxError : public ParserError {
public:
    /// Constructor for syntax errors
    /// @param msg The string representing the message to show
    /// @param n The line number in which the error appeared
    SyntaxError(const char * msg, unsigned int n)
        : ParserError( "Syntax error", msg, n )
        {}
};

/// The parser itself
class Cp3Parser {
private:
    std::auto_ptr<FileLexer> lex;
    InputFile  * inputFile;
    OutputFile * outputHeader;
    OutputFile * outputImpl;
    std::string onlyFileName;
    Tds::Module module;

    void throwSyntaxError(const char *);

    void writePreambles();
    void writeColophons();

    void processSpecialCharacter();
    void processEndings();
    void processDirective();
    void processMainFunction();
    void processUsing();
    void processImport();
    void processTypedef();
    void processClass();
    void processNamespace();
    void processVisibility();
    void processComments();
    void processClassMember();
    void processField(Tds::Class &cl, Tds::Attribute &atr);
    void processMethod(Tds::Class &cl, Tds::Method &mth);
    void processNamespaceMember();
    void processRegularFunction(Tds::Function &);
    void processConstant(Tds::Constant &);
    void processQuickList(Tds::Method & mth);

    std::string getId();
    std::string getReference();
    void skipDelimiter(const std::string &delim);
public:
    Cp3Parser(InputFile &, OutputFile &, OutputFile &, Tds::Entity::Strictness = Tds::Entity::MediumStrictness);

    void process();

    const std::string &getCurrentLine() const
        { return lex->getLine(); }
    unsigned int getCurrentPos() const
        { return lex->getCurrentPos(); }
    const Lexer * getCurrentLex() const
        { return lex.get(); }
    unsigned int getNumLine() const
        { return lex->getLineNumber(); }
    const Tds::Module &getModule() const
        { return module; }
    bool isKeyword(const std::string &);
    void updateNumLineInfo(OutputFile *f)
        { updateNumLineInfo( *lex, f ); }

    static void saveToFile(const std::vector<std::string> &v, const std::string &f);
    void saveDependenciesToFile(const std::string &f) const
        { saveToFile( module.getDependencies(), f ); }

    static void updateNumLineInfo(FileLexer &lex, OutputFile *f);
    static std::string getNumLineInfo(FileLexer &lex, unsigned int numLine = 0);
    static void writeNumLineInfo(OutputFile *f, FileLexer &lex, unsigned int l);
    static std::string readBody(FileLexer &l);
};

}

}

#endif // CP3PARSER_H_INCLUDED
