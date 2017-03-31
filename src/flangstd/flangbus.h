#ifndef FLANG_BUS_H
#define FLANG_BUS_H

#include<stdlib.h>
#define FLANG_BUS_INITIAL_SIZE 64 //I like the number 64, but it's arbitrary.

//Conceptually, a Flang bus should operate like this:
//It should be shaped like a clock, with the only hand pointing up.
//The thing under the hand can be Scanned, returning the value under the hand.
//The thing under the hand can be Popped, Scanning then removing the value. The hand will go to the thing to the left of it.
//Something can be Pushed to the right of the hand. The hand will move to the new item.
//The hand can be rotated either direction any distance.
struct Flang_bus {
	//A bus is a circular buffer which will will push from the pointer, and allocate to the right.
	//Memory works like this:
	//smmmmmmcmmmu              bmmmme
	//s is *start, m is memory in the buffer, c is *current, u is *used, b is *beg, and e is *end.
	//I will call |s----|u the first memory section and |b-----|e the second.
	//There is no allocated memory at or after *end or before *start. They ususally stay.
	//*current moves to wherever it needs to go.
	//*used always marks the end of the first section of memory. *beg does the beginning of the second section.
	double *start, *current, *used, *beg, *end;
};

//Make a new bus of length 64
struct Flang_bus *Flang_bus_New();
//give the value at the current position
#define Flang_bus_Scan(f) (*f->current)
//give the value at the current position, then remove it
double Flang_bus_Pop(struct Flang_bus *f);
//put in a new value to the right of the current, then push up the current.
void Flang_bus_Push(struct Flang_bus *f, double v);
double Flang_bus_Length(struct Flang_bus *f);
//move current up
void Flang_bus_RotateLeft(struct Flang_bus *f, double steps);
//move current down
void Flang_bus_RotateRight(struct Flang_bus *f, double steps);

#endif
