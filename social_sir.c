#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 10000 /* Pop storrelse */
#define F 5000  /* anat familier */
#define C 2 /* antal sociale contexter */

int social[N][C];


void initialize_social();


int main(){
	int pop, s, i, r;
	double t, v, b, tot, rd;
	pop = 1000000;
	i = 5;
	s = pop - i;
	r = 0;
	t = 0.0;
	
	v = 1.0;
	b = 2.0/(s + i + r);
	
	while (i > 1){
		/* find time to event */
		rd=(double)random()/RAND_MAX;
		tot = v*i + b*s*i;
		t += - log(rd)/tot;
		/* find event type */
		rd=(double)random()/RAND_MAX;
		if (rd < v*i/tot)
			{i--; r++;}
		else
			{s--; i++;}
		printf("%f  %d\n",t,i);
	}
	return 0;
}


// new_decol_day=(int)floor((double)random()/RAND_MAX*community_days);


