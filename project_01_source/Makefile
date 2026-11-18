CXX := g++
CC  := gcc
CXXFLAGS := -O3 -std=c++14 -Wall -Wextra -Wpedantic
CFLAGS   := -O3 -Wall -Wextra

SRC := .
LIB_DIR := ./lib/fips202

INCLUDES := -I$(SRC) -I$(LIB_DIR)
WIN_LIBS :=
ifeq ($(OS),Windows_NT)
WIN_LIBS := -ladvapi32
endif

# Các đối tượng biên dịch
U512_OBJS := u512.o modular_math.o
FIPS_OBJS := fips202.o prng.o
DH_OBJS   := dh.o

all: main test_suite

fips202.o: $(LIB_DIR)/fips202.c $(LIB_DIR)/fips202.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(LIB_DIR)/fips202.c -o fips202.o

prng.o: $(LIB_DIR)/prng.cpp $(LIB_DIR)/prng.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(LIB_DIR)/prng.cpp -o prng.o

u512.o: u512.cpp u512.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c u512.cpp -o u512.o

modular_math.o: modular_math.cpp modular_math.h u512.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c modular_math.cpp -o modular_math.o

dh.o: dh.cpp dh.h u512.h modular_math.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c dh.cpp -o dh.o

# Biên dịch chương trình kiểm thử đơn vị
test_suite: test_suite.cpp $(U512_OBJS) $(DH_OBJS) $(FIPS_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) test_suite.cpp $(U512_OBJS) $(DH_OBJS) $(FIPS_OBJS) $(WIN_LIBS) -o test_suite

test_member1: test_suite
	@echo "test_member1 is now an alias for test_suite"

run_test: test_suite
	./test_suite

# Biên dịch chương trình chính Diffie-Hellman
main: main.cpp $(U512_OBJS) $(DH_OBJS) $(FIPS_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) main.cpp $(U512_OBJS) $(DH_OBJS) $(FIPS_OBJS) $(WIN_LIBS) -o main

run: main
	./main

clean:
	rm -f *.o test_suite test_member1 main test_suite.exe test_member1.exe main.exe
