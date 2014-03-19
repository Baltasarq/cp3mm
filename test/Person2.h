#ifndef __MODULE__PERSON2_
#define __MODULE__PERSON2_
#line 6 "Person2.mpp"
#include <iostream>
#line 7 "Person2.mpp"
#include <string>
#line 9 "Person2.mpp"
#line 9 "Person2.mpp"
#line 10 "Person2.mpp"
#include "Ente.h"
#line 12 "Person2.mpp"
namespace BussinessLogic {
#line 14 "Person2.mpp"
class Person : public Ente { 
#line 16 "Person2.mpp"
public: static const std::string CanonicalName;
#line 18 "Person2.mpp"
private:  std::string name;
#line 19 "Person2.mpp"
private:  unsigned int age;
#line 20 "Person2.mpp"
private:  std::string email;
#line 21 "Person2.mpp"
private:  double salary;
#line 23 "Person2.mpp"
public:  explicit Person (const std::string &n, unsigned int a, const std::string &e);
#line 26 "Person2.mpp"
public:  Person (const std::string &n);
#line 30 "Person2.mpp"
public:  int getAge () const
{ return age; }
#line 32 "Person2.mpp"
public:  void setAge (int a);
#line 35 "Person2.mpp"
public:  const std::string& getEmail () const
{ return email; }
#line 37 "Person2.mpp"
public:  void setEmail (const std::string &e);
#line 40 "Person2.mpp"
public:  const std::string& getName () const
{ return name; }
#line 42 "Person2.mpp"
public:  void setName (const std::string &n);
#line 48 "Person2.mpp"
public:  std::string toString () const;
}; // class Person
#line 54 "Person2.mpp"
} // namespace BussinessLogic
#line 54 "Person2.mpp"
#endif // module Person2

