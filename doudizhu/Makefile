CC = gcc
C++ = g++
LINK = g++

CCFLAGS = -c -g -fPIC -O3 -Wall
C++FLAGS = -c -g -fPIC -O3 -Wall -std=gnu++14

#期望得到的执行文件或动态库.so
TARGET=ddz_endgame.so
 
INCLUDES = -I. -I/usr/include/lua5.3
 
C++FILES = $(wildcard *.cc)
CFILES = $(wildcard *.c)
 
OBJFILE = $(CFILES:.c=.o) $(C++FILES:.cpp=.o)
 
all:$(TARGET)
 
$(TARGET): $(OBJFILE)
	$(LINK) $^ -Wall -fPIC -O3 -shared -o $@
 
%.o:%.c
	$(CC) -o $@ $(CCFLAGS) $< $(INCLUDES)
 
%.o:%.cpp
	$(C++) -o $@ $(C++FLAGS) $< $(INCLUDES)
 
install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJFILE)
