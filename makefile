CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS = -lcurl
SRC = main.cpp

ifeq ($(findstring mingw,$(CXX)),mingw)
  TARGET = cf_ddns.exe
else
  TARGET = cf_ddns
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS)  -o $@ $^ $(LIBS)

json.hpp:
	curl -L -o json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp

clean:
	rm -f $(TARGET)
