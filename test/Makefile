# set LD_LIBRARY_PATH
export CC  = clang
export CXX = clang++
export CXXFLAGS = -std=c++17 -Wall -O3 -I/usr/local/opt/llvm/include -I/usr/local/include
export LDFLAGS = -L/usr/local/lib $(brew info llvm | grep LDFLAGS= | cut -d = -f 2 | tr '"' ' ')
# specify tensor path
BIN = test_lazy_eval test_logger test_reflection test_pattern_matching test_tensor

.PHONY: clean all

all: $(BIN) 

test_lazy_eval: test_lazy_eval.cc

test_logger: test_logger.cc

test_reflection: test_reflection.cc

test_pattern_matching: test_pattern_matching.cc

test_tensor: test_tensor.cc

$(BIN) :
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.cpp %.o %.c %.cc, $^)

clean:
	rm -rf $(BIN) *~
