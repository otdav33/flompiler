#include<stdio.h>
#include<string.h>
#include<expat.h>

#define MAXCHAR 1000000
#define NUMOUTPUTS 5 //real number is NUMOUTPUTS-1
#define LENOUTPUTS 9 //real number is LENOUTPUTS-1
#define GATELEN 48

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

int Depth, Gatelistnum; //current XML tree depth, current Gatelist id
char Defs, Gate; //How far is the current defs tag? gate? (0 if not inside)
struct gate Gatelist[GATELEN]; //list of all gates

const XML_Char *val4key(const XML_Char **attr, const XML_Char *key) {
	int i;
	for (i = 0; attr[i]; i += 2)
		if (strcmp(attr[i], key) == 0)
			return attr[i + 1];
	return 0;
}

void start(void *data, const XML_Char *name, const XML_Char **attr) {
	if (!Defs && !strncmp(name, "defs", 4)) { //if tag name is "defs", and we aren't in defs
		Defs = Depth; //make it known that we are in a defs tag.
	}
	else if (!Gate && !strcmp(name, "g")) { //if tag name is "g", and we are in defs
		const XML_Char *id = val4key(attr, "id");
		if (id && strcmp(id, "#node")) { //if there is an id and it's not "#node"
			Gate = Depth;
			Gatelist[Gatelistnum].name = id;
		}
	} else if (!strcmp(name, "use")) {
		const XML_Char *href = val4key(attr, "xlink:href"); //TODO: add implementation for #nn and ##node
		const XML_Char *x = val4key(attr, "x");
		const XML_Char *y = val4key(attr, "y");
		if (!strcmp(href, "##node")) {
			const XML_Char *class = val4key(attr, "class");
			//Set the node in question to the value at hand
			if (class[0] == 'i')
				Gatelist[Gatelistnum].i[atoi(class[1])-1] = {atof(x), atof(y)};
			if (class[0] == 'o')
				Gatelist[Gatelistnum].o[atoi(class[1])-1] = {atof(x), atof(y)};
		} else { //TODO: fix the following hack.
			const XML_Char gname = href + 1; //SVG referenced gate name
			int i;
			for (i = 0; i < GATELEN && Gatelist[i].name != gname; i++); //advances to the referred gate
			Gatelist[Gatelistnum].i = Gatelist[i].i;
			Gatelist[Gatelistnum].o = Gatelist[i].o;
		}//End broken hack
	}
	//TODO: other stuff
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
	if (Defs == Depth)
		Defs = 0; //If we leave a defs tag, make it known.
	if (Gate == Depth) {
		Gate = 0; //If we leave a gate tag, make it known.
		Gatelistnum ++;
	}
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
