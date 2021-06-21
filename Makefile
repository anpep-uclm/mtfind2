CXXFLAGS += -Iinclude -pthread -lrt -std=c++20

all: mtfind2

mtfind2: src/SearchService.cpp src/Client.cpp src/mtfind2.cpp
	${CXX} ${CXXFLAGS} $^ -o $@

test:
	./mtfind2

clean:
	rm -f mtfind2 *.d

.PHONY: all clean
