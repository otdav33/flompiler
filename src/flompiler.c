#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"flompiler.h"

//will divide the chars of s into a new group every char in sep, and put the result in result. WARNING: destroys *s
char **split(char *s, char sep) {
	char **result = malloc(MAXLINES);
	char *next; //next found sep
	int i = 0, dist; //index of result
	while (*s && (next = strchr(s, sep))) {
		dist = next - s;
		if (dist) { //if you aren't on a sep
			result[i] = malloc(dist+1); //need an extra for the null.
			strncpy(result[i], s, dist); //slice it and move up
			result[i++][dist] = '\0'; //put on terminator
		}
		s = 1 + next;
	}
	result[i] = malloc(strlen(s)); //need an extra for the null.
	strcpy(result[i++], s); //put on the last string.
	result[i] = ""; //null terminator (points to the memory address of a baked-in "" string.
	return result;
}

//will transfer code to a structure
void read(struct scope *scopes, char *escaped, char *s) {
	//i = line index, j = word index, k = output index, f = normal line index, si = scope index
	int i, j, k, f = 0, si = 0;
	char **a = split(s, '\n');
	//iterate through lines
	for (i = 0; a[i][0]; i++) {
		if (a[i][0] == '#') {
			strcat(escaped, a[i]);
			strcat(escaped, "\n");
			printf("catted line '%s'\n", a[i]);
			continue;
		}
		printf("normal line '%s' %i\n", a[i], f);
		//split a[i] into lines
		char **words = split(a[i], ' ');

		char currentlineislambda = 0;
		//find a lambda
		for (j = 0; j < MAXVALS && words[j][0]; j++)
			if (words[j][0] == ';') {
				//switch scopes
				if (f) {
					//final stuff
					scopes[si].f[f].name[0] = 0; //null terminator
					f = 0;
				}
				currentlineislambda = 1;
				break;
			}
		if (!f && !currentlineislambda) {
			fprintf(stderr, "the first line must be a lambda.\n");
			exit(1);
		}
		//put the inputs into scopes[si].f[i]
		printf("words[0] = '%s'\n", words[0]);
		for (j = 0; j < MAXVALS && words[j][0] >= 'a' && words[j][0] <= 'z'; j++)
			strcpy(scopes[si].f[f].ins[j], words[j]);
		//put the function name into scopes[si].f[f]
		if (!words[j]) {
			fprintf(stderr, "No function specified, line %i.\n", i + 1);
			exit(1);
		}
		strcpy(scopes[si].f[f].name, words[j++]);
		//put the outputs into scopes[si].f[f]
		for (k = 0; k < MAXVALS && words[j][0] >= 'a' && words[j][0] <= 'z'; k++)
			strcpy(scopes[si].f[f].outs[k], words[j++]);
		scopes[si].f[f++].satisfied = 0; //inputs are not yet satisfied. I don't know if it needs set.
		//for (j = 0; j < LINELEN; j++)
		//free(words[j]);
		//free(words);
	}
	//final stuff
	scopes[si].f[f].name[0] = 0; //null terminator
	//for (i = 0; i < MAXLINES; i++)
	//free(a[i]);
	free(a);
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

//only return the name before the "$" if there is a "$"
char *beforedollar(char *s) {
	char *r = malloc(WORDLEN);
	strcpy(r, s);
	char *posdollar = strchr(r, '$');
	if (posdollar)
		r[posdollar - r] = '\0';
	return r;
}

//update every func.satisfied flag for a value
void satisfy(char *program, struct func *funcs, char *value) {
	printf("starting to satisfy %s\n", value);
	int i, j;
	for (i = 0; funcs[i].name[0]; i++) { //loop through funcs
		printfunc(funcs[i]);
		for (j = 0; funcs[i].ins[j][0] && j < MAXVALS; j++) { //loop through ins
			if (!strcmp(beforedollar(funcs[i].ins[j]), beforedollar(value))) { //matches
				//do other functions if they are ready
				printf("presatisfied is %i. 1 << j is %i.\n", funcs[i].satisfied, 1 << j);
				funcs[i].satisfied |= 1 << j;
				printf("satisfied %s at func %i, input %i to #%i.\n", value, i, j, funcs[i].satisfied);
				if (issatisfied(funcs + i)) {
					runfunc(program, funcs, i);
					funcs[i].satisfied = 0;
				}
			}
		}
	}
}

//will give the "type var" or "var" depending on which is appropriate.
char *decl(char *s) {
	char *line = malloc(LINELEN); //return value
	//put "type " if needed
	char *posdollar = strchr(s, '$');
	if (posdollar) {
		strcpy(line, posdollar + 1);
		strcat(line, " ");
	} else
		strcpy(line, "double ");
	//variable "name = "
	char *bd = beforedollar(s);
	printf("bd of '%s' is '%s'\n", s, bd);
	strcat(line, bd);
	strcat(line, ";\n");
	free(bd);
	return line;
}

//will run a function funcs[i] and put code into p
void runfunc(char *program, struct func *funcs, int i) {
	printf("running %s\n", funcs[i].name);
	int j, k;
	char *line = malloc(LINELEN); //stores current line
	strcpy(line, "");
	if (funcs[i].name[0] == '#' || funcs[i].name[0] == '\'') { //is constant
		for (j = 0; funcs[i].outs[j][0]; j++) { //iterate through outputs
			//do the "type var = "
			char *rvalue = malloc(WORDLEN);
			if (funcs[i].name[0] == '#') {
				strcpy(rvalue, funcs[i].name + 1);
			} else {
				strcpy(rvalue, "\'");
				strcat(rvalue, funcs[i].name + 1);
				strcat(rvalue, "'");
			}
			strcat(line, beforedollar(funcs[i].outs[j]));
			strcat(line, " = ");
			strcat(line, rvalue);
			free(rvalue);
			//do the "val;\n"
			strcat(line, ";\n");
		}
		printf("line: '%s'\n", line);
		strcat(program, line); //put the line in the program
		for (j = 0; funcs[i].outs[j][0]; j++) //iterate through outputs
			satisfy(program, funcs, funcs[i].outs[j]); //satisfy the output
	} else if (funcs[i].outs[1][0]) { //multiple outputs
		fprintf(stderr, "Multiple outputs are not yet supported.\n");
		exit(1);
		//TODO
	} else { //one output
		if (funcs[i].outs[0][0]) {
			strcat(line, beforedollar(funcs[i].outs[0]));
			strcat(line, " = ");
		}
		//write out something to the effect of "type val = func(ins[0], ins[1], ...);\n"
		//or "val = func(ins[0], ins[1], ...);\n"
		//"func("
		if (funcs[i].name[0] == '@')
			strcat(line, funcs[i].name + 1);
		else
			strcat(line, funcs[i].name);
		strcat(line, "(");
		for (j = 0; funcs[i].ins[j][0]; j++) { //iterate through inputs
			strcat(line, strcmp(funcs[i].ins[j], "start") ? funcs[i].ins[j] : "0"); //put input
			if (funcs[i].ins[j+1][0]) //if not the last input
				strcat(line, ", ");
		}
		strcat(line, ");\n");
		strcat(program, line); //put the line in the program
		if (funcs[i].outs[0][0]) //check if there is an output
			satisfy(program, funcs, funcs[i].outs[0]); //satisfy the output
	}
	free(line);
}

int main() {
	printf("main: started\n");
	char *flangprogram = malloc(MAXLINES * LINELEN);
	fread(flangprogram, sizeof(char), MAXLINES * LINELEN, stdin);
	printf("Inputted program is \"%s\"\n", flangprogram);
	char *program = malloc(MAXLINES * LINELEN);
	struct scope *scopes = calloc(sizeof(struct scope), MAXSCOPES);
	strcpy(program, "");
	read(scopes, program, flangprogram);
	printf("main: finished reading\n");
	strcat(program, "\nint main() {\n");
	int i, j;
	for (i = 0; scopes[0].f[i].name[0]; i++)
		for (j = 0; scopes[0].f[i].outs[j][0]; j++) {
			printf("main: scopes[0].f[%i].outs[%i] = '%s';\n", i, j, scopes[0].f[i].outs[j]);
			char *d = decl(scopes[0].f[i].outs[j]);
			strcat(program, d);
			free(d);
		}
	for (i = 0; scopes[0].f[i].name[0]; i++)
		if (!scopes[0].f[i].ins[j][0] && scopes[0].f[i].name[0] != ';')
			runfunc(program, scopes[0].f, i);
	satisfy(program, scopes[0].f, "start");
	strcat(program, "}\n");
	printf("main: output:\n%s", program);
	return 0;
}
