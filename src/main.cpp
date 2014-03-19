// cp3 - C++ Preprocessor
/*
    cp3 - A C++ module manager by means of a preprocessor

    baltasarq@yahoo.es
*/

#include "appinfo.h"
#include "fileio.h"
#include "fileman.h"
#include "cp3parser.h"

#include <cstdio>
#include <string>
#include <stdexcept>

const std::string OptForce   = "force";
const std::string OptVersion = "version";
const std::string OptVerbose = "verbose";
const std::string OptHelp    = "help";
const std::string OptLevel   = "level=";

const std::string MsgHelp =
    "cp3 [options] <filename>\n"
    "\t--help   \tShows this message and exits\n"
    "\t--version\tShows version and copyright information and exits\n"
    "\t--force  \tGenerate files ignoring timestap check\n"
    "\t--level=x  \tPuts strictness of the preprocessor at level x (=1,2,3)\n"
;


void processOptions(
                        unsigned int &firstArg,
                        const char *argv[], int &argc,
                        bool &force, bool &help, bool &version, bool &verbose,
                        Cp3mm::Tds::Entity::Strictness &strict)
{
    unsigned int lengthErase = 1;

    for(firstArg = 1; firstArg < (unsigned int) argc; ++firstArg) {
        std::string opt = argv[ firstArg ];

        // Maybe it is time to exit
        if ( opt[ 0 ] != '-' ) {
            break;
        }

        // Erase all '-'
        lengthErase = 1;
        if ( opt[ 1 ] == '-' ) {
            lengthErase = 2;
        }
        opt.erase( 0, lengthErase );

        // Look for option
        StringMan::minsCnvt( opt );

        if ( opt == OptHelp ) {
             help = true;
            --argc;
        }
        else
        if( opt == OptVersion ) {
            version = true;
            --argc;
        }
        else
        if ( opt == OptForce ) {
            force = true;
            --argc;
        }
        else
        if( opt == OptVerbose ) {
            verbose = true;
            --argc;
        }
        else
        if ( opt.substr( 0, OptLevel.length() )== OptLevel ) {
            strict = (Cp3mm::Tds::Entity::Strictness) ( ( opt[ opt.length() -1 ] - '0' ) - 1 );
            --argc;
        }
        else throw std::runtime_error( "invalid option" );
    }
}

bool isUpdated(
    const InputFile &in,
    const std::string &outHeaderName,
    const std::string &outImplName)
{
    bool toret = false;

    InputFile out1( outHeaderName );
    InputFile out2( outImplName );

    if ( in.isOpen()
      && out1.isOpen()
      && out2.isOpen() )
    {
        toret = ( out1.getTimeStamp() >= in.getTimeStamp()
              &&  out2.getTimeStamp() >= in.getTimeStamp() )
        ;
    }

    return toret;
}

int main(int argc, const char * argv[])
{
    std::string inputFileName;
    std::auto_ptr<Cp3mm::Parser::Cp3Parser> parser;
    bool force = false;
    bool help = false;
    bool version = false;
    bool verbose = false;
    unsigned int firstArg = 1;
    Cp3mm::Tds::Entity::Strictness strictness = Cp3mm::Tds::Entity::MediumStrictness;

    try {
        // Welcome
        printf( "%s", Cp3mm::AppInfo::TitleMessage.c_str() );
        printf( " (%s %s)\n\n", Cp3mm::AppInfo::Name.c_str(), Cp3mm::AppInfo::Version.c_str() );

        // Explore and answer command-line options
        processOptions( firstArg, argv, argc, force, help, version, verbose, strictness );

        if ( version ) {
            printf( "%s (%s) by %s - %s\n\n",
                    Cp3mm::AppInfo::Name.c_str(), Cp3mm::AppInfo::Version.c_str(),
                    Cp3mm::AppInfo::Author.c_str(), Cp3mm::AppInfo::Copyright.c_str()
            );
            goto End;
        }

        if ( help ) {
            printf( "%s\n", MsgHelp.c_str() );
            goto End;
        }

        // Report, if needed
        if ( strictness == Cp3mm::Tds::Entity::ErroneousStrictness ) {
            strictness = Cp3mm::Tds::Entity::MediumStrictness;
        }

        if ( verbose ) {
            printf( "Force: %s\tStrict level: %s\n\n",
                    force? "yes" : "no",
                    Cp3mm::Tds::Entity::cnvtStrictnessToString( strictness ).c_str()
            );
        }

        // Process parameter
        if ( argc == 2 ) {
            inputFileName = argv[ firstArg ];

            // Only process it provided it has the correct extension
            if ( Cp3mm::AppInfo::isAcceptedExt( FileMan::getExt( inputFileName ) ) )
            {
                // Open input file
                InputFile inputFile( inputFileName );

                // Prepare output file names
                std::string outputHeaderName = FileMan::replaceExt( inputFileName, Cp3mm::AppInfo::CHeaderFilesExt );
                std::string outputImplName   = FileMan::replaceExt( inputFileName, Cp3mm::AppInfo::CppFilesExt );
                std::string outputDepsName   = FileMan::replaceExt( inputFileName, Cp3mm::AppInfo::DepFilesExt );

                // Chk if anything really needs to be done
                if ( !force
                  && isUpdated( inputFile, outputHeaderName, outputImplName ) )
                {
                    printf( "Skipping '%s' due to '%s' and '%s' being up to date.\n\n",
                                inputFileName.c_str(),
                                outputHeaderName.c_str(),
                                outputImplName.c_str()
                    );
                    goto End;
                }

                // Open output files
                OutputFile outHeader( outputHeaderName );
                OutputFile outImpl( outputImplName );

                // Process file
                parser.reset(
                    new Cp3mm::Parser::Cp3Parser( inputFile, outHeader, outImpl, strictness )
                );
                printf( "Processing( '%s' )...\n", inputFileName.c_str() );
                parser->process();

                // Finishing
                parser->saveDependenciesToFile( outputDepsName );
                printf( "Done( '%s' ).\n", outputImplName.c_str() );
            }
            else printf( "Skipped( '%s' ).\n", inputFileName.c_str() );
        }
        else throw std::runtime_error( "invalid number of arguments" );
    } catch(const Cp3mm::Parser::ParserError &e) {
        std::string statement = parser->getCurrentLine();
        unsigned int pos = 0;

        if ( parser->getCurrentLex() != NULL ) {
            statement = parser->getCurrentLine();
            pos = parser->getCurrentPos();
        }

        std::printf( "\n%s: %s at %d,%d\n\t%s\n\t%s:%d: '%s'\n",
                inputFileName.c_str(),
                e.getType(),
                e.getNumLine(),
                pos,
                statement.c_str(),
                inputFileName.c_str(),
                e.getNumLine(),
                e.what()
        );

        exit( EXIT_FAILURE );
    }
    catch(const Cp3mm::Tds::StrictnessError &e) {
        std::printf( "\nStrictness error: '%s'\n", e.what() );
        exit( EXIT_FAILURE );
    }
    catch(const Cp3mm::Tds::SemanticError &e) {
        std::printf( "\nSemantic error: '%s'\n", e.what() );
        exit( EXIT_FAILURE );
    }
    catch(const std::runtime_error &e) {
        std::printf( "\nError: '%s'\n", e.what() );
        exit( EXIT_FAILURE );
    }
    catch(const std::exception &e) {
        std::printf( "\nCRITICAL: '%s'\n", e.what() );
        exit( EXIT_FAILURE );
    }
    catch(...) {
        std::printf( "\nCRITICAL UNKNOWN ERROR\n" );
        exit( EXIT_FAILURE );
    }

    End:
    return EXIT_SUCCESS;
}
