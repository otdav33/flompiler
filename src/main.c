#include"flompiler.h"
#include<expat.h>
#include<stdio.h>

void printfuncs(struct defunc *f) {
	int i, j;
	for (i = 0; i < FUNCTYPES && f[i].name[0]; i++) {
		printf("Func %s; %i, %i, i:", f[i].name, f[i].numi, f[i].numo);
		for (j = 0; j < f[i].numi; j++)
			printf(" (%f,%f)", f[i].i[j].x, f[i].i[j].y);
		printf("; o:");
		for (j = 0; j < f[i].numo; j++)
			printf(" (%f,%f)", f[i].o[j].x, f[i].o[j].y);
		putchar('\n');
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage is %s filename.svg.\n", argv[0]);
		return 1;
	}
	XML_Parser parser = XML_ParserCreate(0); //setup for parser
	XML_SetElementHandler(parser, &start, &end); //tag handler setup
	//file setup
	FILE *file = fopen(argv[1], "r");
	if (file == 0) {
		printf("file invalid");
		return 1;
	}
	XML_Char str[MAXCHAR];
	size_t len = fread(str, sizeof(XML_Char), MAXCHAR, file);
	//setup memory variable
	struct data d;
	XML_SetUserData(parser, &d);
	//do actual work
	if (!XML_Parse(parser, str, len, 1)) {
		printf("expat error '%s'\n", XML_ErrorString(XML_GetErrorCode(parser)));
		return 1;
	}
	XML_ParserFree(parser);
	//Print out the gathered function types
	int i;
	for (i = 0; i < d.APC; i += 2)
		printf("Arrow: (%f, %f), (%f, %f)\n", d.APL[i].x, d.APL[i].y, d.APL[i+1].x, d.APL[i+1].y);
	printf("----------------\n");
	printfuncs(d.DFL);
	printf("----------------\n");
	printfuncs(d.UFL);
	return 0;
}
