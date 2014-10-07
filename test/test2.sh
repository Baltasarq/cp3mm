for((i=0;i<100;++i)); do
	./cp3 Person.mpp > /dev/null;
	./cp3 Math.mpp > /dev/null;
	g++ Person.cpp Math.cpp -o person;
done
