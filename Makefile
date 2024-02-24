all: main

main: main.cpp proxy.cpp csbuild.cpp package.cpp handler.cpp cache.cpp
	g++ --std=c++11 -g  -o main main.cpp proxy.cpp csbuild.cpp package.cpp handler.cpp cache.cpp -lpthread

.PHONY:
	clean
clean:
	rm -rf *.o main *~ *#