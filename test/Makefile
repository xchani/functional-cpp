# set LD_LIBRARY_PATH
export CC  = gcc
export CXX = g++
export CFLAGS = -std=c++14 -Wall -O3
# specify tensor path
BIN = test_lazy_eval

.PHONY: clean all

all: $(BIN) 

test_lazy_eval: test_lazy_eval.cc

$(BIN) :
	$(CXX) $(CFLAGS) -o $@ $(filter %.cpp %.o %.c %.cc, $^)

clean:
	rm -rf $(BIN) *~
