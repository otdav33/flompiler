#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"flompiler.h"

//will divide the chars of s into a new group every char in sep, and put the result in result.
void split(char **result, char *s, char *sep) {
	int i = 0, j, k = 0;
	for (; *s && *s != EOF; s++) {
		//check if the character is in sep
		for (j = 0; sep[j] && *s != sep[j]; j++);
		if (!sep[j]) //reached end of sep so no match
			result[k][i++] = *s;
		else if (i > 0) //noncontiguous match
			i = result[k++][i] = 0;
	}
	if (!sep[j]) //no match
		result[k++][i] = 0; //go one past end and add terminator
	result[k] = 0; // null terminator
}

//will transfer code to a structure
struct func *read(char *s) {
	//split s into lines
	char **a = malloc(MAXLINES);
	struct func *funclist = calloc(sizeof(struct func), MAXLINES);
	int i, j, k;
	for (i = 0; i < MAXLINES; i++)
		a[i] = malloc(LINELEN);
	split(a, s, "\n");
	//Each line is in a[line number]
	for (i = 0; a[i]; i++) {
		//split a[i] into lines
		char **line = malloc(LINELEN/WORDLEN);
		for (j = 0; j < LINELEN/WORDLEN; j++)
			line[j] = malloc(WORDLEN);
		split(line, a[i], " ");
		//put the inputs into funclist[i]
		for (j = 0; j < MAXVALS && line[j] && line[j][0] >= 'a' && line[j][0] <= 'z'; j++)
			strcpy(funclist[i].ins[j], line[j]);
		if (!j) {
			fprintf(stderr, "No inputs were specified for function %s, line %i.\n", funclist[i].name, i + 1);
			exit(0);
		}
		//put the function name into funclist[i]
		if (!line[j]) {
			fprintf(stderr, "No function specified, line %i.\n", i + 1);
			exit(0);
		}
		strcpy(funclist[i].name, line[j++]);
		//put the outputs into funclist[i]
		for (k = 0; k < MAXVALS && line[j] && line[j][0] >= 'a' && line[j][0] <= 'z'; k++)
			strcpy(funclist[i].outs[k], line[j++]);
		funclist[i].satisfied = 0; //inputs are not yet satisfied. I don't know if it needs set.
	}
	funclist[i].name[0] = 0; //null terminator
	return funclist;
}

int indexofvalue(struct value *values, char *value) {
	int i;
	for (i = 0; values[i].name[0] && strcmp(values[i].name, value); i++); //advance to matching value
	if (!values[i].name[0]) //no value found
		return -1;
	return i;
}

//return all values from *f
//TODO: type checking
struct value *makevals(struct func *f) {
	struct value *vals = calloc(sizeof(struct value), MAXLINES * MAXVALS);
	int i, j, k = 0; //index of f, outs, vals
	for (i = 0; f[i].name[0]; i++)
		for (j = 0; f[i].outs[j][0]; j++)
			if (indexofvalue(vals, f[i].outs[j]) == -1) { //value not already in vals
				strcpy(vals[k].name, f[i].outs[j]);
				vals[k++].declared = 0;
			}
	vals[k].name[0] = 0;
	return vals;
}

void printfunc(struct func f) {
	int i;
	printf("name: %s\n", f.name);
	for (i = 0; f.ins[i][0]; i++)
		printf("in: %s, ", f.ins[i]);
	printf("\n");
	for (i = 0; f.outs[i][0]; i++)
		printf("out: %s, ", f.outs[i]);
	printf("\n");
}

//update every func.satisfied flag for a value
void satisfy(struct func *funcs, char *value) {
	printf("starting to satisfy %s\n", value);
	int i, j;
	for (i = 0; funcs[i].name[0]; i++) { //loop through funcs
		printfunc(funcs[i]);
		for (j = 0; funcs[i].ins[j][0] && j < MAXVALS; j++) {//loop through ins
			printf("strcmp(%s, %s) = %i\n", funcs[i].ins[j], value, strcmp(funcs[i].ins[j], value));
			printf("equality: 0=%i, 1=%i, 2=%i, 3=%i a=%i b=%i\n", funcs[i].ins[j][0] == value[0], funcs[i].ins[j][1] == value[1], funcs[i].ins[j][2] == value[2], funcs[i].ins[j][3] == value[3], funcs[i].ins[j][3], value[3]);
			printf("len: %i, %i\n", strlen(funcs[i].ins[j]), strlen(value));
			if (!strcmp(funcs[i].ins[j], value)) { //matches
				//do other functions if they are ready
				printf("presatisfied is %i. 1 << j is %i.\n", funcs[i].satisfied, 1 << j);
				funcs[i].satisfied |= 1 << j;
				printf("satisfied %s at func %i, input %i to #%i.\n", value, i, j, funcs[i].satisfied);
			}
		}
	}
}

//returns true if a function is fully satisfied (only used in makemain)
int issatisfied(struct func *f) {
	printf("is %s satisfied?\n", f->name);
	char i, sat = 0; //iterator, holds full potential satisfaction
	for (i = 0; i < MAXVALS && f->ins[i][0]; i++) //iterate inputs
		sat |= 1 << i; //calculate potential satisfaction
	if (sat == f->satisfied) //f is fully satisfied
		return 1;
	printf("no. %i != %i\n", f->satisfied, sat);
	return 0;
}

//converts a value to C for inputs
char *convvar(char *varname) {
	if (!strcmp(varname, "start"))
		return "0";
	//TODO constants
	else
		return varname;
}

//will run a function funcs[i] and put code into p
void runfunc(char *program, struct value *values, struct func *funcs, int i) {
	printf("Func %s is fully satisfied. Running\n", funcs[i].name);
	int j, k;
	if (funcs[i].outs[1][0]) { //multiple outputs
		fprintf(stderr, "Multiple outputs are not yet supported.\n");
		exit(0);
		for (j = 0; funcs[i].outs[j]; j++) { //iterate outs
			//TODO
		}
		//TODO
	} else { //one output
		char *line = malloc(LINELEN); //stores current line
		//write out something to the effect of "type val = func(ins[0], ins[1], ...);\n"
		//or "val = func(ins[0], ins[1], ...);\n"
		if (funcs[i].outs[0][0]) {
			int valindex = indexofvalue(values, funcs[i].outs[0]);
			if (valindex == -1) {
				fprintf(stderr, "Value %s is taken, but isn't given.\n", funcs[i].outs[0]);
				exit(0);
			}
			//put "type " if needed
			if (!values[valindex].declared) {
				strcat(line, "char ");
				values[valindex].declared = 1;
			}
			//variable "name = "
			strcat(line, values[valindex].name);
			strcat(line, " = ");
		}
		//"func("
		strcat(line, funcs[i].name);
		strcat(line, "(");
		for (j = 0; funcs[i].ins[j][0]; j++) { //iterate through inputs
			strcat(line, convvar(funcs[i].ins[j])); //put input
			if (funcs[i].ins[j+1][0]) //if not the last input
				strcat(line, ", ");
		}
		strcat(line, ");\n");
		printf("Line is: %s", line);
		strcat(program, line); //put the line in the program
		printf("%s will be satisfied.\n", funcs[i].outs[0]);
		printf("len is %i\n", funcs[i].outs[0]);
		satisfy(funcs, funcs[i].outs[0]); //satisfy the current function
	}
}

//make a program and put it in program
void makemain(char *program, struct value *values, struct func *funcs) {
	strcpy(program, "#include<flang.h>\n\nint main() {\n");
	satisfy(funcs, "start");
	int anyprocessed = 1, i;
	while (anyprocessed) { //keep processing until you can't anymore.
		anyprocessed = 0;
		for (i = 0; funcs[i].name[0]; i++)
			if (issatisfied(funcs + i)) {
				runfunc(program, values, funcs, i);
				funcs[i].satisfied = 0;
				anyprocessed = 1;
			}
	}
	strcat(program, "}\n");
}

int main() {
	printf("main: started\n");
	char *flangprogram = malloc(MAXLINES * LINELEN);
	fread(flangprogram, sizeof(char), MAXLINES * LINELEN, stdin);
	printf("Inputted program is m\"%s\"\n", flangprogram);
	struct func *funcs = read(flangprogram);
	printf("main: finished reading\n");
	struct value *values = makevals(funcs);
	int i;
	for (i = 0; values[i].name[0]; i++) //advance to matching value
		printf("main: value[%i] = %s;\n", i, values[i].name);
	char *program = malloc(MAXLINES * LINELEN);
	makemain(program, values, funcs);
	printf("main: output:\n%s", program);
	return 0;
}
