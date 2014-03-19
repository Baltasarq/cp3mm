#include "appinfo.h"
/*
    Implementation of the AppInfo class
*/

const std::string Cp3mm::AppInfo::Name             = "Cp3--";
const std::string Cp3mm::AppInfo::TitleMessage     = "Cp3-- A module manager for C++";
const std::string Cp3mm::AppInfo::Version          = "v0.40 20100224";
const std::string Cp3mm::AppInfo::Author           = "jbgarcia@uvigo.es";
const std::string Cp3mm::AppInfo::Copyright        = "(c) Copyright Baltasar 2009-2010";
const std::string Cp3mm::AppInfo::Cp3FilesExt      = ".cp3";
const std::string Cp3mm::AppInfo::StdMdlFilesExt   = ".mpp";
const std::string Cp3mm::AppInfo::CHeaderFilesExt  = ".h";
const std::string Cp3mm::AppInfo::CppFilesExt      = ".cpp";
const std::string Cp3mm::AppInfo::DepFilesExt      = ".dep";
const std::string * Cp3mm::AppInfo::AcceptedExts[] = { &Cp3FilesExt, &StdMdlFilesExt, NULL };

bool Cp3mm::AppInfo::isAcceptedExt(const std::string &x)
{
    const std::string ** cursor = AcceptedExts;

    for(; *cursor != NULL; ++cursor ) {
        if ( **cursor == x ) {
            break;
        }
    }

    return ( *cursor != NULL );
}
