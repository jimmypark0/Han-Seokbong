CURDIR  = ./
SRCDIR  = ./
OBJDIR  = ./

SUFFIXES = .o .c .h
.SUFFIXES: $(SUFFIXES)

SHELL = /bin/sh
DISTRO = clanton-tiny
TARGET_NAME = i586-poky-linux-uclibc
NATIVE_NAME = x86_64-pokysdk-linux
GCC_VERSION = 4.7.2

TARGET_BIN    = hello
TARGET_ARCH   = -m32 -march=i586 --sysroot=$(SYSROOT_TARGET)
INCLUDE_DIRS  = -I $(SYSROOT_TARGET)/usr/include \
				-I $(SYSROOT_TARGET)/usr/include/c++ \
				-I $(SYSROOT_NATIVE)/usr/lib/$(TARGET_NAME)/gcc/$(TARGET_NAME)/$(GCC_VERSION)/include
LIBRARY_DIRS  = -L $(SYSROOT_TARGET)/lib \
				-L $(SYSROOT_TARGET)/usr/lib \
				-L $(SYSROOT_TARGET)/usr/lib/$(TARGET_NAME)/$(GCC_VERSION)
COMPILE_OPTS  = -O2 -pipe -g -feliminate-unused-debug-types -fpermissive -Wall $(INCLUDE_DIRS) 

SYSROOT_TARGET = /opt/$(DISTRO)/1.4.2/sysroots/$(TARGET_NAME)/
SYSROOT_NATIVE = /opt/$(DISTRO)/1.4.2/sysroots/$(NATIVE_NAME)/

TOOLDIR = $(SYSROOT_NATIVE)/usr/bin/$(TARGET_NAME)

CC      = $(TOOLDIR)/i586-poky-linux-uclibc-g++
CXX     = $(CC)
AS      = $(CC)
LD      = $(CC)
AR      = $(TOOLDIR)/i586-poky-linux-uclibc-ar
CXXFLAGS= $(COMPILE_OPTS)
CFLAGS = -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed $(LIBRARY_DIRS) -lstdc++

TARGET_ARCH   = -m32 -march=i586 --sysroot=$(SYSROOT_TARGET)
LDFLAGS = -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed $(LIBRARY_DIRS) -lstdc++ $(TARGET_ARCH)
CC      = $(TOOLDIR)/i586-poky-linux-uclibc-g++ $(LDFLAGS)

PROJ = index

CFILES = $(SRCDIR)indexS.c
OBJS = $(OBJDIR)indexS.o

all: index

tag: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)
index: $(OBJS) ../libindex.a
	$(CC) -o ./index $(OBJS) -L.. ../libindex.a

$(OBJDIR)indexS.o: $(SRCDIR)indexS.c
	$(CC) $(CFLAGS) -c $(SRCDIR)indexS.c -o $(OBJDIR)indexS.o
