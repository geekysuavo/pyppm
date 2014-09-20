# Makefile: all compilation and programming parameters for PPM firmware

# define the project base filename and version.
PROJ=ppm
VER=2
REV=1

# list object files to be compiled into the project.
#FIXME OBJECTS=tcnt.o gpio.o spi.o ppm.o main.o
OBJECTS=gpio.o spi.o ppm.o main.o
COREOBJ=clk.o usb.o

# include all software-global make rules.
include ../global.mk

# change the target microcontroller.
MCU=atmega32u4

