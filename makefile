CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS = -lcurl
TARGET = cf_ddns
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS)  -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
