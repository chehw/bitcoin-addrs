BIN_DIR=bin
LIB_DIR=lib
VERSION_MAJOR=0
VERSION_MINOR=1

TARGETS=$(BIN_DIR)/pubkey_to_addrs 
# TARGETS += $(LIB_DIR)/libbitcoin_addrs.so

DEBUG ?= 1
OPTIMIZE ?= -O2

CC=gcc -std=gnu99 -Wall -D_DEFAULT_SOURCE -D_GNU_SOURCE
LINKER=$(CC)
AR=ar crf

CFLAGS = -Iinclude -Ibase -Iutils
LIBS = -lm -lpthread -lgnutls -lgmp

CFLAGS += $(shell pkg-config --cflags libsecp256k1)
LIBS += $(shell pkg-config --libs libsecp256k1)

DEPS=

## debug mode
ifeq ($(DEBUG),1)
CFLAGS += -g -D_DEBUG
OPTIMIZE = -O0
endif
LDFLAGS = $(CFLAGS)

SRC_DIR=src
OBJ_DIR=obj
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

OBJECTS_SHARED := $(OBJECTS:%.o=%.o.shared)
OBJECTS_SHARED := $(filter-out main.o.shared,$(OBJECTS_SHARED))

BASE_SRC_DIR=base
BASE_OBJ_DIR=obj/base
BASE_SOURCES := $(wildcard $(BASE_SRC_DIR)/*.c)
BASE_OBJECTS := $(BASE_SOURCES:$(BASE_SRC_DIR)/%.c=$(BASE_OBJ_DIR)/%.o)
BASE_OBJECTS_SHARED := $(BASE_SOURCES:$(BASE_SRC_DIR)/%.c=$(BASE_OBJ_DIR)/%.o.shared)

UTILS_SRC_DIR=utils
UTILS_OBJ_DIR=obj/utils
UTILS_SOURCES := $(wildcard $(UTILS_SRC_DIR)/*.c)
UTILS_OBJECTS := $(UTILS_SOURCES:$(UTILS_SRC_DIR)/%.c=$(UTILS_OBJ_DIR)/%.o)
UTILS_OBJECTS_SHARED := $(UTILS_SOURCES:$(UTILS_SRC_DIR)/%.c=$(UTILS_OBJ_DIR)/%.o.shared)


all: do_init $(TARGETS)

## exe file
$(BIN_DIR)/pubkey_to_addrs: $(SOURCES) $(BASE_OBJECTS) $(UTILS_OBJECTS)
	$(LINKER) -o $@ $^ $(LDFLAGS) $(LIBS)

## symbolic link
$(LIB_DIR)/libbitcoin_addrs.so: $(LIB_DIR)/libbitcoin_addrs.so.$(VERSION_MAJOR).$(VERSION_MINOR)
	test -e $@ && rm $@
	ln -s $< $@

## shared library
$(LIB_DIR)/libbitcoin_addrs.so.$(VERSION_MAJOR).$(VERSION_MINOR): $(BASE_OBJECTS_SHARED) $(OBJECTS_SHARED) $(UTILS_OBJECTS_SHARED)
	$(LINKER) -fPIC -shared -o $@ $^ $(LIBS)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
	
$(BASE_OBJECTS): $(BASE_OBJ_DIR)/%.o : $(BASE_SRC_DIR)/%.c $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
	
$(UTILS_OBJECTS): $(UTILS_OBJ_DIR)/%.o : $(UTILS_SRC_DIR)/%.c $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)


## shared objects
$(OBJECTS_SHARED): $(OBJ_DIR)/%.o.shared : $(SRC_DIR)/%.c $(DEPS)
	$(CC) -fPIC -o $@ -c $< $(CFLAGS)
	
$(BASE_OBJECTS_SHARED): $(BASE_OBJ_DIR)/%.o.shared : $(BASE_SRC_DIR)/%.c $(DEPS)
	$(CC) -fPIC -o $@ -c $< $(CFLAGS)
	
$(UTILS_OBJECTS_SHARED): $(UTILS_OBJ_DIR)/%.o.shared : $(UTILS_SRC_DIR)/%.c $(DEPS)
	$(CC) -fPIC -o $@ -c $< $(CFLAGS)

.PHONY: do_init clean
do_init:
	mkdir -p bin lib obj obj/base obj/utils
	
clean:
	rm -f $(TARGETS) obj/*.o obj/*.shared obj/base/*.o obj/base/*.shared obj/utils/*.o obj/utils/*.shared
	
	
