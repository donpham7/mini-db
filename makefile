CXX = g++
CXXFLAGS = -std=c++17 -I include -Wall -Wextra -g
LDFLAGS = 

SRC = src/main.cpp lib/Page.cpp lib/disk_manager.cpp lib/buffer_pool.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = mini_db

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean