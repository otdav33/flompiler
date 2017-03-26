#include<stdio.h>
#include"flangstd.h"

void printbus(struct Flang_bus *f) {
	printf("s: %i, c: %i, u: %i, b:%i, e:%i\n", f->start - f->start, f->current - f->start, f->used - f->start, f->beg - f->start, f->end - f->start);
}

int main() {
	struct Flang_bus *f = NewBus();
	Push(f, (float) '1');
	Push(f, (float) '2');
	Push(f, (float) '3');
	printf("test5'%c'\n", (char) Pop(f));
	printf("testb'%c'\n", (char) Scan(f));
	RotateRight(f, 1);
	printf("testm'%c'\n", (char) Scan(f));
	RotateLeft(f, 1);
	printf("teste'%c'\n", (char) Scan(f));
	printf("test7'%c'\n", (char) Pop(f));
	printf("test8'%c'\n", (char) Pop(f));
	Push(f, (float) 'e');
	printf("test9'%c'\n", (char) Pop(f));
}
