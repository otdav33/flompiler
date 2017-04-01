#include"flangbus.h"

//Make a new bus of length 64
struct Flang_bus *Flang_bus_New() {
	struct Flang_bus *f = malloc(sizeof(struct Flang_bus));
	f->start = malloc(sizeof(double) * FLANG_BUS_INITIAL_SIZE); //make the buffer
	f->beg = f->end = (f->start + FLANG_BUS_INITIAL_SIZE);
	f->used = f->current = f->start; //start at the beginning
	return f;
}

//Double bus length. This will move *end and *beg with the last section of memory so you can fit more data.
void Flang_bus_extend(struct Flang_bus *f) {
	size_t halflength = f->end - f->start;
	size_t length = halflength * 2;
	f->start = realloc(f->start, length);
	double *i = f->end, *j = (f->end += halflength); //iterators starting at the old and new ends
	while (--i >= f->beg)
		*(--j) = *i; //move all the data.
	f->beg += halflength;
}

//will rearrange the data so that the data structure is the same, but f.current is before f.used
void Flang_bus_center(struct Flang_bus *f) {
	while (f->used-1 > f->current) //if c is in the first memory section.
		*(--f->beg) = *(--f->used); //copy all after f.current in the first section to beginning of second
	while (f->beg <= f->current) //if c is in the second memory section.
		*(f->used++) = *(f->beg++); //copy all before and including f.current in the second section to end of the first.
	f->current = f->used - 1; //realign the current marker, in case f.current started out in the second section.
}

//give the value at the current position, then remove it
double Flang_bus_Pop(struct Flang_bus *f) {
	Flang_bus_center(f); //put it in easiest-to-deal-with position
	f->used--; //delete-by-pointer
	return *(f->current--);
}

//put in a new value to the right of the current, then push up the current.
void Flang_bus_Push(struct Flang_bus *f, double v) {
	Flang_bus_center(f);
	if (f->used == f->beg)
		Flang_bus_extend(f); //make sure it doesn't overflow
	*(++f->current) = v;
	++f->used;
}

double Flang_bus_Length(struct Flang_bus *f) {
	return f->used - f->start + f->end - f->beg;
}

//actual rotate left without checking
void Flang_bus_rotateleft(struct Flang_bus *f, size_t len, size_t steps) { 
	//fancier version of f->current += steps with wrapping
	size_t invsteps = len - steps;
	if (steps >= f->used - f->current) { //if you would go past the first memory section
		if (invsteps < f->current - f->start)
			f->current -= invsteps;
		else {
			int dist = f->used - f->current;
			f->current = f->beg;
			Flang_bus_rotateleft(f, len, steps - dist);
		}
	} else if (steps >= f->end - f->current) { //if you would go past the second memory section
		if (invsteps < f->current - f->beg)
			f->current -= invsteps;
		else {
			double dist = f->end - f->current;
			f->current = f->start;
			Flang_bus_rotateleft(f, len, steps - dist);
		}
	} else
		f->current += steps;
}

//move current up
void Flang_bus_RotateLeft(struct Flang_bus *f, double steps) { 
	size_t len = Flang_bus_Length(f);
	if (!len) //bus length != 0
		return;
	//You only need to go around once.
	Flang_bus_rotateleft(f, len, (size_t) steps % len);
}

//move current down
void Flang_bus_RotateRight(struct Flang_bus *f, double steps) { 
	size_t len = Flang_bus_Length(f);
	if (!len) //bus length != 0
		return;
	Flang_bus_rotateleft(f, len, len - ((size_t) steps % len));
}
