CXX=g++
CXXFLAGS=-O3 -Wall -g -std=c++17 -DDEBUG

TARGET=any

SOURCES=any.cpp main.cpp

OBJECTS=$(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
