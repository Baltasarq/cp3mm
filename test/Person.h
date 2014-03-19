#ifndef __MODULE__PERSON_
#define __MODULE__PERSON_
#line 6 "Person.mpp"
#include <iostream>
#line 7 "Person.mpp"
#include <sstream>
#line 8 "Person.mpp"
#include <string>
#line 10 "Person.mpp"
#include "Math.h"
#line 12 "Person.mpp"
#line 14 "Person.mpp"
namespace BussinessLogic {
#line 16 "Person.mpp"
class Person { 
#line 18 "Person.mpp"
public: static const std::string CanonicalName;
#line 20 "Person.mpp"
private:  std::string name;
#line 21 "Person.mpp"
private:  unsigned int age;
#line 22 "Person.mpp"
private:  std::string email;
#line 23 "Person.mpp"
private:  double salary;
#line 25 "Person.mpp"
public:  explicit Person (const std::string &n, unsigned int a, const std::string &e);
#line 28 "Person.mpp"
public:  Person (const std::string &n);
#line 32 "Person.mpp"
public:  int getAge () const
{ return age; }
#line 34 "Person.mpp"
public:  void setAge (int a);
#line 37 "Person.mpp"
public:  const std::string& getEmail () const
{ return email; }
#line 39 "Person.mpp"
public:  void setEmail (const std::string &e);
#line 42 "Person.mpp"
public:  const std::string& getName () const
{ return name; }
#line 44 "Person.mpp"
public:  void setName (const std::string &n);
#line 50 "Person.mpp"
public:  void putSalary (double x)
{
salary = x;}
#line 54 "Person.mpp"
public:  double getSalary () const
{
return salary;}
#line 58 "Person.mpp"
public:  double getNetSalary () const
{
return Math::removePercentage( salary, 0.20 );}
#line 62 "Person.mpp"
public:  virtual std::string toString () const
{
std::ostringstream out;

out << getName()
<< ", " << getSalary();

return out.str();}
}; // class Person
#line 74 "Person.mpp"
} // namespace BussinessLogic
#line 74 "Person.mpp"
#endif // module Person

