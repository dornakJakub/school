CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

TARGET = ipk-L4-scan
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

# build
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) -lpcap

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean
clean:
	rm -f $(OBJ) $(TARGET)

# nix
NixDevShellName:
	@echo c

.PHONY: all clean test NixDevShellName