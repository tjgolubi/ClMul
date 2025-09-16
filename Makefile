# Copyright 2025 Terry Golubiewski, all rights reserved.

export PROJNAME:=ClMul
export PROJDIR := $(abspath .)
# SWDEV := $(PROJDIR)/SwDev
include $(SWDEV)/project.mk

APP:=$(abspath $(HOME)/App)
BOOST:=$(APP)/boost
MP11:=$(APP)/mp11

# Must use "=" instead of ":=" because $E will be defined below.
MAIN_E=main.$E
CRCTIME_E=CrcTime.$E

TARGET1=$(MAIN_E)
TARGETS=$(TARGET1)

SRC1:=main.cpp

ifeq ($(COMPILER), gcc)
CDEFS+=-DUSE_PCLMUL_CRC32=1
SRC2+=cksum_pclmul0.cpp
endif
ifeq ($(COMPILER), clang)
CDEFS+=-DUSE_VMULL_CRC32=1
SRC2+=cksum_vmull0.cpp
endif

SOURCE:=$(SRC1)


#SYSINCL:=$(addsuffix /include, $(UNITS)/core $(UNITS)/systems $(GSL))
SYSINCL:=$(BOOST) $(addsuffix /include, $(MP11))
INCLUDE:=$(PROJDIR)

# Must use "=" because LIBS will be changed below.
LIBS=

#DEBUG=1

include $(SWDEV)/$(COMPILER).mk
include $(SWDEV)/build.mk

#CLEAN+=cksum_core.txt cksum_tjg.txt
#SCOUR+=bigfile.bin cksum.txt tjg.txt tjg.bin tjg256.bin

.ONESHELL:

.PHONY: all clean scour test

all: depend $(TARGETS)

$(TARGET1): $(OBJ1) $(LIBS)
        $(LINK)
