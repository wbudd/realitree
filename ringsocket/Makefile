# SPDX-License-Identifier: MIT
# Copyright © 2019 William Budd

# RingSocket apps don't need to bother with soname versioning, because they're
# only runtime-loaded by RingSocket through dlopen() in accordance with the .so
# filename specified in the RingSocket configuration file. (In the unlikely
# event of rs_app() ABI breakage, RingSocket will introduce a key/value pair in
# the app directives of its conf file to differentiate between ABI versions.)

NAME = realitree
NAME_PREFIX = rt_

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11
CFLAGS_DLL = -fpic
CFLAGS_OPTIM = -O3 -flto -fuse-linker-plugin
LFLAGS_DLL = -shared -Wl,-z,relro,-z,now
LFLAGS_OPTIM = -flto -fuse-linker-plugin -fuse-ld=gold

SRC_DIR = src
SRC_PATHS := $(wildcard $(SRC_DIR)/$(NAME_PREFIX)*.c)
SRC_NAMES = $(SRC_PATHS:$(SRC_DIR)/%=%)

OBJ_DIR = .objects
OBJ_NAMES = $(SRC_NAMES:.c=.o)
OBJ_PATHS = $(addprefix $(OBJ_DIR)/, $(OBJ_NAMES))

.PHONY: optimized
optimized: $(NAME)

$(NAME): $(OBJ_PATHS)
	$(CC) $(LFLAGS_DLL) $(LFLAGS_OPTIM) $(OBJ_PATHS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_DLL) $(CFLAGS_OPTIM) -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

.PHONY: clean
clean:
	rm -rf $(NAME) $(OBJ_DIR)
