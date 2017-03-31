#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXVALS 8 //number of inputs/outputs per function max
#define WORDLEN 100 //largest word size
#define LINELEN 5000 //largest line size
#define MAXLINES 5000 //largest line size

struct func {
	char ins[MAXVALS][WORDLEN], outs[MAXVALS][WORDLEN], name[WORDLEN]; //inputs and outputs, name of function
	char satisfied; //mask for if the inputs are satisfied
};

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
		for (j = 0; line[j][0] >= 'a' && line[j][0] <= 'z'; j++)
			strcpy(funclist[i].ins[j], line[j]);
		//put the function name into funclist[i]
		strcpy(funclist[i].name, line[j++]);
		//put the outputs into funclist[i]
		for (k = 0; line[j] && line[j][0] >= 'a' && line[j][0] <= 'z'; k++)
			strcpy(funclist[i].outs[k], line[j++]);
	}
	return funclist;
}

main() {
	struct func *funcs = read("test Test test test\nval Func val");
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
}
