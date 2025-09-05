# Copyright 2025 Terry Golubiewski, all rights reserved.

export PROJNAME:=tjg32
export PROJDIR := $(abspath .)
# SWDEV := $(PROJDIR)/SwDev
include $(SWDEV)/project.mk

# Must use "=" instead of ":=" because $E will be defined below.
MAIN_E=main.$E
CRCTIME_E=CrcTime.$E

TARGET1=$(MAIN_E)
TARGETS=$(TARGET1)

SRC1:=main.cpp

SOURCE:=$(SRC1)

SYSINCL:=
INCLUDE:=$(PROJDIR)

# Must use "=" because LIBS will be changed below.
LIBS=

#DEBUG=1

include $(SWDEV)/$(COMPILER).mk
include $(SWDEV)/build.mk

.ONESHELL:

.PHONY: all clean scour test

all: depend $(TARGETS)

$(TARGET1): $(OBJ1) $(LIBS)
        $(LINK)
