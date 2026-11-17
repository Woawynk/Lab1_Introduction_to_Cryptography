CXX := g++
CXXFLAGS := -O3 -std=c++14 -Wall -Wextra -Wpedantic

SRC := .
LIB_DIR := ./lib/fips202

INCLUDES := -I$(SRC) -I$(LIB_DIR)

# Các file mã nguồn
U512_OBJS := u512.o modular_math.o

all: test_member1

u512.o: u512.cpp u512.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c u512.cpp -o u512.o

modular_math.o: modular_math.cpp modular_math.h u512.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c modular_math.cpp -o modular_math.o

test_member1: test_member1.cpp $(U512_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) test_member1.cpp $(U512_OBJS) -o test_member1

run_test: test_member1
	./test_member1

clean:
	rm -f *.o test_member1 main
