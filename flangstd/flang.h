#ifndef FLANG_STD
#define FLANG_STD

#include<stdio.h>

//2i1o: gives left input
#define Left(a, b)
//1i1o: gives printed inputted char
#define Putchar(a) putchar(a)
//1i1o: gives inputted char
#define Getchar(a) getchar()
//3i2o: gives r on left output if (a == b), otherwise on right output
#define Equal(a, r, b, equal, different) (a == b ? equal : different)(r)
//3i2o: gives r on left output if (a > b), otherwise on right output
#define Greater(a, r, b, greater, less) (a == b ? greater : less)(r)
//2i1o for the next couple simple math operators
#define Sum(a, b, fun) fun(a + b)
#define Difference(a, b, fun) fun(a - b)
#define Product(a, b, fun) fun(a * b)
#define Quotient(a, b, fun) fun(a / b)

#endif
