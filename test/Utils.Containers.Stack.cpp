#include "Utils.Containers.Stack.h"
#line 12 "Utils.Containers.Stack.mpp"
 Utils::Containers::Stack::StackFrame::StackFrame (int x)
{ this->x = x; }
#line 14 "Utils.Containers.Stack.mpp"
 int Utils::Containers::Stack::StackFrame::getX ()
{ return x; }
#line 26 "Utils.Containers.Stack.mpp"
 bool Utils::Containers::Stack::push (const DataType &x)
{
bool toret = true;

if ( num < Max )
v[num++]=x;
else	toret = false;

return toret;}
#line 40 "Utils.Containers.Stack.mpp"
 bool Utils::Containers::Stack::pop ()
{
bool toret = true;

if ( num > 0 )
--num;
else	toret = false;

return toret;}

