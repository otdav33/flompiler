#include<expat.h>
#include<string.h>

//Nodes are points for functions' inputs or outputs
#define MAXCHAR 1000000
#define NUMOUTPUTS 4 //max output nodes per function
#define LENOUTPUTS 16 //max connected functions per node
#define FUNCTYPES 48 //max number of function-defining <g>s in <defs>
#define FUNCNAMELEN 32 //length of name string for above
#define MAXFUNCS 1000 //number of used functions
#define NODEDIST 6 //maximum distance between arrow end and node 

struct point { //coordinate
	double x, y;
};

struct defunc { //implemented function or used function with points (defined function)
	char name[FUNCNAMELEN], numi, numo; //builtin name, length of below arrays
	struct point i[NUMOUTPUTS], o[NUMOUTPUTS]; //coordinates of inputs and outputs, null terminated
};

struct tfunc { //used function with relation to other functions instead of points (tree function)
	char name[FUNCNAMELEN], numi, numo; //builtin name
	struct tfunc *o[NUMOUTPUTS][LENOUTPUTS]; //outputs, [Output #][Connection #]
	int n[NUMOUTPUTS][LENOUTPUTS]; //input number for connected function
};

struct data {
	struct defunc DFL[MAXFUNCS], UFL[FUNCTYPES]; //list of all functions available, then used
	struct point APL[2 * MAXFUNCS * LENOUTPUTS]; //organized as tail, tip, tail, tip, etc. Each arrow has 2 points.
	int DFC, UFC, APC; //length of DFL, UFL, APL
	int Depth, Defs, Group; //current, defs tag, and g tag XML tree depth
};

//extract.c
void start(void *data, const XML_Char *name, const XML_Char **attr);
void end(void *data, const XML_Char *name);
//connect.c
int connect(struct tfunc *tfuncs, struct data *d);
