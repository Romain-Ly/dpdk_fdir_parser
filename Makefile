ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc
RTE_OUTPUT = bin
RTE_TOOLCHAIN = gcc

include $(RTE_SDK)/mk/rte.vars.mk

# binary name
APP = test

LDLIBS += -lyaml


# all source are stor
SRCS-y := src/fdir_yaml_parser.c

CFLAGS_src/fdir_yaml_parser.o += -DUNIT_MAIN

#CFLAGS += -v
CFLAGS += -I$(RTE_SRCDIR)/include
CFLAGS += -g  -O0 -Wall
CFLAGS += -std=gnu11 
CFLAGS += $(CFLAGS-$@)
CFLAGS-main.o += -Iinclude
EXTRA_CFLAGS := -lyaml


include $(RTE_SDK)/mk/rte.extapp.mk

