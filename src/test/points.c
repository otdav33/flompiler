#include<stdio.h>
#include<string.h>
#include<expat.h>

//Nodes are points for functions' inputs or outputs
#define MAXCHAR 1000000
#define NUMOUTPUTS 4 //max output nodes per function
#define LENOUTPUTS 9 //max connected functions per node
#define FUNCTYPES 48 //max number of function-defining <g>s in <defs>
#define FUNCNAMELEN 32 //length of name string for above

struct point { //coordinate
	double x, y;
};

struct definedfunc { //implemented function
	char name[FUNCNAMELEN], numi, numo; //builtin name, length of below arrays
	struct point i[NUMOUTPUTS], o[NUMOUTPUTS]; //coordinates of inputs and outputs, null terminated
};

struct treefunc { //used function
	char name[FUNCNAMELEN], numi, numo; //builtin name
	struct func *o[NUMOUTPUTS+1][LENOUTPUTS]; //outputs, null terminated [Output #][Connection #]
};

struct data {
	struct definedfunc DFL[FUNCTYPES]; //list of all gates
	//DFC is current length of d->DFL
	int DFC, Depth, Defs, Group; //current, defs tag, and g tag XML tree depth
};

const XML_Char *val4key(const XML_Char **attr, const XML_Char *key) { //Gives the b in a="b" for (array, a)
	int i;
	for (i = 0; attr[i]; i += 2)
		if (!strcmp(attr[i], key))
			return attr[i + 1];
	return 0;
}

//will run on the open of each XML tag. "Name" is the tag name (eg "g", "defs"). "attr" is the attributes, with evens keys and odds values.
void start(void *data, const XML_Char *name, const XML_Char **attr) {
	struct data *d = (struct data *)data;
	if (d->Defs) {
		if (!strcmp(name, "g")) { //if tag name is "g"
			const XML_Char *id = val4key(attr, "id");
			if (id && strcmp(id, "#node")) { //if there is an id and it's not "#node"
				d->Group = d->Depth;
				strncpy(d->DFL[d->DFC].name, id, FUNCNAMELEN);
			}
		}
		if (!strcmp(name, "use")) {
			const XML_Char *href = val4key(attr, "xlink:href");
			const XML_Char *x = val4key(attr, "x");
			const XML_Char *y = val4key(attr, "y");
			//add points
			if (!strcmp(href, "##node")) {
				//using a #node is declaring a node directly.
				//class will be in the form "i1" or "o3"
				//The first char will be input or output
				//The second is the number left-to-right starting at 1.
				const XML_Char *class = val4key(attr, "class");
				struct point p = {atof(x), atof(y)};
				//will put node into the list
				if (class[0] == 'i') {
					d->DFL[d->DFC].i[class[1]-'0'-1] = p;
					d->DFL[d->DFC].numi++;
				}
				if (class[0] == 'o') {
					d->DFL[d->DFC].o[class[1]-'0'-1] = p;
					d->DFL[d->DFC].numo++;
				}
			} else { //TODO eventually: fix the following hack.
				//declare nodes indirectly.
				//Instead of dealing with change in position or scaling, the following code copies exactly the nodes of the used item.
				const XML_Char *name = href + 1; //SVG referenced function
				int i, j;
				for (i = 0; i < FUNCTYPES && strcmp(d->DFL[i].name, name); i++); //advances to the referred gate
				printf("i is %i; name is %s\n", i, d->DFL[i].name);
				//copy points
				d->DFL[d->DFC].numi = d->DFL[i].numi;
				for (j = 0; j < d->DFL[i].numi; j++)
					d->DFL[d->DFC].i[j] = d->DFL[i].i[j];
				d->DFL[d->DFC].numo = d->DFL[i].numo;
				for (j = 0; j < d->DFL[i].numo; j++)
					d->DFL[d->DFC].o[j] = d->DFL[i].o[j];
			}//End broken hack
		}
	} else if (!strncmp(name, "defs", 4)) { //if tag name is "defs", and we aren't in defs
		d->Defs = d->Depth; //make it known that we are in a defs tag.
	}
	//printelem(name, attr); //useful for debugging
	d->Depth++; //we are descending, after all.
}

void end(void *data, const XML_Char *name) {
	struct data *d = (struct data *)data;
	d->Depth--;
	if (d->Defs == d->Depth)
		d->Defs = 0; //Leave a defs
	if (d->Group == d->Depth) {
		d->Group = 0; //Leave a g
		d->DFC ++; //if g, is next DF
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
	int i, j;
	for (i = 0; i < FUNCTYPES && d.DFL[i].name[0]; i++) {
		printf("Func %s; %i, %i, i:", d.DFL[i].name, d.DFL[i].numi, d.DFL[i].numo);
		for (j = 0; j < d.DFL[i].numi; j++)
			printf(" (%f,%f)", d.DFL[i].i[j].x, d.DFL[i].i[j].y);
		printf("; o:");
		for (j = 0; j < d.DFL[i].numo; j++)
			printf(" (%f,%f)", d.DFL[i].o[j].x, d.DFL[i].o[j].y);
		putchar('\n');
	}
	return 0;
}
