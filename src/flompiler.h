#ifndef FLOMPILER_H
#define FLOMPILER_H

#define MAXVALS 8 //number of inputs/outputs per function max
#define WORDLEN 100 //largest word size
#define LINELEN 5000 //largest line size
#define MAXLINES 5000 //largest line size
#define MAXSCOPES 100 //largest line size

struct func {
	char ins[MAXVALS][WORDLEN], outs[MAXVALS][WORDLEN], name[WORDLEN]; //inputs and outputs, name of function
	//terminators are if (ins[i][0]) (outs[i][0]) (name[0]) then end
	char satisfied; //mask for if the inputs are satisfied
};

struct scope {
	char name[WORDLEN];
	struct func f[MAXLINES];
};

//will divide the chars of s into a new group every char in sep, and put the result in result.
char **split(char *s, char sep);

//will transfer code to a structure
void read(struct scope *scopes, char *escaped, char *s);

//update every func.satisfied flag for a value
void satisfy(char *program, struct func *funcs, char *value);

//will run a function funcs[i] and put code into p
void runfunc(char *program, struct func *funcs, int i);

#endif
