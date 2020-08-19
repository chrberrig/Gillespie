#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 10000 /* Pop storrelse */
#define F 5000  /* antal familier */
#define C 2 /* antal sociale contexter */

// int social[N][C]; //what is this? matrix?


/*
struct individual {
	int id;
	char state;
	int work;
	int family;
}

int fam_type(int num_fam_types) {
	int fam_type = floor(log(randint(1, (int)exp(num_fam_types))))
	return fam_type
}

int len_arr(int arr[]) {
	size_t n = sizeof(arr) / sizeof(arr[0]);
	return (int) n;
}
*/

/*
int get_index(int target_val, int arr[]) {
	int k;
	for (k=0; k<sizeof(arr); k++) {
		if (arr[k] == target_val) {
			return k;
		}
	}
	return -1;
}

int * get_next_sep(int target_index, int arr_sep[]) {
	int k = 0;
	if (target_index >= arr_sep[sizeof(arr_sep)]) {
		int ret_arr[] = {-1, -1};
		return ret_arr;
	}
	while (arr_sep[k]<target_index) {	
		k++;
	}
	int ret_arr[] = {k, arr_sep[k]};
	return ret_arr;
}

int * get_next_social_index(int target_index, int arr_sep[]) {
	int k = 0;
	if (target_index >= arr_sep[sizeof(arr_sep)]) {
		int ret_arr[] = {-1, -1};
		return ret_arr;
	}
	while (arr_sep[3*k]<target_index) {	
		k++;
	}
	int ret_arr[] = {3*k, arr_sep[3*k]};
	return ret_arr;
}

void shift_to_last(int index_move, int index_last, int arr[]){
	int k;
	int temp = arr[index_move];
	for (k=index_move; k<index_last;k++) {
		arr[k] = arr[k+1];
	}
	arr[index_last] = temp;
}

void update_compartments(int index_move, int arr[], int arr_sep[]) {
	int * index_last = get_next_sep(index_move, arr_sep);
	shift_to_last(index_move, index_last[1] - 1, arr);
	arr_sep[index_last[0]]--;
}
*/


int rand_int(int low, int high) {
	/* Return random integer in range (inclusive) low .. high - 1 */
	double temp = (int) floor((double)((high - low) * rand() / RAND_MAX));
	return temp;
}


int main(){
	int pop; 
	int num_w;
	// int s, i, r;
	// double t, v, bf, bw, bi, tot, rd;
	pop = 30; //currently causing segfault at 1000000. 1e5 is ok.
	num_w = pop/10;
	/*
	i = 5;
	s = pop - i;
	r = 0;
	t = 0.0;
	
	v = 1.0;
	//bf = 2.0/(s + i + r);
	bw = 2.0/(s + i + r);
	//bi = 2.0/(s + i + r);
	*/

	int j, k;
	int contexts_num[] = {9,4,5,8,3};
	size_t contexts_num_length = sizeof(contexts_num) / sizeof(contexts_num[0]);
	printf("len : %d\n", contexts_num_length); 

	//int num_con = contexts_num_length; //const?
	///* struct individuals[];*/
	//int individuals[pop][num_con]; // static?
	//for (k=0; k<pop; k++) {
	//	/*generate the proper ids for social contexts*/
	//	int work_id = rand_int(0, contexts_num[0]);
	//	printf("%d\n", work_id);
	//	int context[] = {work_id};
	//	for (j=0; j< num_con; j++) {
	//		// printf("j : %d\n", j); 
	//		printf("con[j] : %d\n", context[j]); 
	//		individuals[k][j] = context[j]; 
	//		printf("j:%d, k:%d, ind[j][k]:%d\n", j, k, individuals[k][j]);
	//	}
	//}
	

	/*initialize arrays of the different workplaces and families
	and their associated quantities*/
	/*
	int comp[pop], comp_sep[pop];
	int work[pop], work_sep[3*num_w];
	*/
	
	/*for (j=0; j<len_arr(contexts_num); j++) {
		printf("j%d\n", j); 
		printf("con%d\n", contexts_num[j]); 
	}*/

	//int *individuals = init_individuals(pop, contexts);
	/*
	for (j=0; j<pop; j++){
		printf("Individual %d is at workplace %d", j, individuals[j]);
	}
	*/

	/*
	for (j=0; j<3*num_w; j++) {
		work_sep[3*j] = j*pop/num_w;
		if (j % 3 == 0) {
			printf("work_sep[%d] = %d\n", j, work_sep[j]);
		}
	}
	printf("j after loop is %d\n",j );
	int k = 500;
	printf("next workplace from %d starts at %d\n", k, get_next_social_index(k, work_sep));
	*/
	

	return 0;
}
































	/*
	while (i > 1){
		// find time to event 
		rd=(double)random()/RAND_MAX;
		tot = v*i + b*s*i;
		t += - log(rd)/tot;
		// find event type 
		rd=(double)random()/RAND_MAX;
		if (rd < v*i/tot)
			{i--; r++;}
		else
			{s--; i++;}
		printf("%f  %d\n",t,i);
	}*/

// new_decol_day=(int)floor((double)random()/RAND_MAX*community_days);


//int *init_individuals(int pop_size, int context_num[]) {
//	/*num_context is an array, give sthe number of compartments of given social context
//		eg: if there are 100 workplaces, the num_contexts entry for work is 100
//	currently, the ordering is {num_work, num_fam}
//	maybe one should use array of struct instead of 2d array?*/
//	int j,k;
//	int num_con = len_arr(context_num); //const?
//	/* struct individuals[];*/
//	int individuals[pop_size][num_con]; // static?
//	for (k=0; k<pop_size; k++) {
//		/*generate the proper ids for social contexts*/
//		int work_id = rand_int(0, context_num[0]);
//		int context[] = {work_id};
//		for (j=0; j< num_con; j++) {
//			// printf("j : %d\n", j); 
//			// printf("conj : %d\n", context[j]); 
//			individuals[k][j] = context[j]; 
//		}
//	}
//	return individuals;
//}
