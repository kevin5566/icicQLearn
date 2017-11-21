icicQL:Def.o main.o
	g++ -o icicQL Def.o main.o
Def.o:Def.cpp Def.h
	g++ -c -std=c++11 Def.cpp
main.o:main.cpp
	g++ -c -std=c++11 main.cpp

clean:
	rm -f icicQL Def.o main.o