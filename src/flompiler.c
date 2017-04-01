#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"flompiler.h"

//will divide the chars of s into a new group every char in sep, and put the result in result.
void split(char **result, char *s, char *sep) {
	int i = 0, j, k = 0;
	for (; *s != '\0' && *s != EOF; s++) {
		//check if the character is in sep
		for (j = 0; *s != sep[j] && sep[j] != '\0'; j++);
		//char is not in sep
		if (sep[j] == '\0')
			result[k][i++] = *s;
		//char is in sep
		else if (i > 0) { //if there is a sep at the end, do nothing.
			result[k++][i] = '\0';
			i = 0;
		}
	}
	//It is important to put the null terminator after the end.
	result[k+1] = 0;
}

//converts a value to C for inputs
char *convvar(char *varname) {
	if (!strcmp(varname, "start"))
		return "0";
	//TODO constants
	else
		return varname;
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
		for (j = 0; j < MAXVALS && line[j][0] >= 'a' && line[j][0] <= 'z'; j++)
			strcpy(funclist[i].ins[j], line[j]);
		//put the function name into funclist[i]
		strcpy(funclist[i].name, line[j++]);
		//put the outputs into funclist[i]
		for (k = 0; j < MAXVALS && line[j][0] >= 'a' && line[j][0] <= 'z'; k++)
			strcpy(funclist[i].outs[k], line[j++]);
		funclist[i].satisfied = 0; //inputs are not yet satisfied. I don't know if it needs set.
	}
	return funclist;
}

//This should be run when a value is satisfied.
//p is the C program text, funcs is all functions, and value is the satisfied value.
void satisfy(char *program, struct value *values, struct func *funcs, char *value) {
	int i, j, k;
	for (i = 0; funcs[i].name[0]; i++) //loop through funcs
		for (j = 0; funcs[i].ins[j][0] && j < 8; j++) //loop through ins
			if (!strcmp(funcs[i].ins[j], value)) { //matches
				//do other functions if they are ready
				funcs[i].satisfied |= 1 << j;
				char sat = 0;
				if (!funcs[i].ins[j+1]) {
					for (k = j; k >= 0; k--) //ins for satisfied
						sat |= 1 << k;
					if (sat == funcs[i].satisfied)
						runfunc(program, values, funcs);
				}
			}
}

int indexofvalue(struct value *values, char *value) {
	int i = 0;
	while (values[i].name[0] && strcmp(values[i].name, value)) i++; //advance to matching value
	return i;
}

//will run a function funcs[i] and put code into p
void runfunc(char *program, struct value *values, struct func *funcs) {
	int i = strlen(program), j, k, l;
	for (j = 0; program[j]; j++); //go to end of program
	if (funcs[i].outs[1]) { //multiple outputs
		for (k = 0; funcs[i].outs[k]; k++) { //iterate outs
			//TODO
		}
		//TODO
	} else { //one output
		//write out something to the effect of "type val = func(ins[0], ins[1], ...);\n"
		//or "val = func(ins[0], ins[1], ...);\n"
		struct value *out = values + indexofvalue(values, funcs[i].outs[0]);
		//put "type " if needed
		if (!out->declared) {
			/*
				 for (k = 0; out->type[k]; k++)
				 program[j++] = out->type[k];
			 */
			program[j++] = 'c';
			program[j++] = 'h';
			program[j++] = 'a';
			program[j++] = 'r';
			program[j++] = ' ';
			out->declared = 1;
		}
		//variable "name = "
		for (k = 0; out->name[k]; k++)
			program[j++] = out->name[k];
		program[j++] = ' ';
		program[j++] = '=';
		program[j++] = ' ';
		//"func("
		for (k = 0; funcs[i].name[k]; k++)
			program[j++] = funcs[i].name[k];
		program[j++] = '(';
		for (k = 0; funcs[i].ins[k][0]; k++) { //iterate through inputs
			//translate and find value
			char *cval = convvar(funcs[i].ins[k]);
			for (l = 0; cval[l]; l++)
				program[j++] = cval[l];
			if (funcs[i].ins[k+1][0]) {
				program[j++] = ',';
				program[j++] = ' ';
			}
		}
		program[j++] = ')';
		program[j++] = ';';
		program[j++] = '\n';
	}
}

//make a program and put it in program
void makemain(char *program, struct value *values, struct func *funcs) {
	strcpy(program, "#include<flang.h>\n\nint main() {\n");
	satisfy(program, values, funcs, "start"); //process
	strcpy(program + strlen(program), "}\n");
}

int main() {
	char *program = malloc(MAXLINES * LINELEN);
	struct func *funcs = read("start Test test test\nval Func val");
	struct value *values = malloc(sizeof(struct value) * MAXLINES);
	int i, j;
	for (j = 0; funcs->name[0] && j < 10; j++) {
		printf("name: %s\n", funcs->name);
		for (i = 0; i < MAXVALS && funcs->ins[i][0]; i++)
			printf("in: %s, ", funcs->ins[i]);
		printf("\n");
		for (i = 0; i < MAXVALS && funcs->outs[i][0]; i++)
			printf("out: %s, ", funcs->outs[i]);
		printf("\n");
		funcs++;
	}
	makemain(program, values, funcs);
	printf("output:\n%s", program);
	return 0;
}
