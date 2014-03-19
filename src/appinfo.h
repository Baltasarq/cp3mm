#ifndef APPINFO_H_INCLUDED
#define APPINFO_H_INCLUDED

#include <string>

namespace Cp3mm {

/**
    Contains information about the application itself
*/
class AppInfo {
public:
    /// Application's name
    static const std::string Name;

    /// Basic explanation of the purpose of this application
    static const std::string TitleMessage;

    /// Application's version
    static const std::string Version;

    /// Application's author
    static const std::string Author;

    /// Application's copyright message
    static const std::string Copyright;

    /// Extension for files to be processed by Cp3mm
    static const std::string Cp3FilesExt;

    /// Standard extension for modules in C++ -- to be accepted as well
    static const std::string StdMdlFilesExt;

    /// File extension for C++ header files
    static const std::string CHeaderFilesExt;

    /// File extension for C++ implementation files
    static const std::string CppFilesExt;

    /// File extension for dependency files
    static const std::string DepFilesExt;

    /// Vector of string pointers for accepted file extensions
    static const std::string * AcceptedExts[];

    /** Determines whether the extension is accepted or not
        @param ext The extension to evaluate
        @return true if accepted, false otherwise
    */
    static bool isAcceptedExt(const std::string &ext);
};

}

#endif // APPINFO_H_INCLUDED
