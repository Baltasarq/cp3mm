#include "Person.h"
using namespace std;
#line 18 "Person.mpp"
 const std::string BussinessLogic::Person::CanonicalName = "John Doe";
#line 25 "Person.mpp"
 BussinessLogic::Person::Person (const std::string &n, unsigned int a, const std::string &e) :name(n), age(a), email(e)
{}
#line 28 "Person.mpp"
 BussinessLogic::Person::Person (const std::string &n) :name( n )
{}
#line 34 "Person.mpp"
 void BussinessLogic::Person::setAge (int a)
{ age = a; }
#line 39 "Person.mpp"
 void BussinessLogic::Person::setEmail (const std::string &e)
{ email = e; }
#line 44 "Person.mpp"
 void BussinessLogic::Person::setName (const std::string &n)
{
if ( !n.empty() ) {
name = n;
}}
#line 76 "Person.mpp"
 int main ()
{
BussinessLogic::Person p1( BussinessLogic::Person::CanonicalName, 0, "" );
BussinessLogic::Person p2( "Baltasar", 34, "jbgarcia@uvigo.es" );

p2.putSalary( 5500 );

cout << "\nCp3 test\n" << endl;
cout << p1.getName() << endl;
cout << p2.getName() << endl;
cout << "Salary: " << p2.getSalary() ;
cout << '(' << p2.getNetSalary() << ')' << endl;
cout << endl;}

