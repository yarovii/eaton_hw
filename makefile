CXX=g++
LD=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -O2

test: monitor.o testing.o
	$(LD) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o test


