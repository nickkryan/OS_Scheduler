# Makefile
# CS 2200 PRJ4

src=student.c os-sim.c process.c list.c
obj=student.o os-sim.o process.o list.o
inc=student.h os-sim.h process.h list.h
misc=Makefile
target=os-sim
cflags=-Wall -g -O0 -Werror -pedantic -std=c99
lflags=-lpthread

all: $(target)

$(target) : $(obj) $(misc)
	gcc $(cflags) -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $<

clean:
	rm -f $(obj) $(target)
