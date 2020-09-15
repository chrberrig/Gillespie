#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 10000 /* Pop storrelse */
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
	for(loop = len; loop >= 0; loop--) {
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
	for(loop = len; loop >= 0; loop--) {
		sum = sum + arr[loop];      
	}	
	return sum;
}

//Index
int get_first_index(int target_val, int arr[], int arr_len) {
	/* get index of first entry in int array, that matches the target_val:
		Vars:	int target_val,	integer value to search for in list;	
			int arr[],	length of array to search;
			int len,	length of array to search;
		Return: int sum, 	index of first entry match to target_val.	*/
	int k;
	for (k=0; k<arr_len; k++) {
		if (arr[k] == target_val) {
			return k;
		}
	}
	printf("Error: element w. value %d, dont exist in the array\n", target_val);
	exit(1);
	return -1;
}

//Shift
void shift_to_last(int index_move, int index_last, int arr[]){
	int k;
	int temp = arr[index_move];
	for (k=index_move; k<index_last;k++) {
		arr[k] = arr[k+1];
	}
	arr[index_last] = temp;
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

void sep_display(int arr[], int len, int arr_sep[], int len_sep){
	int c,s;
	s = 0;
	for (c = 0; c < len; c++) {
		printf("%d ", arr[c]);
		while (arr_sep[s] == c && s < len_sep) {
			printf("<- %d ", s);
			s++;
		}
		printf("\n");
	}
}

int get_sep_index(int id, int context_id) {
	int k=0;
	int first_index = get_first_index(id, contexts_arr[context_id], N);
	if (contexts_sep[context_id][k]>first_index) {
		printf("Error: get_sep_index; Initial value allready exeeded target!\n");
	}
	while (contexts_sep[context_id][k]<=first_index) {
		k++;
	}
	return k-1;
}


int get_next_sep(int target_index, int context_id) {
	int k = 0;
	if (target_index >= contexts_sep[context_id][3*contexts_num[context_id]-1]) {
		printf("Error: get_next_sep; Target index larger than index of last separator element");
		//int ret_arr[] = {-1, -1};
		//return ret_arr;
		return -1;
	}
	while (contexts_sep[context_id][k]<=target_index) {	
		k++;
	}
	//int ret_arr[] = {k, arr_sep[k]};
	//return ret_arr;
	return contexts_sep[context_id][k];
}

/* sir functions */
void sir_init() {
	int j, k;
	printf("separators in sir init:\n");
	for (j=0; j<C; j++) { 
		for (k=0; k<3*contexts_num[j]; k++) {
			if (k<3*(contexts_num[j]-1)){
				contexts_sir[j][k%3][k/3] = contexts_sep[j][k+1] - contexts_sep[j][k];
			} 
			else if (k<3*contexts_num[j]) { 
				contexts_sir[j][k%3][k/3] = N - contexts_sep[j][k];
			}
		}
	}
}


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
		printf("S2:%d,\tI2:%d,\tR2:%d\n", contexts_sir[context_index][0][j], 
						contexts_sir[context_index][1][j], 
						contexts_sir[context_index][2][j]);
	}
	printf("----------------\n");
}

void update_compartment(int id, int j) { 
	/*
	Update compartment wrt individual and social context:
		vars:	int id: 	id of individual;
			int j: 		social context
		return:	void
	*/
	/* Update array and seperator */
	int index_move = get_first_index(id, contexts_arr[j], N);
	int sep_type_index = get_sep_index(id, j); 
	int index_last = get_next_sep(index_move, j);
	shift_to_last(index_move, index_last - 1, contexts_arr[j]);
	int index_next_sep = get_first_index(index_last, contexts_sep[j], 3*contexts_num[j]);
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

	/* Update sir */
	contexts_sir[j][sep_type_index%3][sep_type_index/3]--;
	contexts_sir[j][sep_type_index%3 + 1][sep_type_index/3]++;
}

void update_all_compartments(int id) {
	int j = 0;
	for (j=0; j<C; j++) { 
		update_compartment(id, j); 
		//printf("compartment updated\n");
		//arr_display(contexts_sep[j], 3*contexts_num[j]);
		//sep_display(contexts_arr[j], N, contexts_sep[j], 3*contexts_num[j]);
	}

}

void update_sum(int context_id) {
	int j, k; 
	double type0_sum=0, type1_sum=0;
	double t_sum = 0;	
	for (k=0; k<contexts_num[context_id]; k++){
		type0_sum = type0_sum + beta[context_id][k]*contexts_sir[context_id][0][k]*contexts_sir[context_id][1][k];
		type1_sum = type1_sum + gam[context_id]*contexts_sir[context_id][1][k];
	}
	contexts_sums[context_id][0] = type0_sum; 
	contexts_sums[context_id][1] = type1_sum;
	for (j=0; j<C; j++) {
		t_sum = t_sum + contexts_sums[j][0];
	}
	type_sums[0] = t_sum;
	type_sums[1] = contexts_sums[0][1];
}


/* Events */
double choose_event_time(int type) {
	double rd, tau, dt;
	rd = (double)random()/RAND_MAX;
	tau = type_sums[type];
	dt = -log(rd)/tau;
	return dt;
}
	
int choose_event_type() {
	double rd, cumul_t_sum, normaliz;
	rd = (double)random()/RAND_MAX;
	cumul_t_sum = 0;
	normaliz = d_arr_sum(type_sums, T);
	int k;	
	for (k=0; k<T; k++) {
		cumul_t_sum = cumul_t_sum + type_sums[k];
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
	double rd, cumul_c_sum;//, normaliz;
	rd = (double)random()/RAND_MAX;
	cumul_c_sum = 0;
	for (j=0; j<C; j++) {
		cumul_c_sum = cumul_c_sum + contexts_sums[j][type_id];
		if (rd<cumul_c_sum/type_sums[type_id]) {
			return j;
		}
	}
	printf("Error: Event context not chosen!\n");
	return -1;
	exit(1);
}

int choose_event_individual(int type){
	int rint = rand_int(contexts_sep[0][type], contexts_sep[0][type+1]);
	return contexts_arr[0][rint];
}

/* Checks */
void check_compartments(int event_type){
	if (contexts_sep[0][0]==contexts_sep[0][1] && event_type==0 ){
		printf("Error: No more suscepibles individuals!");
		exit(1);
	}
	else if (contexts_sep[0][1]==contexts_sep[0][2] && event_type==1){
		printf("Error: No more infectious individuals!");
		exit(1);
	}
}

void gen_context_ids(int context_id, int con_size, int con_list[]) {
	int k;
	if (con_size < N - global_counter) {
		for (k=global_counter; k<global_counter+con_size; k++) {
			con_list[k] = contexts_num[context_id];
		}
		global_counter = global_counter + con_size;
		contexts_num[context_id]++;
	}
	else {
		con_size = N - global_counter;
		for (k=global_counter; k<global_counter+con_size; k++) {
			con_list[k] = contexts_num[context_id];
		}
		global_counter = global_counter + con_size;
		contexts_num[context_id]++;
	}
}

/* Init functions*/
void init_individuals(){
	int k;

	/* initialize individials register array */
	int work_avg = 6;
	int fam_avg = 2;

	/* comp */
	contexts_num[0] = 1;
	for (k=0; k<N; k++) {
		individuals[k][0] = k; 
		contexts_arr[0][k] = k;
	}

	/* work */
	global_counter = 0;
	int work_size = 0;
	int work_list[N];
	while (global_counter < N) {
		work_size = rand_int(1, 2*work_avg+1);
		gen_context_ids(1, work_size, work_list);
	}
	arr_shuffle(work_list, N);
	for (k=0; k<N; k++) {
		individuals[k][1] = work_list[k];
	}	

	/* fam */
	global_counter = 0;
	int fam_size = 0;
	int fam_list[N];
	while (global_counter < N) {
		fam_size = rand_int(1, 2*fam_avg+1);
		gen_context_ids(2, fam_size, fam_list);
	}
	arr_shuffle(fam_list, N);
	for (k=0; k<N; k++) {
		individuals[k][2] = fam_list[k];
	}	
}

void init_arr(int context_index) { // this is also very slow, though only an init-funct.
	/* initialize arrays and separators of the different social contexts. etc. */
	
	//int a, s;
	//for (a=0; a<contexts_num[context_index]; a++) {
	//	s = s+
	//	if (a%3==1){
	//		
	//	}
	//}
	// ----------------------------------------------------------
	int a, c, s, k;
	c = 0;
	s = 0;
	for (a=0; a<contexts_num[context_index]; a++) {
		contexts_sep[context_index][s] = c;
		s++;
		for (k=0; k<N; k++) {
			if (individuals[k][context_index] == a) {
				contexts_arr[context_index][c] = k;
				c++;
			}
		}
		if (a != contexts_num[context_index]-1) {
			contexts_sep[context_index][s] = c;
			s++;
			contexts_sep[context_index][s] = c;
			s++;
		}
	}
	contexts_sep[context_index][s] = N;
	s++;
	contexts_sep[context_index][s] = N;
	s++;
}

void init_all_arr() { 
	int context_index;
	for (context_index=0; context_index<C; context_index++) {
		printf("Init array\n");
		init_arr(context_index);
		printf("Init array done!\n");
	}
}

int main(){
	
	/* Iterables */
	int j, k;
	
	/* Variables */
	int i0 = 5;
	int rint;
	//double tot, rd;
	
	int type;
	int context;
	int id;
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
	
	init_all_arr();
	printf("Array init done! \n");

	/* Init. constants for model */
	/* This must be below init_all_arr() */
	for (j=0; j<C; j++) {
		for (k=0; k<contexts_num[j]; k++) {
			beta[j][k] = b[j];
		}
		gam[j] = v;
	}
	printf("Constants init done! \n");
	

	sir_init();
	printf("SIR init done! \n");

	printf("\n----------------\nInit_infection!\n----------------\n");

	/* initialize a few infected individuals */
	for (k=0; k<i0; k++) { 
		//check_compartments(0);
		rint = rand_int(contexts_sep[0][0], contexts_sep[0][1]);
		id = contexts_arr[0][rint];
		printf("elem: %d\n", id);
		update_all_compartments(id);
		printf("all_compartments updated\n");
		for (j=0; j<C; j++) {
			update_sum(j);
		}
	}
	
	printf("\n----------------\nRunning Sim!\n----------------\n");
	
	FILE *sir_data;
	sir_data = fopen( "sir_data.csv", "w+" );

	fprintf(sir_data, "t,S,I,R\n");
	fprintf(sir_data, "%g,%d,%d,%d\n", t, contexts_sep[0][1]-contexts_sep[0][0] , contexts_sep[0][2]-contexts_sep[0][1], N-contexts_sep[0][2]);
	while (contexts_sep[0][2]-contexts_sep[0][1]>0) {
		type = choose_event_type();
		dt = choose_event_time(type);
		t += dt;
		context = choose_event_context(type);
		id = choose_event_individual(type);
		//printf("--------------------\n");
		//printf("dt:%g,\tt:%g,\ttype:%d,\tcon:%d,\tid:%d\n", dt, t, type, context, id);
		//printf("--------------------\n");
		update_all_compartments(id);
		for (j=0; j<C; j++) {
			update_sum(j);
		}
		fprintf(sir_data, "%g,%d,%d,%d\n", t, contexts_sep[0][1]-contexts_sep[0][0] , contexts_sep[0][2]-contexts_sep[0][1], N-contexts_sep[0][2]);
		//fprintf(sir_data, "t:%g,\ttype:%d,\tcon:%d,\tid:%d\n", t, type, context, id);
	}
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

