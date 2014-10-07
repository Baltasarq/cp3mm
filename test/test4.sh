for((i=0;i<100;++i)); do
	./cp3 Person2.mpp > /dev/null;
	g++ Person2.cpp -o person2;
done
