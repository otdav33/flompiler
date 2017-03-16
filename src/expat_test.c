#include<stdio.h>
#include<string.h>
#include<expat.h>

#define MAXCHAR 1000000
#define NUMOUTPUTS 4 //maximum number of function outputs
#define LENOUTPUTS 9 //maximum of node branches (real number is LENOUTPUTS-1)
#define GATELEN 48
#define FUNCNAMELEN 32
#define MAXFUNC 10000 //Maximum number of functions allowed in the program

struct point { //coordinate
	double x, y;
};

struct gate { //implemented function
	char name[FUNCNAMELEN], numinputs, numoutputs; //builtin name, length of below arrays
	struct point i[NUMOUTPUTS], o[NUMOUTPUTS]; //coordinates of inputs and outputs, null terminated
};

struct connection {
	struct func *o; //output
	int n; //input # for the above func
};

struct connections {
	struct connection c[LENOUTPUTS]; //connections
	int n; //number of connections
};

struct func { //used function after connection, or inside statement
	char name[FUNCNAMELEN]; //builtin name
	struct connection c[NUMOUTPUTS+1][LENOUTPUTS]; //null terminated
};

struct statement { //used function before connection
	struct point *p;
	struct gate *g;
	struct func *f;
};

struct arrow {
	struct point a, b;
};

int Depth, Gatenum, Stmtnum, Arrownum; //current XML tree depth, current Gatelist id, etc.
char Defs, Gate; //How far is the current defs tag? gate? (0 if not inside)
struct gate Gatelist[GATELEN]; //list of all gates
struct statement Stmtlist[MAXFUNC];
struct func Funclist[MAXFUNC];
struct arrow Arrowlist[MAXFUNC * LENOUTPUTS]; //could overflow

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

int isdigit(const XML_Char c) { //return 1 if floating point char, otherwise 0
	const XML_Char *d = "+-0123456789.";
	int i;
	for (i = 13; i >= 0; i--) 
		if (c == d[i])
			return 1;
	return 0;
}
int issep(const XML_Char c) { //return 1 if arrow's seperator characters, otherwise 0
	const XML_Char *d = "M ,";
	int i;
	for (i = 3; i >= 0; i--)
		if (c == d[i])
			return 1;
	return 0;
}

void addarrow(const XML_Char *d) {
	XML_Char tmp[400];
	int i, j;
	for (i = 0; issep(d[i]); i++); //skip to first number
	for (j = 0; isdigit(d[i]); i++) //go through number
		tmp[j++] = d[i];
	if (j == 0) {
		printf("A path's d attribute is invalid. Value was %s\n", d);
		exit(1);
	}

	tmp[j] = '\0'; //don't forget that string terminator
	Arrowlist[Arrownum].a.x = atof(tmp); //record that number as the first x
	while (issep(d[i])) i++; //skip to second number
	for (j = 0; isdigit(d[i]); i++) //go through number
		tmp[j++] = d[i];
	if (j == 0) {
		printf("A path's d attribute is invalid. Value was %s\n", d);
		exit(1);
	}
	tmp[j] = '\0';
	Arrowlist[Arrownum].a.y = atof(tmp); //record that number as the first y
	while (d[i] != '\0' && d[i] != '\n') i++; //skip to end
	i--; //back up one char
	//back up two numbers
	while (isdigit(d[i])) i--;
	while (issep(d[i])) i--;
	while (isdigit(d[i])) i--;
	i++; //we are at the start of the x of the last coordinate.
	for (j = 0; isdigit(d[i]); i++)
		tmp[j++] = d[i];
	if (j == 0) {
		printf("A path's d attribute is invalid. Value was %s\n", d);
		exit(1);
	}
	tmp[j] = '\0';
	Arrowlist[Arrownum].b.x = atof(tmp); //record as the second x
	while (issep(d[i])) i++;
	for (j = 0; isdigit(d[i]); i++)
		tmp[j++] = d[i];
	if (j == 0) {
		printf("A path's d attribute is invalid. Value was %s\n", d);
		exit(1);
	}
	tmp[j] = '\0';
	Arrowlist[Arrownum].b.y = atof(tmp);
	Arrownum++; //move on to the next arrow
}

void addstatement(const XML_Char *attr) {
	//shove as much as currently possible into Stmtlist
	Stmtlist[Stmtnum].p->x = val4key(attr, "x");
	*(Stmtlist[Stmtnum].p).y = val4key(attr, "y");
	const XML_Char *name = val4key(attr, "xlink:href") + 1;
	strcpy(Stmtlist[Stmtnum].f.name, name);
	int i;
	for (i = 0; strcmp(Gatelist[i].name, name) && i < GATELEN; i++);
	if (i == GATELEN) {
		printf("%s used before definition", name);
		exit(1);
	}
	Stmtlist[Stmtnum].g = Gatelist[i];
	Stmtnum++;
}

void start(void *data, const XML_Char *name, const XML_Char **attr) {
	if (!Defs && !strcmp(name, "defs")) { //if tag name is "defs", and we aren't in defs
		Defs = Depth; //make it known that we are in a defs tag.
	} else if (Defs && !Gate && !strcmp(name, "g")) { //if tag name is "g", and we are in defs
		const XML_Char *id = val4key(attr, "id");
		if (id && strcmp(id, "#node")) { //if there is an id and it's not "#node"
			Gate = Depth;
			strncpy(Gatelist[Gatenum].name, id, FUNCNAMELEN);
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
				Gatelist[Gatenum].i[class[1]-'0'-1] = p;
				Gatelist[Gatenum].numinputs++;
			}
			if (class[0] == 'o') {
				Gatelist[Gatenum].o[class[1]-'0'-1] = p;
				Gatelist[Gatenum].numoutputs++;
			}
		} else { //TODO: fix the following hack.
			const XML_Char *gname = href + 1; //SVG referenced gate name
			int i;
			for (i = 0; i < GATELEN && !strcmp(Gatelist[i].name, gname); i++); //advances to the referred gate
			gatecpy(Gatelist + Gatenum, Gatelist + i); //copies Gatelist[Gatelistnum] <-- Gatelist[i]
		}//End broken hack
	} else if (!Defs && !strcmp(name, "path")) {
		addarrow(val4key(attr, "d"));
	} else if (!Defs && !strcmp(name, "use")) {
		addstatement(attr);
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
		Gatenum ++;
	}
}

struct point pointadd(struct point a, struct point b) {
	struct point p = {a.x + b.x, a.y + b.y};
	return p;
}

int near(struct point a, struct point b, double dist) { //Decides if the points overlap, with max distance dist
	//TODO: circular rather than square nodes
	double diffx = a.x - b.x, diffy = a.y - b.y;
	if (diffx > dist || diffx < -dist)
		return 0;
	if (diffy > dist || diffy < -dist)
		return 0;
	return 1;
}

struct connections connectionsappend(struct connections a, struct connections b) { //combine two connectionses
	struct connections n;
	int i, j;
	for (i = 0; i < a.n; i++)
		n.c[i] = a.c[i];
	for (j = 0; j < b.n; j++)
		n.c[i++] = b.c[j];
	n.n = a.n + b.n;
}

struct connections connected(struct point p) { //gives the statements directly and indirectly connected to the point
	int i; //statement index
	struct connections c = getconnections(p); //getconnections of children first
	for (i = 0; i < Stmtnum; i++) //iterate through statements
		if (near(p, *(Stmtlist[j].p), 72))
			for (k = 0; k < Stmtlist[j].g->numinputs; k++) {
				if (near(Arrowlist[i].b, pointadd(Stmtlist[j].g->i[k], Stmtlist[j].p), 6)) {
					c[l].o = Stmtlist.f; //set the connection to the refferred function.
					c[l++].n = k;
				}
			}
	return c;
}

struct connections getconnections(struct point p) {
	struct connections c;
	for (c.n = 0; c.n < Arrownum; c.n++)
		if (near(p, Arrowlist[c.n].a, 6))
			c = connectionsappend(c[c.n], connected(Arrowlist[c.n].b));
	c.n++; //c.n (length) is one greater than the last element's index.
	return c;
}

void makeFunclist() { //Turn Gatelist, Stmtlist and Arrowlist into Funclist
	int i, j; //function index, output index
	for (i = 0; i < Stmtnum; i++) { //iterate through statements
		struct statement stmt = Stmtlist[i];
		Funclist[i] = *(stmt.f);
		for (j = 0; j < stmt.g->numoutputs; j++) { //iterate through outputs
			Funclist[i].c[j] = getconnections(pointadd(stmt.p, stmt.g->o[j]));
		}
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
