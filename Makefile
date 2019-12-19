
override PROJNAME := libbitmap

DESTDIR  ?= /
prefix   ?= /usr/local
BINDIR   ?= ${prefix}/bin
LIBDIR   ?= ${prefix}/lib
BUILDDIR ?= ./build

CC       := $(CROSS_COMPILE)gcc
LD       := $(CROSS_COMPILE)gcc -o
LD_TEST  := $(CROSS_COMPILE)g++ -o
AR       := $(CROSS_COMPILE)ar
CFLAGS   ?=
CXXFLAGS ?= -Werror
LDFLAGS  ?=
ARFLAGS  ?= rv
RM       := rm -f
MKDIR    := mkdir -p
RMDIR    := rmdir
ECHO     := echo "$(PROJNAME):"

TARGET_LIB_STATIC ?= $(PROJNAME).a
TARGET_LIB_SHARED ?= $(PROJNAME).so
TARGET_TEST       ?= $(PROJNAME)-test

override INTERNAL_CFLAGS     := -std=c11 -Wall
override INTERNAL_CXXFLAGS   := -std=gnu++11 -Wall
override INTERNAL_INCLUDEDIR := ./include

override SRCDIR       := ./src
override SRCDIR_TEST  := ./test
override BUILDDIR_OBJ := $(BUILDDIR)/obj
override BUILDDIR_BIN := $(BUILDDIR)/bin

override SRC := $(wildcard $(SRCDIR)/*.c)
override OBJ := $(SRC:$(SRCDIR)/%.c=$(BUILDDIR_OBJ)/%.o)
override SRC_TEST := $(wildcard $(SRCDIR_TEST)/*.cpp)
override OBJ_TEST := $(SRC_TEST:$(SRCDIR_TEST)/%.cpp=$(BUILDDIR_OBJ)/%.o)

override OUT_STATIC := $(BUILDDIR_BIN)/$(TARGET_LIB_STATIC)
override OUT_SHARED := $(BUILDDIR_BIN)/$(TARGET_LIB_SHARED)
override OUT_TEST   := $(BUILDDIR_BIN)/$(TARGET_TEST)

override INCLUDES      := -I$(INTERNAL_INCLUDEDIR) -I$(SRCDIR)
override INCLUDES_TEST := -I$(INTERNAL_INCLUDEDIR) -I$(SRCDIR_TEST)

.PHONY: \
all \
static \
shared \
test \
clean \
remove \
install \
install-test \
\
static-flags \
shared-flags

all:    static install
static: static-flags $(BUILDDIR_OBJ) $(BUILDDIR_BIN) $(OUT_STATIC)
shared: shared-flags $(BUILDDIR_OBJ) $(BUILDDIR_BIN) $(OUT_SHARED)
test:   static $(OUT_TEST)

static-flags:
	$(eval override INTERNAL_CFLAGS_OBJ := )
shared-flags:
	$(eval override INTERNAL_CFLAGS_OBJ := -fPIC)

$(OUT_STATIC): $(OBJ)
	$(AR) $(ARFLAGS)cs $@ $(OBJ)

$(OUT_SHARED): $(OBJ)
	$(LD) $@ $(OBJ) -shared $(LDFLAGS)

$(OUT_TEST): $(OBJ_TEST)
	$(LD_TEST) $@ $(OBJ_TEST) $(OBJ)

$(BUILDDIR_OBJ):
	@test -d $@ || $(MKDIR) $@

$(BUILDDIR_BIN):
	@test -d $@ || $(MKDIR) $@

$(OBJ): $(BUILDDIR_OBJ)/%.o : $(SRCDIR)/%.c
	$(CC) $(INTERNAL_CFLAGS) $(INTERNAL_CFLAGS_OBJ) $(INCLUDES) $(CFLAGS) -c $< -o $@
$(OBJ_TEST): $(BUILDDIR_OBJ)/%.o : $(SRCDIR_TEST)/%.cpp
	$(CXX) $(INTERNAL_CXXFLAGS) $(INCLUDES_TEST) $(CXXFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJ)
	-$(RM) $(OBJ_TEST)
	-$(RMDIR) $(BUILDDIR_OBJ)
	-$(RMDIR) $(BUILDDIR_BIN)
	-$(RMDIR) $(BUILDDIR)
remove:
	-$(RM) $(OUT_STATIC)
	-$(RM) $(OUT_SHARED)
	-$(RM) $(OUT_TEST)
	-$(RMDIR) $(BINDIR)

install:
	-install -D -m 0644 $(OUT_STATIC) $(DESTDIR)$(LIBDIR)/$(TARGET_LIB_STATIC)
	-install -D -m 0644 -s $(OUT_SHARED) $(DESTDIR)$(LIBDIR)/$(TARGET_LIB_SHARED)

install-test:
	-install -D -m 0755 $(OUT_TEST) $(DESTDIR)/$(BINDIR)
