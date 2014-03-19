#include "Utils.String.h"
#line 12 "Utils.String.mpp"
static int Utils::String::charToUpper (int x)
{
return std::tolower( x );}
#line 18 "Utils.String.mpp"
static int Utils::String::charToLower (int x)
{
return std::tolower( x );}
#line 23 "Utils.String.mpp"
 void Utils::String::toUpper (std::string &s)
{
std::string::iterator it = s.begin();

for(; it != s.end(); ++it) {
*it = charToUpper( *it );
}}
#line 31 "Utils.String.mpp"
 void Utils::String::toLower (std::string &s)
{
std::string::iterator it = s.begin();

for(; it != s.end(); ++it) {
*it = charToLower( *it );
}}

