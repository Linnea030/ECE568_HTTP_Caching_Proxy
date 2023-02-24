all: main

main: main.cpp proxy.cpp proxy.h csbuild.cpp csbuild.h package.cpp package.h socket_info.h
	g++ --std=c++11 -g  -o main main.cpp proxy.cpp proxy.h csbuild.cpp csbuild.h package.cpp package.h socket_info.h -lpthread

.PHONY:
	clean
clean:
	rm -rf *.o main *~ *#