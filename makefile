CC = gcc
CFLAGS = -g -O2 -Wall
OUT_PATH = ./out
CLIBS = -lpcre2-8

GTK_VERSION = gtk4
GTK_CFLAGS = `pkg-config --cflags $(GTK_VERSION)`
GTK_LIBS = `pkg-config --libs $(GTK_VERSION)`

GUI_SRC_PATH = ./GUI
GUI_SRC = $(wildcard $(GUI_SRC_PATH)/*.c) 
GUI_TARGET_ = $(OUT_PATH)/gui.out

CLI_SRC_PATH = ./src
CLI_SRC = $(wildcard $(CLI_SRC_PATH)/*.c)
CLI_TARGET_ = $(OUT_PATH)/cli.out

ifeq ($(OS),Windows_NT)
	GUI_TARGET = $(GUI_TARGET_:%.out=%.exe)
	CLI_TARGET = $(CLI_TARGET_:%.out=%.exe)
else
	GUI_TARGET = $(GUI_TARGET_)	
	CLI_TARGET = $(CLI_TARGET_)
endif

all:gui cli

dir:
	@mkdir -p $(OUT_PATH)

gui:dir $(GUI_SRC)
	@echo GTK_CFLAGS:
	@echo $(GTK_CFLAGS)
	@echo GTK_LIBS:
	@echo $(GTK_LIBS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(GUI_SRC) -o $(GUI_TARGET) $(GTK_LIBS) $(CLIBS)
ifeq ($(OS),Windows_NT)
	-@cp ./bin/GUI/* ./out/
endif

cli:dir $(CLI_SRC)
	$(CC) $(CFLAGS) $(CLI_SRC) -o $(CLI_TARGET) $(CLIBS)
ifeq ($(OS),Windows_NT)
	-@cp ./bin/src/* ./out/
endif

.PHONY: clean all
clean:
	-@rm -f $(GUI_TARGET)
	-@rm -f $(CLI_TARGET)
	-@rm -rf $(OUT_PATH)
	@echo Clean done