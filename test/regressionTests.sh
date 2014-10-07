# if ('$1'='') then $1="dbgCp3";

# Simple, standalone module
./$1 Ente.mpp
./$1 Person2.mpp
g++ Person2.cpp
./a.out

# Simple, multiple modules
./$1 Person.mpp
./$1 Math.mpp
g++ Person.cpp Math.cpp
./a.out

# More complex, multiple modules
./$1 testutils.mpp
./$1 Utils.String.mpp
./$1 Utils.Math.mpp
./$1 Utils.Containers.Stack.mpp
g++ testutils.cpp Utils.String.cpp Utils.Math.cpp Utils.Containers.Stack.cpp
./a.out
