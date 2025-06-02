CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -O2
SRC = main.cpp
EXT ?=
TARGET = cf_ddns$(EXT)

# 判断是否为 Windows 平台
ifeq ($(OS),Windows_NT)
  STATICFLAGS =
  LIBS = -lcurl
  CPPFLAGS =
else
  STATICFLAGS = -static
  LIBS := $(shell pkg-config --static --libs libcurl)
  CPPFLAGS := $(shell pkg-config --cflags libcurl)
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(STATICFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
