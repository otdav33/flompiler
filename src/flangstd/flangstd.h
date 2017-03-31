#ifndef FLSTD_H
#define FLSTD_H
#include<stdio.h>
#include"flangbus.h"

#define Left(a, b)       a
#define Sum(a, b)        (a + b)
#define Difference(a, b) (a - b)
#define Product(a, b)    (a * b)
#define Quotient(a, b)   (a / b)
#define Print(a)         ((double) putchar((char)a))
#define Keyboard(a)      ((double) getchar())
#define Equals(a, b)     if(a == b)
#define Greater(a, b)    if(a > b)

#define NewBus      Flang_bus_New
#define Scan        Flang_bus_Scan
#define Pop         Flang_bus_Pop
#define Push        Flang_bus_Push
#define Length      Flang_bus_Length
#define RotateRight Flang_bus_RotateRight
#define RotateLeft  Flang_bus_RotateLeft

#endif
