# Compiler and compiler flags
CC = gcc
CXX = g++
CFLAGS = -Wall -pthread
CXXFLAGS = -Wall -pthread -I/usr/local/include/opencv4
LDFLAGS = -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

# Source files for different targets
CLIENT_SRCS = send_an_image_client.cpp csapp.c
DISPLAY_SRCS = display.cpp
COMMON_SRCS = csapp.c

# Object files
OBJ_DIR = obj
CLIENT_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(CLIENT_SRCS:.c=.o)))
CLIENT_OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(CLIENT_OBJS:.cpp=.o)))
DISPLAY_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(DISPLAY_SRCS:.cpp=.o)))
COMMON_OBJS = $(addprefix $(OBJ_DIR)/,$(COMMON_SRCS:.c=.o))

# Your program's names
TARGET_CLIENT = send_an_image_client
TARGET_DISPLAY = display

# Default target
all: $(TARGET_CLIENT) $(TARGET_DISPLAY)

# Rule for building the client program
$(TARGET_CLIENT): $(CLIENT_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_CLIENT) $(CLIENT_OBJS) $(COMMON_OBJS) $(LDFLAGS)

# Rule for building the display program
$(TARGET_DISPLAY): $(DISPLAY_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_DISPLAY) $(DISPLAY_OBJS) $(COMMON_OBJS) $(LDFLAGS)

# Rule for compiling C source files into object files
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for compiling C++ source files into object files
$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target for removing compiled files
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET_CLIENT) $(TARGET_DISPLAY)
