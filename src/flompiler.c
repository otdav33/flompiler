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

void split(char **result, char *s, char *sep) {
	int i = 0, j;
	for (; *s != '\0' && *s != EOF; s++) {
		for (j = 0; *s != sep[j] && sep[j] != '\0'; j++);
		if (sep[j] == '\0')
			(*result)[i++] = *s;
		else {
			(*result++)[i] = '\0';
			i = 0;
		}
	}
	*result = 0;
}

//will transfer code to a structure
struct func *read(char *s) {
	char **a = malloc(MAXLINES);
	struct func *funclist = calloc(sizeof(struct func), MAXLINES);
	int i, j, k;
	for (i = 0; i < MAXLINES; i++)
		a[i] = malloc(LINELEN);
	split(a, s, "\n");
	for (i = 0; a[i]; i++) {
		char **line = malloc(LINELEN/WORDLEN);
		for (j = 0; j < LINELEN/WORDLEN; j++)
			line[j] = malloc(WORDLEN);
		split(line, a[i], " ");
	printf("end\n");
		for (j = 0; line[j][0] >= 'a' && line[j][0] <= 'z'; j++)
			strcpy(funclist[i].ins[j], line[j]);
		strcpy(funclist[i].name, line[j++]);
		k = 0;
		while (line[j])
			strcpy(funclist[i].outs[k++], line[j++]);
	}
}

main() {
	struct func *funcs = read("test Test test test\nval Func val");
	int i;
	while (funcs->name) {
		printf("name: %s\n", funcs->name);
		for (i = 0; i < MAXVALS; i++)
			printf("in: %s, ", funcs->ins[i]);
		printf("\n");
		for (i = 0; i < MAXVALS; i++)
			printf("out: %s, ", funcs->outs[i]);
		funcs++;
	}
}
