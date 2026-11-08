CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS += -Isrc
.PHONY: all test check clean
all: build/server
build:
	mkdir -p build
build/server: src/server.cpp src/core.cpp src/core.h src/net.h src/logger.h | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/server.cpp src/core.cpp -o $@
build/test_modules: tests/test_modules.cpp src/core.cpp src/core.h src/net.h src/logger.h | build
	$(CXX) $(CPPFLAGS) -std=c++17 -O0 -g -Wall -Wextra -Wpedantic -UNDEBUG tests/test_modules.cpp src/core.cpp -o $@
test: build/test_modules
	./build/test_modules
check: test
clean:
	rm -rf build
