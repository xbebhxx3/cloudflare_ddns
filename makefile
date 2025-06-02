CXX ?= g++
SRC = main.cpp
EXT ?=
TARGET = cf_ddns$(EXT)

ifeq ($(STATIC),1)
    CXXFLAGS = -std=c++17 -Wall -O2 -static -DCURL_STATICLIB
    LDFLAGS = -static -lcurl -lssl -lcrypto -lz -ldl
else
    CXXFLAGS = -std=c++17 -Wall -O2
    LDFLAGS = -lcurl -lssl -lcrypto -lz -ldl
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

json.hpp:
	curl -L -o json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp

clean:
	rm -f $(TARGET)
