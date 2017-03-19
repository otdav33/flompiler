#include"flompiler.h"
#include<stdio.h>
#include<string.h>

const XML_Char *val4key(const XML_Char **attr, const XML_Char *key) { //Gives the b in a="b" for (array, a)
	int i;
	for (i = 0; attr[i]; i += 2)
		if (!strcmp(attr[i], key))
			return attr[i + 1];
	return 0;
}

struct point pointadd(struct point a, struct point b) {
	struct point p = {a.x + b.x, a.y + b.y};
	return p;
}

void usefunc(struct defunc *b, struct point p, struct defunc *a) { //copies a into b with offset p
	strncpy(b->name, a->name, FUNCNAMELEN);
	b->numi = a->numi;
	b->numo = a->numo;
	int i;
	for (i = 0; i < a->numi; i++)
		b->i[i] = pointadd(a->i[i], p);
	for (i = 0; i < a->numo; i++)
		b->o[i] = pointadd(a->o[i], p);
}

int isflt(const XML_Char c) { //will return nonzero only if the character passed could be part of a floating point
	const XML_Char *flts = "+-.0123456789";
	int i;
	for (i = 0; flts[i]; i++)
		if (flts[i] == c) {
			return 1;
		}
	return 0;
}

void f2l2(struct point *r, const XML_Char *s) { //processes a path's d value
	XML_Char buf[strlen(s)/4]; //general buffer
	int i = 0, j = 0; //index of s, buf
	//printf("string is %s\n", s);
	while (!isflt(s[i])) {
		i++; //skip to the first number
		if (i == strlen(s)) {
			printf("path had no numbers\n");
			exit(1);
		}
	}
	while (isflt(s[i])) {
		//printf("looped %i, buflen %i, bufindex %i, sindex %i, slen %i\n", i, strlen(buf), j, i, strlen(s));
		buf[j++] = s[i++]; //record first number
	}
	buf[j] = '\0';
	r[0].x = atof(buf); //store first number
	j = 0;
	while (!isflt(s[i])) i++;
	while (isflt(s[i]))
		buf[j++] = s[i++];
	buf[j] = '\0';
	r[0].y = atof(buf); //#2
	i = strlen(s) - 1; //go to end
	while (!isflt(s[i])) i--;
	while (isflt(s[i])) i--;
	while (!isflt(s[i])) i--;
	while (isflt(s[i])) i--; //go back two numbers
	//now we're before the last two numbers
	i++;
	//now we're at the last two numbers
	j = 0; //reset buffer
	while (isflt(s[i]))
		buf[j++] = s[i++];
	buf[j] = '\0'; //copy
	r[1].x = atof(buf); //store
	j = 0;
	while (!isflt(s[i])) i++;
	while (isflt(s[i]))
		buf[j++] = s[i++];
	buf[j] = '\0';
	r[1].y = atof(buf); //last one
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
				for (i = 0; i < FUNCTYPES && strcmp(d->DFL[i].name, name); i++); //advances to the referred function
				//copy points
				d->DFL[d->DFC].numi = d->DFL[i].numi;
				for (j = 0; j < d->DFL[i].numi; j++)
					d->DFL[d->DFC].i[j] = d->DFL[i].i[j];
				d->DFL[d->DFC].numo = d->DFL[i].numo;
				for (j = 0; j < d->DFL[i].numo; j++)
					d->DFL[d->DFC].o[j] = d->DFL[i].o[j];
			}//End broken hack
		}
	} else {
		if (!strcmp(name, "defs")) { //if tag name is "defs", and we aren't in defs
			d->Defs = d->Depth; //make it known that we are in a defs tag.
		}
		if (!strcmp(name, "use")) {
			const XML_Char *name = val4key(attr, "xlink:href") + 1;
			const XML_Char *x = val4key(attr, "x");
			const XML_Char *y = val4key(attr, "y");
			int i;
			for (i = 0; i < FUNCTYPES && strcmp(d->DFL[i].name, name); i++); //advances to the referred function
			if (i == FUNCTYPES) {
				printf("Could not find function \"%s\".\n", name);
				exit(1);
			}
			struct point offset = {atof(x), atof(y)};
			usefunc(d->UFL + d->UFC, offset, d->DFL + i);
			d->UFC++;
		}
		if (!strcmp(name, "path")) {
			const XML_Char *s = val4key(attr, "d");
			struct point p[2];
			f2l2(p, s);
			d->APL[d->APC++] = p[0];
			d->APL[d->APC++] = p[1]; //process and store arrow
		}
	}
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
