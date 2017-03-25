#ifndef FLANG_BUS_H
#define FLANG_BUS_H

#include<stdlib.h>
#define FLANG_BUS_INITIAL_SIZE 64 //I like the number 64, but it's arbitrary.

struct Flang_bus {
	//A bus is a circular buffer which will will push from the pointer, and allocate to the right.
	//Memory works like this:
	//smmmmmmcmmmu              bmmmme
	//s is *start, m is memory in the buffer, c is *current, u is *used, b is *beg, and e is *end.
	//There is no allocated memory at or after *end or before *start. They ususally stay.
	//*current moves to wherever it needs to go.
	//*used always marks where the first section of memory ends. *beg does the beginning of the second section.
	double *start, *current, *used, *beg, *end;
}

//Make a new bus of length 64
struct Flang_bus Flang_bus_New() {
	struct Flang_bus f;
	size_t length = sizeof(double) * FLANG_BUS_INITIAL_SIZE; //desired buffer length
	f.start = malloc(length); //make the buffer
	f.end = f.start + length;
	f.current = f.used = f.start; //start at the beginning
	return f;
}

//Double bus length. This will move *end and *beg with the last section of memory so you can fit more data.
Flang_bus_Extend(struct Flang_bus f) {
	size_t halflength = f.end - f.start;
	size_t length = halflength * 2;
	realloc(f, length);
	double *i = f.end, *j = (f.end += halflength); //iterators starting at the old and new ends
	while (--i >= f.beg)
		*(--j) = *i; //move all the data.
	f.beg += halflength;
}

double Flang_bus_Pop(struct Flang_bus f) {
	double v = *f.current; //return value
	for (
