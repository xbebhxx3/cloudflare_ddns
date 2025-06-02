CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LIBS = -lcurl
SRC = main.cpp

ifeq ($(OS),Windows_NT)
  TARGET = cf_ddns.exe
else
  TARGET = cf_ddns
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS)  -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
