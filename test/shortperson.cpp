#include "shortperson.h"
#line 11 "shortperson.mpp"
 Person::Person::Person (const std::string &n, unsigned int ph) :name( n ), phone( ph )
{}
#line 14 "shortperson.mpp"
 unsigned int Person::Person::getPhone () const
{ return phone; }
#line 16 "shortperson.mpp"
 const std::string& Person::Person::getName () const
{ return name; }
#line 18 "shortperson.mpp"
 std::string Person::Person::toString () const
{
std::ostringstream out;

out << getName() << ", " << getPhone();

return out.str();}
#line 33 "shortperson.mpp"
 int main ()
{
Person::Person p( "Baltasar", 988387028 );
std::cout << p.toString() << std::endl;

return EXIT_SUCCESS;}

