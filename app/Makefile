ifeq ($(OS),Windows_NT)
RM = del /Q /F
CP = copy /Y
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
RM = rm -rf
CP = cp -f
endif

all: led_control_app.dll

led_control_app.dll: led_control_app.c led_control_app.h
	gcc -std=c11 -Wall -Wextra -pedantic -c -fPIC led_control_app.c -o led_control_app.o -lsetupapi
	gcc -shared led_control_app.o -o led_control_app.dll -lsetupapi

.PHONY: clean
clean:
	-$(RM) *.o *.dll *.h.gch
