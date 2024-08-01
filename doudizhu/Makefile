CC = gcc
C++ = g++
LINK = g++

CCFLAGS = -c -g -fPIC -O3 -Wall
C++FLAGS = -c -g -fPIC -O3 -Wall -std=gnu++14

#期望得到的执行文件或动态库.so
TARGET=ddz_endgame.so
 
INCLUDES = -I. -I/usr/include/lua5.3
 
CCFILES = $(wildcard *.cc)
CFILES = $(wildcard *.c)
OBJFILE = $(CFILES:.c=.o) $(CCFILES:.cc=.o)

all:$(TARGET)
 
$(TARGET): $(OBJFILE)
	$(LINK) $^ -Wall -fPIC -lpthread -O3 -shared -o $@
 
%.o:%.c
	$(CC) -o $@ $(CCFLAGS) $< $(INCLUDES)
 
%.o:%.cc
	$(C++) -o $@ $(C++FLAGS) $< $(INCLUDES)
 
install:
	tsxs -i -o $(TARGET)

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJFILE)
