#include "Person2.h"
using namespace std;
using std::cout;
#line 16 "Person2.mpp"
 const std::string BussinessLogic::Person::CanonicalName = "John Doe";
#line 23 "Person2.mpp"
 BussinessLogic::Person::Person (const std::string &n, unsigned int a, const std::string &e) :name(n), age(a), email(e)
{}
#line 26 "Person2.mpp"
 BussinessLogic::Person::Person (const std::string &n) :name( n )
{}
#line 32 "Person2.mpp"
 void BussinessLogic::Person::setAge (int a)
{ age = a; }
#line 37 "Person2.mpp"
 void BussinessLogic::Person::setEmail (const std::string &e)
{ email = e; }
#line 42 "Person2.mpp"
 void BussinessLogic::Person::setName (const std::string &n)
{
if ( !n.empty() ) {
name = n;
}}
#line 48 "Person2.mpp"
 std::string BussinessLogic::Person::toString () const
{
return ( getName() + ": " + getEmail() );}
#line 56 "Person2.mpp"
 int main ()
{
BussinessLogic::Person p1( BussinessLogic::Person::CanonicalName, 0, "" );
BussinessLogic::Person p2( "Baltasar", 34, "jbgarcia@uvigo.es" );

cout << "\nCp3 test\n" << endl;
cout << p1.getName() << endl;
cout << p2.getName() << endl;
cout << endl;}

