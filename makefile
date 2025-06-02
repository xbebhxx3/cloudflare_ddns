CXX ?= g++
PKG_CONFIG ?= pkg-config
SRC = main.cpp
EXT ?=
TARGET = cf_ddns$(EXT)
CPPFLAGS ?=

ifeq ($(STATIC),1)
    CXXFLAGS = -std=c++17 -Wall -O2 -static -DCURL_STATICLIB
    PKG_LIBS := $(shell $(PKG_CONFIG) --libs libcurl)
    PKG_CFLAGS := $(shell $(PKG_CONFIG) --cflags libcurl)
    LDFLAGS = -static $(PKG_LIBS)
    CPPFLAGS += $(PKG_CFLAGS)
else
    CXXFLAGS = -std=c++17 -Wall -O2
    PKG_LIBS := $(shell $(PKG_CONFIG) --libs libcurl)
    PKG_CFLAGS := $(shell $(PKG_CONFIG) --cflags libcurl)
    LDFLAGS = $(PKG_LIBS)
    CPPFLAGS += $(PKG_CFLAGS)
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

json.hpp:
	curl -L -o json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp

clean:
	rm -f $(TARGET)
