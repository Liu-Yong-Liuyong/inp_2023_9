# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11

# Source files
SRCS = hw2_chat_server.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = hw2_chat_server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

run:
	@echo "Usage: ./hw2_chat_server [port_number]"
