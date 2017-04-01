#ifndef FLOMPILER_H
#define FLOMPILER_H

#define MAXVALS 8 //number of inputs/outputs per function max
#define WORDLEN 100 //largest word size
#define LINELEN 5000 //largest line size
#define MAXLINES 5000 //largest line size

struct func {
	char ins[MAXVALS][WORDLEN], outs[MAXVALS][WORDLEN], name[WORDLEN]; //inputs and outputs, name of function
	char satisfied; //mask for if the inputs are satisfied
};

struct value {
	char name[WORDLEN], type[WORDLEN], declared;
};

//will divide the chars of s into a new group every char in sep, and put the result in result.
void split(char **result, char *s, char *sep);

//will transfer code to a structure
struct func *read(char *s);

//This should be run when a value is satisfied.
//p is the C program text, funcs is all functions, and value is the satisfied value.
void satisfy(char *program, struct value *values, struct func *funcs, char *value);

int indexofvalue(struct value *values, char *value);

//converts a value to C for inputs
char *convvar(char *varname);

//will run a function funcs[i] and put code into p
void runfunc(char *program, struct value *values, struct func *funcs, int i);

//make a program and put it in program
void makemain(char *program, struct value *values, struct func *funcs);

#endif
