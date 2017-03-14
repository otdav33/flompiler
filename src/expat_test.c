#include<stdio.h>
#include<string.h>
#include<expat.h>

#define MAXCHAR 1000000
#define NUMOUTPUTS 5 //real number is NUMOUTPUTS-1
#define LENOUTPUTS 9 //real number is LENOUTPUTS-1

struct point { //coordinate
	int x, y;
};

struct gate { //implemented function
	char name[32]; //builtin name
	struct point *i[NUMOUTPUTS], *o[NUMOUTPUTS]; //coordinates of inputs and outputs, null terminated
};

struct func { //used function
	char name[32]; //builtin name
	struct func *o[NUMOUTPUTS][LENOUTPUTS]; //outputs, null terminated
};

int Depth, DefsDepth; //current XML tree depth, depth of the defs tag
char Defs; //are we in a defs tag?
struct gate Gatelist[48]; //list of all gates

const XML_Char *val4key(const XML_Char **attr, const XML_Char *key) {
	int i;
	for (i = 0; attr[i]; i += 2)
		if (strcmp(attr[i], key) == 0)
			return attr[i + 1];
	return 0;
}

void start(void *data, const XML_Char *name, const XML_Char **attr) {
	if (!Defs && strncmp(name, "defs", 4) == 0) { //if tag name is "defs"
		Defs = 1; //make it known that we are in a defs tag.
		DefsDepth = Depth;
	}
	if (!Defs && strcmp(name, "g") == 0) { //if tag name is "g"
		const XML_Char *id = val4key(attr, "id");
		if (id) {




	int i;
	printf("elem");
	for (i = 0; i < Depth; i++)
		printf("  ");
	printf("%s", name);
	for (i = 0; attr[i]; i += 2)
		printf(" %s='%s'", attr[i], attr[i + 1]);
	printf("\n");
	Depth++;
}

void text(void *data, const XML_Char *s, int len) {
	if (len < 2)
		return;
	int i;
	printf("text");
	for (i = 0; i < Depth; i++)
		printf("  ");
	char temp[len + 1];
	strncpy(temp, s, len);
	temp[len] = '\0';
	printf("%s\n", temp);
}

void end(void *data, const XML_Char *name) {
	Depth--;
	if (Defs && DefsDepth == Depth)
		Defs = 0; //If we leave a defs tag, make it known.
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage is %s filename.svg.\n", argv[0]);
		return 1;
	}
	XML_Parser parser = XML_ParserCreate(0);
	XML_SetElementHandler(parser, &start, &end);
	XML_SetCharacterDataHandler(parser, &text);
	FILE *file = fopen(argv[1], "r");
	if (file == 0) {
		printf("file invalid");
		return 1;
	}
	XML_Char str[MAXCHAR];
	size_t len = fread(str, sizeof(XML_Char), MAXCHAR, file);
	if (!XML_Parse(parser, str, len, 1)) {
		printf("expat error '%s'\n", XML_ErrorString(XML_GetErrorCode(parser)));
		return 1;
	}
	XML_ParserFree(parser);
	return 0;
}
