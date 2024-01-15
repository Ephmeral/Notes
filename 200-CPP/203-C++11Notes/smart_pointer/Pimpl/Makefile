CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra

SRCS := Widget.cpp main.cpp
OBJS := $(SRCS:.cpp=.o)

TARGET := widget_app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
