CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS = -lcurl
SRC = main.cpp

EXT ?=
TARGET = cf_ddns$(EXT)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS)  -o $@ $^ $(LIBS)

json.hpp:
	curl -L -o json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp

clean:
	rm -f $(TARGET)
