CXX ?= g++
CXXFLAGS ?= -O2 -Wall
LDFLAGS ?=

SRCDIR := compressor/BitBuster2
TARGET := bitbuster2
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
