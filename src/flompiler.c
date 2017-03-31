#include<stdio.h>
#include<stdlib.h>

#define MAXVALS 8 //number of inputs/outputs per function max
#define WORDLEN 100 //largest word size
#define LINELEN 5000 //largest line size
#define MAXLINES 5000 //largest line size

struct funcs {
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
struct funcs *read(char *s) {
	//TODO
}

main() {
	char **a = malloc(WORDLEN);
	int i;
	for (i = 0; i < WORDLEN; i++)
		a[i] = malloc(WORDLEN);
	split(a, "stuff is here.", " .");
	while (*a)
		printf("word is %s\n", *(a++));
}
