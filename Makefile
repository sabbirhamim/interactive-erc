# Makefile
PROJECT   := ierc
CC        := cc
LEX       := flex
YACC      := bison
CFLAGS    := -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200809L -Wno-unused-function
LDFLAGS   :=
INC       := -Isrc
BUILD     := build
SRC       := src

OBJS := $(BUILD)/lexer.o $(BUILD)/parser.o \
        $(BUILD)/main.o $(BUILD)/error.o \
        $(BUILD)/suggest.o $(BUILD)/interactive.o \
        $(BUILD)/symbol_table.o $(BUILD)/semantic.o

all: $(BUILD)/$(PROJECT)

$(BUILD)/$(PROJECT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# --- Bison MUST run first: produces parser.c and parser.h ---
$(BUILD)/parser.c $(BUILD)/parser.h: $(SRC)/parser.y
	mkdir -p $(BUILD)
	$(YACC) -Wall -Wcounterexamples -d -v -o $(BUILD)/parser.c $<

$(BUILD)/parser.o: $(BUILD)/parser.c
	$(CC) $(CFLAGS) $(INC) -c $(BUILD)/parser.c -o $@

# --- Flex AFTER parser.h exists ---
$(BUILD)/lexer.o: $(SRC)/lexer.l $(BUILD)/parser.h
	mkdir -p $(BUILD)
	$(LEX) -o $(BUILD)/lexer.c $<
	$(CC) $(CFLAGS) $(INC) -c $(BUILD)/lexer.c -o $@

# --- Generic C objects ---
$(BUILD)/%.o: $(SRC)/%.c $(SRC)/%.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(BUILD)/main.o: $(SRC)/main.c $(SRC)/util.h
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

run: all
	$(BUILD)/$(PROJECT) --interactive examples/ok.lang

clean:
	rm -rf $(BUILD) *.out
.PHONY: all run clean
