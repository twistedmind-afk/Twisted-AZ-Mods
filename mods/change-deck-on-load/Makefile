# SPDX-License-Identifier: GPL-3.0-only
MOD_ID := change-deck-on-load
SRCS := observe.c mode.c queue.cpp select_load.cpp
AZ_SDK_INCLUDE ?= ../../sdk/include
OUT ?= out
CFLAGS = -O2 -Wall -Wextra -Werror -fPIC -fvisibility=hidden -fno-omit-frame-pointer -fexceptions -fasynchronous-unwind-tables -fno-optimize-sibling-calls -I$(AZ_SDK_INCLUDE)
OBJS = $(addprefix $(OUT)/,$(addsuffix .o,$(SRCS)))
.PHONY: all
all: $(OUT)/$(MOD_ID).so
$(OUT)/%.c.o: %.c $(wildcard *.h) $(AZ_SDK_INCLUDE)/azmod-mod.h
	@mkdir -p $(OUT)
	gcc $(CFLAGS) -c $< -o $@
$(OUT)/%.cpp.o: %.cpp $(wildcard *.h) $(AZ_SDK_INCLUDE)/azmod-mod.h
	@mkdir -p $(OUT)
	g++ $(CFLAGS) -fvisibility-inlines-hidden -std=c++14 -c $< -o $@
$(OUT)/$(MOD_ID).so: $(OBJS) exports.map
	g++ -shared -static-libgcc -Wl,--no-undefined -Wl,--hash-style=gnu -Wl,--version-script=exports.map $(OBJS) -o $@
