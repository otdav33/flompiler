#include<stdio.h>
#include<string.h>
#include<expat.h>

#define MAXCHAR 1000000

void start(void *data, const XML_Char *name, const XML_Char **attr) {
	int i;
	printf("elem");
	for (i = 0; i < *(int *)data; i++)
		printf("  ");
	printf("%s", name);
	for (i = 0; attr[i]; i += 2)
		printf(" %s='%s'", attr[i], attr[i + 1]);
	printf("\n");
	(*(int *)data)++;
}

void text(void *data, const XML_Char *s, int len) {
	if (len < 2)
		return;
	int i;
	printf("text");
	for (i = 0; i < *(int *)data; i++)
		printf("  ");
	char temp[len + 1];
	strncpy(temp, s, len);
	temp[len] = '\0';
	printf("%s\n", temp);
}

void end(void *data, const XML_Char *name) {
	(*(int *)data)--;
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
	int Depth;
	XML_SetUserData(parser, &Depth);
	size_t len = fread(str, sizeof(XML_Char), MAXCHAR, file);
	if (!XML_Parse(parser, str, len, 1)) {
		printf("expat error '%s'\n", XML_ErrorString(XML_GetErrorCode(parser)));
		return 1;
	}
	XML_ParserFree(parser);
	return 0;
}
