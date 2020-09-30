#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define N 4000 /* Pop storrelse */
#define C 3 /* Number of social contexts */
#define T 2 /* Process types (infection, recovery) */

static int global_counter;
static double beta[C][N];
static double gam[C];
static int contexts_num[C]; 
static int contexts_arr[C][N];
static int contexts_sep[C][3*N];
static int contexts_sir[C][3][N]; // sir-Compartments - social contexts
static double contexts_sums[C][T]; 
//static double part_sums[T][N]; 
static double type_sums[T]; 
static int individuals[N][C];

/* Array function */
// Shuffle
void val_switch(int *x, int *y) {
	/* Switches values of two array entries:
		Vars:	int *x, *y indecies of array entries to switch. 
		Return: void	*/
	int temp = *x;
	*x = *y;
	*y = temp;
}

void arr_shuffle(int arr[], int len ) {
	/* Shuffles entries of int array:
		Vars:	int arr[], array to shuffle;	
			int len, length of array.
		Return: void	*/
	// srand(time(NULL));
	for (int i = len-1; i > 0; i--) {
		int j = rand() % (i+1);
		val_switch(&arr[i], &arr[j]);
	}
}

//Display
void arr_display(int arr[], int len) {
	/* Displays entries of int array in terminal:
		Vars:	int arr[], array to display;	
			int len, length of array.
		Return: void	*/
	//printf("The elements in the array are:  \n");    
	for (int i = 0; i < len; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void d_arr_display(double arr[], int len) {
	/* Displays entries of int array in terminal:
		Vars:	int arr[],	array to display;	
			int len,	length of array.
		Return: void	*/
	//printf("The elements in the array are:  \n");    
	for (int i = 0; i < len; i++) {
		printf("%g ", arr[i]);
	}
	printf("\n");
}

//Sum
int arr_sum(int arr[], int len) {
	/* Sums entries of int array:
		Vars:	int arr[],	array to sum;	
			int len,	length of array to sum.
		Return: int sum,	sum of array entries.	*/
	int sum, loop;
	sum = 0;
	for(loop = len-1; loop >= 0; loop--) {
		sum = sum + arr[loop];      
	}	
	return sum;
}

double d_arr_sum(double arr[], int len) {
	/* Sums entries of double array:
		Vars:	double arr[],	array to sum;	
			int len,	length of array to sum.
		Return: double sum,	sum of array entries.	*/
	int loop;
	double sum = 0;
	for(loop = len-1; loop >= 0; loop--) {
		// printf("%d\n", loop);
		sum = sum + arr[loop];      
		// printf("%g\n", sum);
	}	
	return sum;
}

//Index
int get_arr_index(int id, int context_id) {
	/* get index of first entry in int array, that matches the target_val:
		Vars:	int target_val,	integer value to search for in list;	
			int arr[],	array to search;
			int len,	length of array to search;
		Return: int sum, 	index of first entry match to target_val.	*/
	int k;
	int k_start = 0;
	if (context_id == 0){
		//k_start = relu_int(id - (N - contexts_sep[0][1])); // Works!!! Do not delete !!!
		k_start = individuals[id][context_id];
	}
	else{
		int k_init = 3*individuals[id][context_id];
		k_start = contexts_sep[context_id][k_init]; 
	}
	for (k=k_start; k<N; k++) {
		if (contexts_arr[context_id][k] == id) {
			return k;
		}
	}
	arr_display(contexts_sep[context_id], 3*contexts_num[context_id]);
	arr_display(contexts_arr[context_id], N);
	printf("Error; get_arr_index: element w. value %d, dont exist in the array\n", id);
	exit(1);
	return -1;
}

int rand_int(int low, int high) {
	/* Return random integer in range (inclusive) low .. high - 1 */
	if (low == high){
		printf("Invalid interval for generating random integer [%d, %d]\n", low, high-1);
		exit(1);
	}
	return (int) low + floor((float) (high - low) * rand()/RAND_MAX);
}

/* Sep Functions*/

void sep_display(int context_id){
	int c,s;
	s = 0;
	for (c = 0; c < N; c++) {
		printf("%d ", contexts_arr[context_id][c]);
		while (contexts_sep[context_id][s] == c && s < 3*contexts_num[context_id]) {
			printf("<- %d ", s);
			s++;
		}
		printf("\n");
	}
}

int *get_sep(int id, int context_id) {
	int k;
	static int ret_arr[2];
	clock_t start_t, cond_t, get_index_t, loop_t;
	start_t = clock();
	if (context_id == 0){
		k=0;
	}
	else {
		k = 3*individuals[id][context_id];
	}
	cond_t = clock() - start_t;
	//printf("call get_arr_index from get_sep!\n");
	int first_index = get_arr_index(id, context_id);
	get_index_t = clock() - start_t;
	if (contexts_sep[context_id][k]>first_index) {
		printf("Error: get_sep_index; Initial value allready exeeded target!\n");
		ret_arr[0] = -1;
		ret_arr[1] = -1;
		return ret_arr;
	}
	while (contexts_sep[context_id][k]<=first_index) {
		k++;
	}
	loop_t = clock() - start_t;
	//printf("cond_t: %ld,\tget_index: %ld,\tloop:%ld\n", cond_t, get_index_t, loop_t);
	ret_arr[0] = k-1;
	ret_arr[1] = contexts_sep[context_id][k-1];
	return ret_arr;
}

int *index_get_sep(int target_index, int context_id) {
	int k;
	static int ret_arr[2];
	int id = contexts_arr[context_id][target_index];
	if (context_id == 0){
		k = 0;
	}
	else {
		k = 3*individuals[id][context_id];
	}
	if (target_index >= contexts_sep[context_id][3*contexts_num[context_id]-1]) {
		printf("Error: get_next_sep; Target index larger than index of last separator element\n");
		ret_arr[0] = -1;
		ret_arr[1] = -1;
		return ret_arr;
	}
	while (contexts_sep[context_id][k]<=target_index) {	
		k++;
	}
	ret_arr[0] = k-1; 
	ret_arr[1] = contexts_sep[context_id][k-1];
	return ret_arr;
}

/* sir functions */

void context_state_display(int context_index) {
	/*Displays SIR state of context:
		vars:	int context_index,	index of the context to display;
		Return:	void	*/
	int j, S, I, R;
	for (j=0; j<contexts_num[context_index]; j++) { 
		printf("----------------\n");
		S = contexts_sep[context_index][3*j];
		I = contexts_sep[context_index][3*j+1];
		R = contexts_sep[context_index][3*j+2];
		if (j<contexts_num[context_index]-1) {
			printf("S:%d,\tI:%d,\tR:%d\n", I-S, R-I, contexts_sep[context_index][3*(j+1)]-R);
		}
		else if (j==contexts_num[context_index]-1) {
			printf("S:%d,\tI:%d,\tR:%d\n", I-S, R-I, N-R);
		}
		/* Alternative implementation */
		printf("S2:%d,\tI2:%d,\tR2:%d\n", contexts_sir[context_index][0][j], 
						contexts_sir[context_index][1][j], 
						contexts_sir[context_index][2][j]);
	}
	printf("----------------\n");
}


/* Updates etc. */

void update_compartment(int id, int j) { 
	/*
	Update compartment wrt individual and social context:
		vars:	int id: 	id of individual;
			int j: 		social context
		return:	void
	*/
	FILE *update_compartment_log;
	char update_compartment_filename[50]; 
	sprintf(update_compartment_filename, "logs/update_compartment_log_%d.csv", N);
	//printf("update_compartment_log_file: %s\n", update_compartment_filename);
	update_compartment_log = fopen( update_compartment_filename, "w+" );
	clock_t start_t, move_t, sep_t, shifttolast_t, total_t; 

	/* Update array and seperator */
	start_t = clock();
	int index_move;
	index_move = get_arr_index(id, j);
	move_t = clock()-start_t;
		
	//printf("index_sep_type:\n");
	int *sep = get_sep(id, j);
	int index_sep = sep[0];
	int index_next_sep = sep[0] + 1;
	int index_last = contexts_sep[j][index_next_sep];
	//val_sep = sep[1];
	//printf("sep_index: %d\tsep_id: %d\n", sep[0], sep[1]);
	sep_t = clock()-start_t;

	// maybe just switch with last instead of permutate?
	int last_id = contexts_arr[j][index_last-1];
	val_switch(&contexts_arr[j][index_move], &contexts_arr[j][index_last-1]);
	if (j == 0){
		individuals[id][0] = index_last-1;
		individuals[last_id][0] = index_move;
	}
	//shift_to_last(index_move, index_last - 1, contexts_arr[j]); // <-- works!!! do not delete!!! Also correct in get_arr_index!!!
	shifttolast_t = clock()-start_t;
	
	contexts_sep[j][index_next_sep]--;
	/* checks! */
	/*
	if (contexts_sep[j][index_next_sep]<contexts_sep[j][index_next_sep-1]){
		printf("Error: crossing backwards contexts_sep[%d][%d]: %d\n", j, index_next_sep, contexts_sep[j][index_next_sep]);
		exit(1);
	}
	if (contexts_sep[j][index_next_sep]<0){
		printf("Error: contexts_sep[%d][%d]: %d\n", j, index_next_sep, contexts_sep[j][index_next_sep]);
		exit(1);
	}
	*/

	/* Update sir and partial sums */
	int type = index_sep%3;
	int type_num = index_sep/3;
	//printf("type_num:%d\n", type_num);
	//printf("contexts_num:%d\n", contexts_num[j]);
	//double subtract;
	//double add;
	//if (type==0){
	contexts_sums[j][0] -= beta[j][type_num]*contexts_sir[j][0][type_num]*contexts_sir[j][1][type_num];
	contexts_sums[j][1] -= gam[j]*contexts_sir[j][1][type_num];
	contexts_sir[j][type][type_num]--;
	contexts_sir[j][type + 1][type_num]++;
	contexts_sums[j][0] += beta[j][type_num]*contexts_sir[j][0][type_num]*contexts_sir[j][1][type_num];
	contexts_sums[j][1] += gam[j]*contexts_sir[j][1][type_num];
	//}
	//else if (type==1){
	//	contexts_sums[j][type] -= gam[j]*contexts_sir[j][type][type_num];
	//	contexts_sir[j][type][type_num]--;
	//	contexts_sir[j][type + 1][type_num]++;
	//	contexts_sums[j][type] += gam[j]*contexts_sir[j][type][type_num];
	//}
	//contexts_sir[j][type][type_num]--;
	//contexts_sir[j][type + 1][type_num]++;

	double t_sum = 0;	
	for (j=0; j<C; j++) {
		t_sum += contexts_sums[j][0];
	}
	//t_sums_update = clock() - start_t;
	type_sums[0] = t_sum;
	type_sums[1] = contexts_sums[0][1];
	
	//if (type==0){
	//	part_sums[type][type_num] = beta[j][type_num]*contexts_sir[j][type][type_num]*contexts_sir[j][type+1][type_num];
	//}
	//else if (type==1){
	//	part_sums[type][type_num] = gam[j]*contexts_sir[j][type][type_num];
	//	//printf("%g\n", part_sums[type][type_num]);
	//}
	//printf("updated_part_sum[%d][%d] = %g\n", type, type_num, part_sums[type][type_num]);
	total_t = clock()-start_t;
	printf("%ld,\t%ld,\t%ld,\t%ld\n", move_t, sep_t, shifttolast_t, total_t); 
	fprintf(update_compartment_log, "%ld, %ld, %ld, %ld\n", move_t, sep_t, shifttolast_t, total_t); 
	fclose(update_compartment_log);
}

void update_all_compartments(int id) {
	int j = 0;
	//clock_t start_t, intermediate_t, total_t; 
	//start_t = clock();
	//update_compartment(id, 0); 
	//intermediate_t = clock() - start_t;
	//printf("%ld\n", intermediate_t);
	//update_compartment(id, 1); 
	//intermediate_t = clock() - start_t;
	//printf("%ld\n", intermediate_t);
	//update_compartment(id, 2); 
	//intermediate_t = clock() - start_t;
	//printf("%ld\n", intermediate_t);
	for (j=0; j<C; j++) { 
		update_compartment(id, j); 
		//printf("compartment updated\n");
		//arr_display(contexts_sep[j], 3*contexts_num[j]);
		//sep_display(j);
		//sep_display(contexts_arr[j], N, contexts_sep[j], 3*contexts_num[j]); // old
	//	intermediate_t = clock() - start_t;
	//	printf("%ld\n", total_t);
	}
	//total_t = clock() - start_t;
	//printf("%ld\n", total_t);
}

//void update_sum(int context_id) {
//	int j, k; 
//	double type0_sum=0, type1_sum=0;
//	double t_sum = 0;	
//	clock_t start_t, type_sums_iteration_new, type_sums_iteration_old, contexts_sums_update, t_sums_update, type_sums_update; 
//	start_t = clock();
//	//type0_sum = d_arr_sum(part_sums[0], contexts_num[context_id]);
//	//type1_sum = d_arr_sum(part_sums[1], contexts_num[context_id]);
//	//printf("new_type0_sum: %g\n", type0_sum);
//	//printf("new_type1_sum: %g\n", type1_sum);
//	//type_sums_iteration_new = clock() - start_t;
//	//type0_sum=0, 
//	//type1_sum=0;
//	//for (k=0; k<contexts_num[context_id]; k++){
//	//	type0_sum = type0_sum + beta[context_id][k]*contexts_sir[context_id][0][k]*contexts_sir[context_id][1][k];
//	//	type1_sum = type1_sum + gam[context_id]*contexts_sir[context_id][1][k];
//	//}
//	//printf("old_type0_sum: %g\n", type0_sum);
//	//printf("old_type1_sum: %g\n", type1_sum);
//	//type_sums_iteration_old = clock() - start_t;
//	//contexts_sums[context_id][0] = type0_sum; 
//	//contexts_sums[context_id][1] = type1_sum;
//	//contexts_sums[context_id][0] = d_arr_sum(part_sums[0], contexts_num[context_id]);
//	//contexts_sums[context_id][1] = d_arr_sum(part_sums[1], contexts_num[context_id]);
//	//type_sums_iteration = clock() - start_t;
//	//contexts_sums_update = clock() - start_t;
//
//	//for (j=0; j<C; j++) {
//	//	t_sum += contexts_sums[j][0];
//	//}
//	//t_sums_update = clock() - start_t;
//	//type_sums[0] = t_sum;
//	//type_sums[1] = contexts_sums[0][1];
//	//type_sums_update = clock() - start_t;
//
//	//printf("type_sums_iterations, contexts_sums_update, t_sums_update, type_sums_update");
//	//printf("%ld,\t%ld,\t%ld,\t%ld,\t%ld\n", type_sums_iteration_new, type_sums_iteration_old, contexts_sums_update, t_sums_update, type_sums_update);
//	printf("%ld,\t%ld,\t\n", t_sums_update, type_sums_update);
//}

/* Events */
double choose_event_time() {
	double rd, tau, dt;
	rd = (double)random()/RAND_MAX;
	tau = d_arr_sum(type_sums, T);
	dt = -log(rd)/tau;
	return dt;
}

//int choose_event(){
//	double rd, cumul_t_sum, cumul_c_sum, normaliz;
//	rd = (double)random()/RAND_MAX;
//	cumul_t_sum = 0;
//	cumul_c_sum = 0;
//	normaliz = d_arr_sum(type_sums, T);
//	for (k=0; k<T; k++) {
//		cumul_t_sum = cumul_t_sum + type_sums[k];
//		if (rd<cumul_t_sum/normaliz) {
//			return k;
//		}
//	}
//}
	
int choose_event_type() {
	double rd, cumul_t_sum, normaliz;
	rd = (double)random()/RAND_MAX;
	cumul_t_sum = 0;
	normaliz = d_arr_sum(type_sums, T);
	int k;	
	for (k=0; k<T; k++) {
		cumul_t_sum += type_sums[k];
		if (rd<cumul_t_sum/normaliz) {
			return k;
		}
	}
	printf("Error: Event type not chosen!\n");
	return -1;
	exit(1);
}

int choose_event_context(int type_id) {
	int j;	
	double rd, cumul_c_sum;
	rd = (double)random()/RAND_MAX;
	cumul_c_sum = 0;
	for (j=0; j<C; j++) {
		cumul_c_sum += contexts_sums[j][type_id];
		if (rd<cumul_c_sum/type_sums[type_id]) {
			return j;
		}
	}
	printf("Error: Event context not chosen!\n");
	return -1;
	exit(1);
}

int *choose_event_individual(int type){
	static int ret_arr[2]; 
	int rint = rand_int(contexts_sep[0][type], contexts_sep[0][type+1]);
	//if (type==0){
	//	int rint = rand_int(contexts_sep[0][type], contexts_sep[0][type+1]);
	//}
	//else if (type==1){
	//	int rint = rand_int(contexts_sep[0][type], contexts_sep[0][type+1]);
	//}
	ret_arr[0] = rint;
	ret_arr[1] =  contexts_arr[0][rint];
	return ret_arr;
}

void gen_context_ids(int context_id, int con_size, int con_list[]) {
	int k, sep_div3;
	sep_div3 = contexts_num[context_id];
	if (con_size < N - global_counter) {
		for (k=global_counter; k<global_counter+con_size; k++) {
			con_list[k] = contexts_num[context_id];
		}
		contexts_sep[context_id][3*sep_div3] = global_counter; //
		global_counter = global_counter + con_size;
		contexts_sep[context_id][3*sep_div3+1] = global_counter; //
		contexts_sep[context_id][3*sep_div3+2] = global_counter; //
		contexts_num[context_id]++;
	}
	else {
		con_size = N - global_counter;
		for (k=global_counter; k<global_counter+con_size; k++) {
			con_list[k] = contexts_num[context_id];
		}
		contexts_sep[context_id][3*sep_div3] = global_counter; //
		global_counter = global_counter + con_size;
		contexts_sep[context_id][3*sep_div3+1] = global_counter; //
		contexts_sep[context_id][3*sep_div3+2] = global_counter; //
		contexts_num[context_id]++;
	}
}

/* Init functions*/
void init_individuals(){
	/* initialize individials register array, contexts_arr and, contexts_sep */
	int k;

	int work_avg = 10;
	int fam_avg = 2;
	int copy_list[N];

	for (k=0; k<N; k++) {
		copy_list[k] = k;
	}	

	/* comp */
	contexts_num[0] = 1;
	for (k=0; k<N; k++) {
		individuals[k][0] = k; 
		contexts_arr[0][k] = k;
	}
	contexts_sep[0][0] = 0;
	contexts_sep[0][1] = N;
	contexts_sep[0][2] = N;

	/* work */
	global_counter = 0;
	int work_size = 0;
	int work_list[N];
	while (global_counter < N) {
		work_size = rand_int(1, 2*work_avg+1);
		gen_context_ids(1, work_size, work_list);
	}
	//arr_shuffle(work_list, N);
	arr_shuffle(copy_list, N);
	for (k=0; k<N; k++) {
		individuals[copy_list[k]][1] = work_list[k];
		contexts_arr[1][k] = copy_list[k];
	}	

	/* fam */
	global_counter = 0;
	int fam_size = 0;
	int fam_list[N];
	while (global_counter < N) {
		fam_size = rand_int(1, 2*fam_avg+1);
		gen_context_ids(2, fam_size, fam_list);
	}
	//arr_shuffle(fam_list, N);
	arr_shuffle(copy_list, N);
	for (k=0; k<N; k++) {
		individuals[copy_list[k]][2] = fam_list[k];
		contexts_arr[2][k] = copy_list[k];
	}	
}

void init_sir() {
	/* Initializes both sir and sums */
	int j, k;
	// printf("separators in sir init:\n");
	for (j=0; j<C; j++) { 
	// printf("new_context:\n");
		for (k=0; k<3*contexts_num[j]; k++) {
			if (k<3*(contexts_num[j]-1)){
				contexts_sir[j][k%3][k/3] = contexts_sep[j][k+1] - contexts_sep[j][k];
			} 
			else if (k<3*contexts_num[j]) { 
				contexts_sir[j][k%3][k/3] = N - contexts_sep[j][k];
			}
			//part_sums[0][k] = beta[j][k]*contexts_sir[j][0][k]*contexts_sir[j][1][k];
			//part_sums[1][k] = gam[j]*contexts_sir[j][1][k];
		}
		//contexts_sums[j][0] = d_arr_sum(part_sums[0], contexts_num[j]);
		//contexts_sums[j][1] = d_arr_sum(part_sums[1], contexts_num[j]);
		//printf("arr_display:\n");
		//d_arr_display(part_sums[0], contexts_num[j]);
		//printf("contexts_sums:\n");
		//printf("%g\n", contexts_sums[j][0]);
	}
// printf("SIR_init done!\n");
}


int main(){
	
	//double testarr[] = {1.0,2.0,3.0,4.0};
	//printf("%g\n", d_arr_sum(testarr,4));
	/* Iterables */
	int j, k;
	
	/* Variables */
	int i0 = 5;
	int rint;
	//double tot, rd;
	
	int type;
	int context;
	int id;
	int id_index;
	double dt;
	static double t;
	t = 0.0;
	
	double v, bf, bw, bc;
	v = 1.15;
	bc = 1.5/N;
	bf = 1.0;
	bw = 0.4/5;
	double b[C] = {bc, bw, bf};
	

	/* initializes arrays in accordence with funct. */
	init_individuals();
	printf("Individuals init done! \n");
	
	/* Init. constants for model */
	/* This must be below init_all_arr() */
	for (j=0; j<C; j++) {
		for (k=0; k<contexts_num[j]; k++) {
			beta[j][k] = b[j];
		}
		gam[j] = v;
	}
	printf("Constants init done! \n");
	

	init_sir();
	printf("SIR init done! \n");

	//for (k=0; k<N; k++) { 
	//	arr_display(individuals[k], C);
	//}

	printf("\n----------------\nInit_infection!\n----------------\n");

	/* initialize a few infected individuals */
	for (k=0; k<i0; k++) { 
		//check_compartments(0);
		rint = rand_int(contexts_sep[0][0], contexts_sep[0][1]);
		id = contexts_arr[0][rint];
		//printf("id: %d\n", id);
		update_all_compartments(id);
		//printf("All compartments updated!\n");
		//for (j=0; j<C; j++) {
		//	update_sum(j);
		//}
		printf("All sums updated!\n");
	}
	
	printf("\n----------------\nRunning Sim!\n----------------\n");
	
	FILE *sir_data;
	sir_data = fopen( "sir_data.csv", "w+" );

	FILE *time_log;
	char time_log_filename[50]; 
	sprintf(time_log_filename, "logs/time_log_%d.csv", N);
	//printf("time_log_file: %s\n", time_log_filename);
	time_log = fopen( time_log_filename, "w+" );
	clock_t start_t, stop_t, time_time, time_type, time_context, time_id, time_update_compartments, time_update_sums, time_print, time_total;

	fprintf(sir_data, "t,S,I,R\n");
	fprintf(sir_data, "%g,%d,%d,%d\n", t, contexts_sep[0][1]-contexts_sep[0][0] , contexts_sep[0][2]-contexts_sep[0][1], N-contexts_sep[0][2]);
	fprintf(time_log, "dt,event_time,type,context,id,update_compartments,update_sums,print,total\n");
	while (contexts_sep[0][2]-contexts_sep[0][1]>0) {
		start_t = clock();
		dt = choose_event_time();
		t += dt;
		time_time = clock() - start_t;
		type = choose_event_type();
		time_type = clock() - start_t;
		context = choose_event_context(type);
		time_context = clock() - start_t;
		int *id_ret = choose_event_individual(type);
		id_index = id_ret[0];
		id = id_ret[1];
		//printf("--------------------\n");
		////printf("id:\t%d\n", id);
		////printf("context:\t%d\n", context);
		//printf("type:\t%d\n", type);
		//printf("type_sums:\n");
		//d_arr_display(type_sums, T);
		//printf("Contexts_sums:\n");
		//for (j=0; j<C; j++) {
		//	d_arr_display(contexts_sums[j], T);
		//}
		//printf("--------------------\n");
		time_id = clock() - start_t;
		//printf("--------------------\n");
		//printf("dt:%g,\tt:%g,\ttype:%d,\tcon:%d,\tid:%d\n", dt, t, type, context, id);
		//printf("--------------------\n");

		update_all_compartments(id);
		time_update_compartments = clock() - start_t;

		//for (j=0; j<C; j++) {
		//	//context_state_display(j);
		//}
		time_update_sums = clock() - start_t;
		fprintf(sir_data, "%g,%d,%d,%d\n", t, contexts_sep[0][1]-contexts_sep[0][0] , contexts_sep[0][2]-contexts_sep[0][1], N-contexts_sep[0][2]);
		time_print = clock() - start_t;
		//fprintf(sir_data, "t:%g,\ttype:%d,\tcon:%d,\tid:%d\n", t, type, context, id);
		stop_t = clock();
		time_total = stop_t - start_t;
		printf("%g, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n", dt, time_time, time_type, time_context, time_id, time_update_compartments, time_update_sums, time_print, time_total);
		//fprintf(time_log, "%g, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n", dt, time_time, time_type, time_context, time_id, time_update_compartments, time_update_sums, time_print, time_total);
	}
	fclose(time_log);
	fclose(sir_data);
	printf("Simulation Done!\n");
	return 0;
}











































/* Graveyard */

//void sir_update_arr() {
//	int j, k;
//	for (j=0; j<C; j++) { 
//		for (k=0; k<3*contexts_num[j]; k++) {
//			//contexts_sir[j][k%3][k/3] = contexts_sep[j][k+1] - contexts_sep[j][k]
//			if (k<3*contexts_num[j]-1){
//				contexts_sir[j][k%3][k/3] = contexts_sep[j][k+1] - contexts_sep[j][k];
//			} 
//			else if (k==3*contexts_num[j]-1) {
//				contexts_sir[j][k%3][k/3] = N - contexts_sep[j][k];
//			}
//		}
//	}
//}

//void init_arr(int context_index) { // this is also very slow, though only an init-funct.
//	/* initialize arrays and separators of the different social contexts. etc. */
//	
//	int a, c, s, k;
//	c = 0;
//	s = 0;
//	for (a=0; a<contexts_num[context_index]; a++) {
//		contexts_sep[context_index][s] = c;
//		s++;
//		for (k=0; k<N; k++) {
//			if (individuals[k][context_index] == a) {
//				contexts_arr[context_index][c] = k;
//				c++;
//			}
//		}
//		if (a != contexts_num[context_index]-1) {
//			contexts_sep[context_index][s] = c;
//			s++;
//			contexts_sep[context_index][s] = c;
//			s++;
//		}
//	}
//	contexts_sep[context_index][s] = N;
//	s++;
//	contexts_sep[context_index][s] = N;
//	s++;
//}
//
//void init_all_arr() { 
//	int context_index;
//	for (context_index=0; context_index<C; context_index++) {
//		printf("Init array\n");
//		init_arr(context_index);
//		printf("Init array done!\n");
//	}
//}
//
/* Checks */
//void check_compartments(int event_type){
//	if (contexts_sep[0][0]==contexts_sep[0][1] && event_type==0 ){
//		printf("Error: No more suscepibles individuals!");
//		exit(1);
//	}
//	else if (contexts_sep[0][1]==contexts_sep[0][2] && event_type==1){
//		printf("Error: No more infectious individuals!");
//		exit(1);
//	}
//}
//
//void sep_display(int arr[], int len, int arr_sep[], int len_sep){
//	int c,s;
//	s = 0;
//	for (c = 0; c < len; c++) {
//		printf("%d ", arr[c]);
//		while (arr_sep[s] == c && s < len_sep) {
//			printf("<- %d ", s);
//			s++;
//		}
//		printf("\n");
//	}
//}

//int old_get_sep_index(int id, int context_id) {
//	//int k = 0;
//	//int k = 3*individuals[id][context_id];
//	//clock_t start_t, get_index_t, loop_t;
//	//start_t = clock();
//	//if (contexts_sep[context_id][k]>first_index) {
//	//	printf("Error: get_sep_index; Initial value allready exeeded target!\n");
//	//}
//	//while (contexts_sep[context_id][k]<=first_index) {
//	//	k++;
//	//}
//	//loop_t = clock() - start_t;
//	////printf("get_index: %ld,\tloop:%ld\n", get_index_t, loop_t);
//	//return k-1;
//}

///* this is slow-ish too (bad scaling lin.)*/
//int get_sep_index(int id, int context_id) {
//	int k;
//	clock_t start_t, cond_t, get_index_t, loop_t;
//	start_t = clock();
//	if (context_id == 0){
//		k=0;
//	}
//	else {
//		k = 3*individuals[id][context_id];
//	}
//	cond_t = clock() - start_t;
//	int first_index = get_arr_index(id, context_id);
//	get_index_t = clock() - start_t;
//	if (contexts_sep[context_id][k]>first_index) {
//		printf("Error: get_sep_index; Initial value allready exeeded target!\n");
//	}
//	while (contexts_sep[context_id][k]<=first_index) {
//		k++;
//	}
//	loop_t = clock() - start_t;
//	//printf("cond_t: %ld,\tget_index: %ld,\tloop:%ld\n", cond_t, get_index_t, loop_t);
//	return k-1;
//}

//int get_next_sep(int target_index, int context_id) {
//	int k;
//	if (context_id == 0){
//		k = 0;
//	}
//	else {
//		k = 3*individuals[contexts_arr[context_id][target_index]][context_id];
//	}
//	if (target_index >= contexts_sep[context_id][3*contexts_num[context_id]-1]) {
//		printf("Error: get_next_sep; Target index larger than index of last separator element\n");
//		return -1;
//	}
//	while (contexts_sep[context_id][k]<=target_index) {	
//		k++;
//	}
//	return contexts_sep[context_id][k];
//}

//int get_first_index(int target_val, int arr[], int arr_len, int context_id) {
//	/* get index of first entry in int array, that matches the target_val:
//		Vars:	int target_val,	integer value to search for in list;	
//			int arr[],	array to search;
//			int len,	length of array to search;
//		Return: int sum, 	index of first entry match to target_val.	*/
//	int k;
//	int k_start = 0;
//	//if (context_id == 0){
//	//	k_start = relu_int(target_val - (N - contexts_sep[0][1]));
//	//}
//	//else{
//	//	int k_init = 3*individuals[target_val][context_id];
//	//	k_start = contexts_sep[context_id][k_init]; 
//	//}
//	////int k_init=3*individuals[target_val][context_id]; //=0;
//	for (k=k_start; k<arr_len; k++) {
//		if (arr[k] == target_val) {
//			return k;
//		}
//	}
//	printf("Error; get_first_index: element w. value %d, dont exist in the array\n", target_val);
//	exit(1);
//	return -1;
//}

//int *bin_search(int target_val, int sorted_arr[], int range_start_index, int range_end_index){
//	int k;
//	static int ret_arr[2];
//	if (range_end_index - range_end_index < 3){
//		//lin_search (int target_val, int sorted_arr[], int range_start_index, int range_end_index)
//		/* linear search for very small intervals */
//		for (k=range_start_index; k< range_end_index; k++){
//			if (sorted_arr[k] == target_val){
//				ret_arr[0] = k;
//				ret_arr[1] = sorted_arr[k];
//				return ret_arr;
//			}
//		}
//	}
//	/* Bisection/binary search if interval is large enough */
//	int split_index = (range_end_index - range_end_index) / 2;
//	if (target_val > sorted_arr[split_index]){
//		bin_search(target_val, sorted_arr, split_index, range_end_index);
//	}
//	else {
//		bin_search(target_val, sorted_arr, range_start_index, split_index);
//	}
//}

//int relu_int(int n){
//	if (n < 0){
//		return 0;
//	}
//	else{
//		return n;
//	}
//}

//Shift
//void shift_to_last(int index_move, int index_last, int arr[]){
//	/* Shift element in array to later posistion given by indices:
//		Vars:	int index_move,	index of element to move;	
//			int arr[],	array in which to shift elements;
//			int len,	length of previously described array;
//		Return: int void	*/
//	int k;
//	int temp = arr[index_move];
//	for (k=index_move; k<index_last;k++) {
//		arr[k] = arr[k+1];
//	}
//	arr[index_last] = temp;
//}

