#include<stdio.h>
#include<string.h>
#include<expat.h>

#define MAXCHAR 1000000
#define NUMOUTPUTS 4 //real number is NUMOUTPUTS
#define LENOUTPUTS 9 //real number is LENOUTPUTS-1
#define GATELEN 48
#define FUNCNAMELEN 32

struct point { //coordinate
	double x, y;
};

struct gate { //implemented function
	char name[FUNCNAMELEN], numinputs, numoutputs; //builtin name, length of below arrays
	struct point i[NUMOUTPUTS], o[NUMOUTPUTS]; //coordinates of inputs and outputs, null terminated
};

struct func { //used function
	char name[FUNCNAMELEN]; //builtin name
	struct func *o[NUMOUTPUTS+1][LENOUTPUTS]; //outputs, null terminated
};

int Depth, Gatelistnum; //current XML tree depth, current Gatelist id
char Defs, Gate; //How far is the current defs tag? gate? (0 if not inside)
struct gate Gatelist[GATELEN]; //list of all gates

const XML_Char *val4key(const XML_Char **attr, const XML_Char *key) { //Gives the b in a="b" for (array, a)
	int i;
	for (i = 0; attr[i]; i += 2)
		if (strcmp(attr[i], key) == 0)
			return attr[i + 1];
	return 0;
}

void ptrarrncpy(struct point *a, struct point *b, int cap) { //like strncpy, but for *points instead of chars
	int i;
	for (i = 0; i < cap; i++)
		a[i] = b[i];
}
void gatecpy(struct gate *a, struct gate *b) { //copies all but name from gate b to a
	a->numinputs = b->numinputs;
	(*a).numoutputs = b->numoutputs;
	ptrarrncpy(a->i, b->i, b->numinputs);
	ptrarrncpy(a->o, b->o, b->numoutputs);
}

void printelem(const XML_Char *name, const XML_Char **attr) { //prints tag info
	int i;
	printf("elem");
	for (i = 0; i < Depth; i++)
		printf("  ");
	printf("%s", name);
	for (i = 0; attr[i]; i += 2)
		printf(" %s='%s'", attr[i], attr[i + 1]);
	printf("\n");
}

void start(void *data, const XML_Char *name, const XML_Char **attr) {
	if (!Defs && !strncmp(name, "defs", 4)) { //if tag name is "defs", and we aren't in defs
		Defs = Depth; //make it known that we are in a defs tag.
	} else if (Defs && !Gate && !strcmp(name, "g")) { //if tag name is "g", and we are in defs
		const XML_Char *id = val4key(attr, "id");
		if (id && strcmp(id, "#node")) { //if there is an id and it's not "#node"
			Gate = Depth;
			strncpy(Gatelist[Gatelistnum].name, id, FUNCNAMELEN);
		}
	} else if (Defs && !strcmp(name, "use")) {
		const XML_Char *href = val4key(attr, "xlink:href"); //TODO: add implementation for #nn and ##node
		const XML_Char *x = val4key(attr, "x");
		const XML_Char *y = val4key(attr, "y");
		if (!strcmp(href, "##node")) {
			const XML_Char *class = val4key(attr, "class");
			//Set the node in question to the value at hand
			struct point p = {atof(x), atof(y)};
			if (class[0] == 'i') {
				Gatelist[Gatelistnum].i[class[1]-'0'-1] = p;
				Gatelist[Gatelistnum].numinputs++;
			}
			if (class[0] == 'o') {
				Gatelist[Gatelistnum].o[class[1]-'0'-1] = p;
				Gatelist[Gatelistnum].numoutputs++;
			}
		} else { //TODO: fix the following hack.
			const XML_Char *gname = href + 1; //SVG referenced gate name
			printf("gname is %s.\n", gname);
			int i;
			for (i = 0; i < GATELEN && Gatelist[i].name != gname; i++); //advances to the referred gate
			gatecpy(Gatelist + Gatelistnum, Gatelist + i); //copies Gatelist[Gatelistnum] <-- Gatelist[i]
		}//End broken hack
	}
	//printelem(name, attr); //useful for debugging
	Depth++;
}

void text(void *data, const XML_Char *s, int len) {//prints text content of an XML element
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
	//XML_SetCharacterDataHandler(parser, &text); //useful for debugging
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
	int i, j;
	for (i = 0; i < GATELEN && Gatelist[i].name[0]; i++) {
		printf("Gate %s; %i, %i, i:", Gatelist[i].name, Gatelist[i].numinputs, Gatelist[i].numoutputs);
		for (j = 0; j < Gatelist[i].numinputs; j++)
			printf(" (%f,%f)", Gatelist[i].i[j].x, Gatelist[i].i[j].y);
		printf("; o:");
		for (j = 0; j < Gatelist[i].numoutputs; j++)
			printf(" (%f,%f)", Gatelist[i].o[j].x, Gatelist[i].o[j].y);
		putchar('\n');
	}
	return 0;
}