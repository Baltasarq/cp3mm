#ifndef __MODULE__SHORTPERSON_
#define __MODULE__SHORTPERSON_
#line 2 "shortperson.mpp"
#include <string>
#line 3 "shortperson.mpp"
#include <iostream>
#line 4 "shortperson.mpp"
#include <sstream>
#line 5 "shortperson.mpp"
#include <cstdlib>
#line 7 "shortperson.mpp"
namespace Person {
#line 9 "shortperson.mpp"
class Person { 
#line 11 "shortperson.mpp"
public:  Person (const std::string &n, unsigned int ph);
#line 14 "shortperson.mpp"
public:  unsigned int getPhone () const;
#line 16 "shortperson.mpp"
public:  const std::string& getName () const;
#line 18 "shortperson.mpp"
public:  std::string toString () const;
#line 27 "shortperson.mpp"
private:  std::string name;
#line 28 "shortperson.mpp"
private:  unsigned int phone;
}; // class Person
#line 31 "shortperson.mpp"
} // namespace Person
#line 31 "shortperson.mpp"
#endif // module shortperson

