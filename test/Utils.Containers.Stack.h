#ifndef __MODULE__UTILS_CONTAINERS_STACK_
#define __MODULE__UTILS_CONTAINERS_STACK_
#line 2 "Utils.Containers.Stack.mpp"
namespace Utils {
#line 3 "Utils.Containers.Stack.mpp"
namespace Containers {
#line 4 "Utils.Containers.Stack.mpp"
class Stack { 
#line 6 "Utils.Containers.Stack.mpp"
typedef int DataType;
#line 8 "Utils.Containers.Stack.mpp"
public: class StackFrame { 
#line 10 "Utils.Containers.Stack.mpp"
private:  int x;
#line 12 "Utils.Containers.Stack.mpp"
public:  StackFrame (int x);
#line 14 "Utils.Containers.Stack.mpp"
public:  int getX ();
}; // class StackFrame
#line 17 "Utils.Containers.Stack.mpp"
#line 18 "Utils.Containers.Stack.mpp"
private:  int *v;
#line 19 "Utils.Containers.Stack.mpp"
private:  int num;
#line 20 "Utils.Containers.Stack.mpp"
private:  int Max;
#line 22 "Utils.Containers.Stack.mpp"
public:  Stack (int max = 128) :Max(max), num(0)
{ v = new int[ Max ]; }
#line 24 "Utils.Containers.Stack.mpp"
public:  ~Stack ()
{ delete[] v; }
#line 26 "Utils.Containers.Stack.mpp"
public:  bool push (const DataType &x);
#line 36 "Utils.Containers.Stack.mpp"
public:  const DataType& getTop () const
{ return v[num - 1]; }
#line 38 "Utils.Containers.Stack.mpp"
public:  bool isEmpty ()
{ return ( num == 0 ); }
#line 40 "Utils.Containers.Stack.mpp"
public:  bool pop ();
}; // class Stack
#line 51 "Utils.Containers.Stack.mpp"
} // namespace Containers
#line 51 "Utils.Containers.Stack.mpp"
} // namespace Utils
#line 52 "Utils.Containers.Stack.mpp"
#endif // module Utils_Containers_Stack

