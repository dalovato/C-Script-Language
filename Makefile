# This is the makefile for Project 6.
# @author David Lovato, dalovato
CC = gcc
CFLAGS = -g -Wall -std=c99 -D_POSIX_C_SOURCE=200112L
nonde: command.o label.o parse.o
command.o: label.o parse.o
clean:
				rm -f nonde nonde.o
				rm -f command command.o
				rm -f parse parse.o
				rm -f label label.o
				rm -f output.txt
				rm -f stderr.txt
