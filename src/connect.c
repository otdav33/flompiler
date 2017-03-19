#include"flompiler.h"
#include<stdio.h>
#include<string.h>

#define squared(d) ((d) * (d)) //parenthesis around d's are necessary because of order of operations and the fact that the precompiler is literal about everything.

int near(struct point a, struct point b, double rad) {
	if (squared(a.x - b.x) + squared(a.y - b.y) < squared(rad)) {
		printf("square of %f is %f + %f is %f = %f < %f is %f\n", a.x - b.x, squared(a.x - b.x), a.y - b.y, squared(a.y - b.y), squared(a.x - b.x) + squared(a.y - b.y), rad, squared(rad)); //for debugging TODO: remove
		return 1;
	}
	return 0;
}

int pointlink(struct data *d, struct tfunc *list, int n, int o, int j, struct point l) {
	//d is the data struct, list is an array of all tfuncs, with index correlating to UFL in *d. n is the index of tfuncs, o the current output, and j the current index for the output.
	//returns j
	int i, k; //index of a list, input #
	for (i = 0; i < d->UFC; i++)
		for (k = 0; k < d->UFL[i].numi; k++)
			if (near(d->UFL[i].i[k], l, NODEDIST)) {
				printf("%i, o%i near %i, i%i\n", n, o, i, k); //for debugging TODO: remove
				list[n].o[o][j] = list + i;
				list[n].n[o][j++] = k;
				if (j == LENOUTPUTS) {
					printf("Too many nodes connected to one output (funcs) %i, %i, %i.\n", j, i, k);
					exit(1);
					//fix with dynamic memory allocation
				}
			}
	for (i = 0; i < d->APC; i += 2)
		if (near(d->APL[i], l, NODEDIST)) {
			j = pointlink(d, list, n, o, j, d->APL[i + 1]); //follow the arrow
			if (j == LENOUTPUTS) {
				printf("Too many nodes connected to one output (arrows) %i, %i.\n", j, i);
				exit(1);
				//fix with dynamic memory allocation
			}
		}
	return j;
}

int connect(struct tfunc *tfuncs, struct data *d) { //will relate information from d into tfuncs. returns length
	int i, j;
	for (i = 0; i < d->UFC; i++) {
		strcpy(tfuncs[i].name, d->UFL[i].name);
		tfuncs[i].numo = d->UFL[i].numo;
		for (j = 0; j < tfuncs[i].numo; j++)
			pointlink(d, tfuncs, i, j, 0, d->UFL[i].o[j]);
	}
	return j;
	//TODO list
}
