#ifndef FLANG_STD
#define FLANG_STD

#include<stdio.h>

//2i1o: gives left input
#define Left(a, b)
//1i1o: gives printed inputted char
#define Putchar(a) putchar(a)
//1i1o: gives inputted char
#define Getchar(a) getchar()
//2i1o for the next couple simple math operators
#define Sum(a, b) (a + b)
#define Difference(a, b) (a - b)
#define Product(a, b) (a * b)
#define Quotient(a, b) (a / b)

#endif
