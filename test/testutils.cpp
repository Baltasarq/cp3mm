#include "testutils.h"
#line 15 "testutils.mpp"
 int main ()
{
std::string name;
std::string strSalary;
double salary;
Utils::Containers::Stack stack;

stack.push( 1 );
stack.push( 2 );
stack.push( 3 );
stack.push( 4 );

while ( !stack.isEmpty() ) {
std::cout << stack.getTop() << std::endl;
stack.pop();
}

name = "bAltAsAr";

Utils::String::toUpper( name );

salary = 2000;

std::cout << name
<< ", your net salary for this month ("
<< salary
<< ") will be: "
<< Utils::Math::removePercentage( salary, 0.15 )
<< std::endl
;

Utils::String::toLower( name );

std::cout
<< "Bye, "
<< name
<< std::endl
;}

