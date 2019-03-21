CURDIR  = ./

HEADER  = ./header/
HDICDIR = ./hdic2/

MAINDIR = ./main/
POSTDIR = ./post/
DICDIR  = ./dicacc/
CODEDIR = ./ksc5601/
AMBDIR  = ./ambig/
NUMDIR  = ./num/
NDXDIR  = ./keyword/
ASPDIR  = ./asp/

OBJDIR  = ../obj/

SUFFIXES = .o .cpp .h
.SUFFIXES: $(SUFFIXES)

PROJ = ham

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
CC      = $(TOOLDIR)/i586-poky-linux-uclibc-g++
LDFLAGS = -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed $(LIBRARY_DIRS) -lstdc++ $(TARGET_ARCH)

CFILES = $(MAINDIR)pre-ham.cpp $(CODEDIR)ksc5601.c $(CODEDIR)unicode.c \
	$(MAINDIR)hamsent.cpp $(MAINDIR)hamword.c $(MAINDIR)tail.c $(MAINDIR)ascii.c \
	$(MAINDIR)josa.cpp $(MAINDIR)eomi.c $(MAINDIR)pomi.c $(MAINDIR)irreg.c \
	$(MAINDIR)eomi-ao.cpp $(MAINDIR)eomi-agr.c $(MAINDIR)eomi-sav.c \
	$(MAINDIR)acc-josa.cpp $(MAINDIR)acc-eomi.c $(MAINDIR)open-ham.c \
	$(MAINDIR)hfwords.cpp \
	$(DICDIR)dic.cpp $(DICDIR)isnoun.c $(DICDIR)n-eomi.c $(DICDIR)issingle.c \
	$(DICDIR)isverb.cpp $(DICDIR)i-drop.c $(DICDIR)v-sfx.c \
	$(DICDIR)xverb.cpp $(DICDIR)xverb-ao.c $(DICDIR)xverb2.c \
	$(DICDIR)access.cpp $(DICDIR)dic-rma.c \
	$(DICDIR)dic-usr.cpp $(DICDIR)dic-cnn.c \
	$(POSTDIR)postham.cpp $(POSTDIR)cmpd.c $(POSTDIR)prefix.c $(POSTDIR)abbr.c \
	$(POSTDIR)score.cpp $(POSTDIR)score-f.c $(POSTDIR)score2.c \
	$(POSTDIR)cnoun-a.cpp $(POSTDIR)pername.c \
	$(POSTDIR)resconv.cpp $(POSTDIR)nv-xpnd.c \
	$(POSTDIR)josaeomi.cpp $(POSTDIR)eomi-var.c $(POSTDIR)cnoun.c $(POSTDIR)cnoun2.c \
	$(AMBDIR)preparse.cpp $(AMBDIR)prepars0.c $(AMBDIR)joemlist.c $(AMBDIR)check.c \
	$(NUMDIR)numeral.cpp $(NUMDIR)numunit.c $(NUMDIR)isnumstr.c \
	$(NDXDIR)keyword.cpp $(NDXDIR)gen-keys.c $(NDXDIR)open-ndx.c \
	$(NDXDIR)garbage.cpp $(NDXDIR)add-cnn.c \
	$(NDXDIR)dic-term.cpp $(NDXDIR)get-term.c $(NDXDIR)t-weight.c $(NDXDIR)get-sent.c \
	$(ASPDIR)bigram.cpp $(ASPDIR)autospac.c $(ASPDIR)asp-data.c

OBJS = $(OBJDIR)pre-ham.o $(OBJDIR)ksc5601.o $(OBJDIR)unicode.o \
	$(OBJDIR)hamsent.o $(OBJDIR)hamword.o $(OBJDIR)tail.o $(OBJDIR)ascii.o \
	$(OBJDIR)josa.o $(OBJDIR)eomi.o $(OBJDIR)pomi.o $(OBJDIR)irreg.o \
	$(OBJDIR)eomi-ao.o $(OBJDIR)eomi-agr.o $(OBJDIR)eomi-sav.o \
	$(OBJDIR)acc-josa.o $(OBJDIR)acc-eomi.o $(OBJDIR)open-ham.o \
	$(OBJDIR)hfwords.o \
	$(OBJDIR)dic.o $(OBJDIR)isnoun.o $(OBJDIR)n-eomi.o $(OBJDIR)issingle.o \
	$(OBJDIR)isverb.o $(OBJDIR)i-drop.o $(OBJDIR)v-sfx.o \
	$(OBJDIR)xverb.o $(OBJDIR)xverb-ao.o $(OBJDIR)xverb2.o \
	$(OBJDIR)access.o $(OBJDIR)dic-rma.o \
	$(OBJDIR)dic-usr.o $(OBJDIR)dic-cnn.o \
	$(OBJDIR)postham.o $(OBJDIR)cmpd.o $(OBJDIR)prefix.o $(OBJDIR)abbr.o \
	$(OBJDIR)score.o $(OBJDIR)score-f.o $(OBJDIR)score2.o \
	$(OBJDIR)resconv.o $(OBJDIR)nv-xpnd.o \
	$(OBJDIR)cnoun-a.o $(OBJDIR)pername.o \
	$(OBJDIR)josaeomi.o $(OBJDIR)eomi-var.o $(OBJDIR)cnoun.o $(OBJDIR)cnoun2.o \
	$(OBJDIR)preparse.o $(OBJDIR)prepars0.o $(OBJDIR)joemlist.o $(OBJDIR)check.o \
	$(OBJDIR)numeral.o $(OBJDIR)numunit.o $(OBJDIR)isnumstr.o \
	$(OBJDIR)keyword.o $(OBJDIR)gen-keys.o $(OBJDIR)open-ndx.o \
	$(OBJDIR)garbage.o $(OBJDIR)add-cnn.o \
	$(OBJDIR)dic-term.o $(OBJDIR)get-term.o $(OBJDIR)t-weight.o $(OBJDIR)get-sent.o \
	$(OBJDIR)bigram.o $(OBJDIR)autospac.o $(OBJDIR)asp-data.o

all: ham

tag: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)
ham: $(OBJS)
	ar rcv ../libindex.a $(OBJS)

$(OBJDIR)ksc5601.o: $(CODEDIR)ksc5601.cpp
	$(CC) $(CFLAGS) -c $(CODEDIR)ksc5601.cpp -o $(OBJDIR)ksc5601.o
$(OBJDIR)unicode.o: $(CODEDIR)unicode.cpp
	$(CC) $(CFLAGS) -c $(CODEDIR)unicode.cpp -o $(OBJDIR)unicode.o

$(OBJDIR)pre-ham.o: $(MAINDIR)pre-ham.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)pre-ham.cpp -o $(OBJDIR)pre-ham.o
$(OBJDIR)hamsent.o: $(MAINDIR)hamsent.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)hamsent.cpp -o $(OBJDIR)hamsent.o
$(OBJDIR)hamword.o: $(MAINDIR)hamword.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)hamword.cpp -o $(OBJDIR)hamword.o
$(OBJDIR)tail.o: $(MAINDIR)tail.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)tail.cpp -o $(OBJDIR)tail.o
$(OBJDIR)ascii.o: $(MAINDIR)ascii.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)ascii.cpp -o $(OBJDIR)ascii.o

$(OBJDIR)josa.o: $(MAINDIR)josa.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)josa.cpp -o $(OBJDIR)josa.o
$(OBJDIR)eomi.o: $(MAINDIR)eomi.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)eomi.cpp -o $(OBJDIR)eomi.o
$(OBJDIR)eomi-ao.o: $(MAINDIR)eomi-ao.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)eomi-ao.cpp -o $(OBJDIR)eomi-ao.o
$(OBJDIR)eomi-agr.o: $(MAINDIR)eomi-agr.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)eomi-agr.cpp -o $(OBJDIR)eomi-agr.o
$(OBJDIR)eomi-sav.o: $(MAINDIR)eomi-sav.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)eomi-sav.cpp -o $(OBJDIR)eomi-sav.o
$(OBJDIR)pomi.o: $(MAINDIR)pomi.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)pomi.cpp -o $(OBJDIR)pomi.o
$(OBJDIR)irreg.o: $(MAINDIR)irreg.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)irreg.cpp -o $(OBJDIR)irreg.o

$(OBJDIR)acc-josa.o: $(MAINDIR)acc-josa.cpp $(MAINDIR)josa.h
	$(CC) $(CFLAGS) -c $(MAINDIR)acc-josa.cpp -o $(OBJDIR)acc-josa.o
$(OBJDIR)acc-eomi.o: $(MAINDIR)acc-eomi.cpp $(MAINDIR)eomi.h
	$(CC) $(CFLAGS) -c $(MAINDIR)acc-eomi.cpp -o $(OBJDIR)acc-eomi.o
$(OBJDIR)open-ham.o: $(MAINDIR)open-ham.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)open-ham.cpp -o $(OBJDIR)open-ham.o
$(OBJDIR)hfwords.o: $(MAINDIR)hfwords.cpp
	$(CC) $(CFLAGS) -c $(MAINDIR)hfwords.cpp -o $(OBJDIR)hfwords.o

$(OBJDIR)dic.o: $(DICDIR)dic.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)dic.cpp -o $(OBJDIR)dic.o
$(OBJDIR)isnoun.o: $(DICDIR)isnoun.cpp $(HDICDIR)sfx-n.h
	$(CC) $(CFLAGS) -c $(DICDIR)isnoun.cpp -o $(OBJDIR)isnoun.o
$(OBJDIR)n-eomi.o: $(DICDIR)n-eomi.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)n-eomi.cpp -o $(OBJDIR)n-eomi.o
$(OBJDIR)isverb.o: $(DICDIR)isverb.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)isverb.cpp -o $(OBJDIR)isverb.o
$(OBJDIR)issingle.o: $(DICDIR)issingle.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)issingle.cpp -o $(OBJDIR)issingle.o
$(OBJDIR)i-drop.o: $(DICDIR)i-drop.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)i-drop.cpp -o $(OBJDIR)i-drop.o
$(OBJDIR)v-sfx.o: $(DICDIR)v-sfx.cpp $(HDICDIR)sfx-v.h
	$(CC) $(CFLAGS) -c $(DICDIR)v-sfx.cpp -o $(OBJDIR)v-sfx.o
$(OBJDIR)xverb.o: $(DICDIR)xverb.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)xverb.cpp -o $(OBJDIR)xverb.o
$(OBJDIR)xverb-ao.o: $(DICDIR)xverb-ao.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)xverb-ao.cpp -o $(OBJDIR)xverb-ao.o
$(OBJDIR)xverb2.o: $(DICDIR)xverb2.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)xverb2.cpp -o $(OBJDIR)xverb2.o

$(OBJDIR)access.o: $(DICDIR)access.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)access.cpp -o $(OBJDIR)access.o
$(OBJDIR)dic-rma.o: $(DICDIR)dic-rma.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)dic-rma.cpp -o $(OBJDIR)dic-rma.o
$(OBJDIR)dic-usr.o: $(DICDIR)dic-usr.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)dic-usr.cpp -o $(OBJDIR)dic-usr.o
$(OBJDIR)dic-cnn.o: $(DICDIR)dic-cnn.cpp
	$(CC) $(CFLAGS) -c $(DICDIR)dic-cnn.cpp -o $(OBJDIR)dic-cnn.o

$(OBJDIR)postham.o: $(POSTDIR)postham.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)postham.cpp -o $(OBJDIR)postham.o
$(OBJDIR)cmpd.o: $(POSTDIR)cmpd.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)cmpd.cpp -o $(OBJDIR)cmpd.o
$(OBJDIR)prefix.o: $(POSTDIR)prefix.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)prefix.cpp -o $(OBJDIR)prefix.o
$(OBJDIR)abbr.o: $(POSTDIR)abbr.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)abbr.cpp -o $(OBJDIR)abbr.o

$(OBJDIR)score.o: $(POSTDIR)score.cpp $(HEADER)score.h
	$(CC) $(CFLAGS) -c $(POSTDIR)score.cpp -o $(OBJDIR)score.o
$(OBJDIR)score2.o: $(POSTDIR)score2.cpp $(HEADER)score.h
	$(CC) $(CFLAGS) -c $(POSTDIR)score2.cpp -o $(OBJDIR)score2.o
$(OBJDIR)score-f.o: $(POSTDIR)score-f.cpp $(HEADER)score.h
	$(CC) $(CFLAGS) -c $(POSTDIR)score-f.cpp -o $(OBJDIR)score-f.o
$(OBJDIR)resconv.o: $(POSTDIR)resconv.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)resconv.cpp -o $(OBJDIR)resconv.o
$(OBJDIR)nv-xpnd.o: $(POSTDIR)nv-xpnd.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)nv-xpnd.cpp -o $(OBJDIR)nv-xpnd.o
$(OBJDIR)josaeomi.o: $(POSTDIR)josaeomi.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)josaeomi.cpp -o $(OBJDIR)josaeomi.o
$(OBJDIR)eomi-var.o: $(POSTDIR)eomi-var.cpp
	$(CC) $(CFLAGS) -c $(POSTDIR)eomi-var.cpp -o $(OBJDIR)eomi-var.o
$(OBJDIR)cnoun.o: $(POSTDIR)cnoun.cpp $(HEADER)ham.h
	$(CC) $(CFLAGS) -c $(POSTDIR)cnoun.cpp -o $(OBJDIR)cnoun.o
$(OBJDIR)cnoun2.o: $(POSTDIR)cnoun2.cpp $(HEADER)ham.h
	$(CC) $(CFLAGS) -c $(POSTDIR)cnoun2.cpp -o $(OBJDIR)cnoun2.o
$(OBJDIR)cnoun-a.o: $(POSTDIR)cnoun-a.cpp $(HEADER)ham.h
	$(CC) $(CFLAGS) -c $(POSTDIR)cnoun-a.cpp -o $(OBJDIR)cnoun-a.o
$(OBJDIR)pername.o: $(POSTDIR)pername.cpp $(HEADER)ham.h
	$(CC) $(CFLAGS) -c $(POSTDIR)pername.cpp -o $(OBJDIR)pername.o

$(OBJDIR)preparse.o: $(AMBDIR)preparse.cpp
	$(CC) $(CFLAGS) -c $(AMBDIR)preparse.cpp -o $(OBJDIR)preparse.o
$(OBJDIR)prepars0.o: $(AMBDIR)prepars0.cpp
	$(CC) $(CFLAGS) -c $(AMBDIR)prepars0.cpp -o $(OBJDIR)prepars0.o
$(OBJDIR)joemlist.o: $(AMBDIR)joemlist.cpp $(AMBDIR)josalist.h $(AMBDIR)eomilist.h
	$(CC) $(CFLAGS) -c $(AMBDIR)joemlist.cpp -o $(OBJDIR)joemlist.o
$(OBJDIR)check.o: $(AMBDIR)check.cpp
	$(CC) $(CFLAGS) -c $(AMBDIR)check.cpp -o $(OBJDIR)check.o

$(OBJDIR)numeral.o: $(NUMDIR)numeral.cpp
	$(CC) $(CFLAGS) -c $(NUMDIR)numeral.cpp -o $(OBJDIR)numeral.o
$(OBJDIR)numunit.o: $(NUMDIR)numunit.cpp
	$(CC) $(CFLAGS) -c $(NUMDIR)numunit.cpp -o $(OBJDIR)numunit.o
$(OBJDIR)isnumstr.o: $(NUMDIR)isnumstr.cpp
	$(CC) $(CFLAGS) -c $(NUMDIR)isnumstr.cpp -o $(OBJDIR)isnumstr.o

$(OBJDIR)keyword.o: $(NDXDIR)keyword.cpp $(HEADER)keyword.h
	$(CC) $(CFLAGS) -c $(NDXDIR)keyword.cpp -o $(OBJDIR)keyword.o
$(OBJDIR)gen-keys.o: $(NDXDIR)gen-keys.cpp $(HEADER)keyword.h
	$(CC) $(CFLAGS) -c $(NDXDIR)gen-keys.cpp -o $(OBJDIR)gen-keys.o
$(OBJDIR)open-ndx.o: $(NDXDIR)open-ndx.cpp
	$(CC) $(CFLAGS) -c $(NDXDIR)open-ndx.cpp -o $(OBJDIR)open-ndx.o
$(OBJDIR)garbage.o: $(NDXDIR)garbage.cpp $(HEADER)ham.h
	$(CC) $(CFLAGS) -c $(NDXDIR)garbage.cpp -o $(OBJDIR)garbage.o
$(OBJDIR)add-cnn.o: $(NDXDIR)add-cnn.cpp
	$(CC) $(CFLAGS) -c $(NDXDIR)add-cnn.cpp -o $(OBJDIR)add-cnn.o
$(OBJDIR)dic-term.o: $(NDXDIR)dic-term.cpp
	$(CC) $(CFLAGS) -c $(NDXDIR)dic-term.cpp -o $(OBJDIR)dic-term.o
$(OBJDIR)get-term.o: $(NDXDIR)get-term.cpp
	$(CC) $(CFLAGS) -c $(NDXDIR)get-term.cpp -o $(OBJDIR)get-term.o
$(OBJDIR)t-weight.o: $(NDXDIR)t-weight.cpp
	$(CC) $(CFLAGS) -c $(NDXDIR)t-weight.cpp -o $(OBJDIR)t-weight.o
$(OBJDIR)get-sent.o: $(NDXDIR)get-sent.cpp
	$(CC) $(CFLAGS) -c $(NDXDIR)get-sent.cpp -o $(OBJDIR)get-sent.o

$(OBJDIR)bigram.o: $(ASPDIR)bigram.cpp
	$(CC) $(CFLAGS) -c $(ASPDIR)bigram.cpp -o $(OBJDIR)bigram.o
$(OBJDIR)autospac.o: $(ASPDIR)autospac.cpp
	$(CC) $(CFLAGS) -c $(ASPDIR)autospac.cpp -o $(OBJDIR)autospac.o
$(OBJDIR)asp-data.o: $(ASPDIR)asp-data.cpp
	$(CC) $(CFLAGS) -c $(ASPDIR)asp-data.cpp -o $(OBJDIR)asp-data.o
