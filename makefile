CC := g++
CFLAGS := -std=c++23 -Wall
LDFLAGS := -lSDL2

SRCS := main.cpp world.cpp graphics_handler.cpp preydator_math.cpp neural_network.cpp $(wildcard Entity/*.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

TARGET := preydator
DEBUG_TARGET := preydator_debug

.PHONY: all clean debug

all: $(TARGET)

debug: CFLAGS += -g
debug: $(DEBUG_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET) $(DEBUG_TARGET)
