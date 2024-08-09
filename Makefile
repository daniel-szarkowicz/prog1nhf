## USER VARIABLES

TARGET_EXEC := aknasch

CC := gcc
#CC := clang
# CFLAGS := -O3 -std=c17 -Werror=all -Werror=pedantic -Wextra
CFLAGS := -O3 -std=c17 -Werror=pedantic -Wextra
LFLAGS := -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_image

BUILD_DIR := ./build
SRC_DIR := ./src

## AUTOMATIC VARIABLES

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_DIRS := $(SRC_DIR)
INC_FLAGS := $(INC_DIRS:%=-I%)

CPPFLAGS := $(INC_FLAGS) -MMD -MP

## RULES
build: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(LFLAGS) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

#.SILENT:
.NOTPARALLEL: clean
.PHONY: clean run build zip

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/$(TARGET_EXEC)
	$<

debug:
	mkdir -p $(BUILD_DIR)
	$(CC) -g $(SRCS) $(LFLAGS) $(CFLAGS) -DDEBUG $(INC_FLAGS) -o $(BUILD_DIR)/$(TARGET_EXEC)_debug
	$(BUILD_DIR)/$(TARGET_EXEC)_debug

zip:
	doxygen Doxyfile
	$(MAKE) -C doxygen/latex
	mkdir -p tmp_zip/aknasch
	cp doxygen/latex/refman.pdf tmp_zip/aknasch/fejlesztoi_dokumentacio.pdf
	cp felhaszaloi_dokumentacio.pdf tmp_zip/aknasch/felhaszaloi_dokumentacio.pdf
	cp -r src/ tmp_zip/aknasch/src/
	cp -r res/ tmp_zip/aknasch/res/
	#cp specifikacio.pdf tmp_zip/aknasch/specifikacio.pdf
	cp Makefile tmp_zip/aknasch/Makefile
	cp Doxyfile tmp_zip/aknasch/Doxyfile
	(cd tmp_zip/; zip aknasch.zip -r aknasch/)
	cp tmp_zip/aknasch.zip aknasch.zip
	rm -rf tmp_zip

-include $(DEPS)
