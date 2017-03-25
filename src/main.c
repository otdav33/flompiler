#include"flompiler.h"
#include<stdio.h>

void printfuncs(struct defunc *f) {
	int i, j;
	for (i = 0; i < FUNCTYPES && f[i].name[0]; i++) {
		printf("Func %i: %s; %i, %i, i:", i, f[i].name, f[i].numi, f[i].numo);
		for (j = 0; j < f[i].numi; j++)
			printf(" (%f,%f)", f[i].i[j].x, f[i].i[j].y);
		printf("; o:");
		for (j = 0; j < f[i].numo; j++)
			printf(" (%f,%f)", f[i].o[j].x, f[i].o[j].y);
		putchar('\n');
	}
}

void printtfuncs(struct tfunc *f, int indent) { //watch the extra t
	int i, j, k = indent;
	while (k-- > 0)
		printf(" ");
	printf("TFUNC: %s; %i\n", f->name, f->numo);
	for (i = 0; i < f->numo; i++)
		for (j = 0; f->o[i][j]; j++)
			printtfuncs(f->o[i][j], indent + 1);
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
	d.DFL = calloc(sizeof(struct defunc), INITIALFUNCS);
	d.UFL = calloc(sizeof(struct defunc), FUNCTYPES);
	d.APL = calloc(sizeof(struct point), 2 * INITIALFUNCS); //needs to have even length
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
	printf("----------------\n");
	struct tfunc tfuncs[MAXFUNCS];
	connect(tfuncs, &d);
	free(d.DFL);
	free(d.UFL);
	printtfuncs(tfuncs, 0);
	free(d.APL);
	return 0;
}
