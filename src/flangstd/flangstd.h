#ifndef FLSTD_H
#define FLSTD_H
#include<stdio.h>

#define Left(a, b) a
#define Sum(a, b) (a + b)
#define Difference(a, b) (a - b)
#define Product(a, b) (a * b)
#define Quotient(a, b) (a / b)

double Print(double v) {
	putchar((int) v);
	return v;
}
double Keyboard(double v) {
	return (double) getchar();
}

#endif
