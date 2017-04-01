#ifndef FLOMPILER_H
#define FLOMPILER_H

#define MAXVALS 8 //number of inputs/outputs per function max
#define WORDLEN 100 //largest word size
#define LINELEN 5000 //largest line size
#define MAXLINES 5000 //largest line size

struct func {
	char ins[MAXVALS][WORDLEN], outs[MAXVALS][WORDLEN], name[WORDLEN]; //inputs and outputs, name of function
	//terminators are if (ins[i][0]) (outs[i][0]) (name[0]) then end
	char satisfied; //mask for if the inputs are satisfied
};

struct value {
	char name[WORDLEN], type[WORDLEN], declared, constant;
};

//will divide the chars of s into a new group every char in sep, and put the result in result.
void split(char **result, char *s, char *sep);

//will transfer code to a structure
struct func *read(char *s);

int indexofvalue(struct value *values, char *value);

//return all values from *f
struct value *makevals(struct func *f);

//update every func.satisfied flag for a value
void satisfy(struct func *funcs, char *value);

//converts a value to C for inputs
char *convvar(char *varname);

//will run a function funcs[i] and put code into p
void runfunc(char *program, struct value *values, struct func *funcs, int i);

//make a program and put it in program
void makemain(char *program, struct value *values, struct func *funcs);

#endif
