#include "libBMP.h"
#include "helperFunctions.h"

#include <stdio.h>
#include <windows.h>
#include <utilapiset.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <unistd.h>

#define W 3500.0
#define H 2500.0

#define MIDDLE_X -25
#define MIDDLE_Y 24
#define ZOOM 10000

// #define X_MAX -0.546
// #define X_MIN -0.572
// #define Y_MAX -0.55
// #define Y_MIN -0.565

#define X_MAX 1.25
#define X_MIN -2.25
#define Y_MAX 1.25
#define Y_MIN -1.25

#define N_MAX 1000
#define THREADS 160

double image_coordinates_to_math_coordinates(long *bX, long *bY, double *mX, double *mY);
double math_coordinates_to_image_cooridnates(double *bX, double *bY, double *mX, double *mY);
long to_pos(long x, long y);

long recursion(double complex c, double complex z, long tiefe);
void calculate_set(uint32_t *data, long thread_nr, long threads);
void draw_color(uint32_t *data, long tiefe, double *bX, double *bY, double *mX, double *mY);
void calculate_image_position(long x_middle, long y_middle, long zoom);
uint32_t combine_color(uint32_t r, uint32_t g, uint32_t b);

DWORD WINAPI calculate_segment(uint32_t* data);

long thread_table[THREADS] = {0};
double x_max, x_min, y_max, y_min;
clock_t start, end;
double cpu_time_used;

long main(void) {
	
	if (H * W * 4 + 54 > 4000000000) {
		printf("\nImage size too big!\n\n");
		Beep(880,200);
		exit(-1);
	}
	
	start = clock();
	Beep(540,200);
	
	uint32_t *data = (uint32_t*) malloc(sizeof(uint32_t) * W * H); 	// Bilddaten
	
	
	calculate_image_position(MIDDLE_X, MIDDLE_Y, ZOOM);									// (-100 to 100, -100 to 100, zoom) 
	//printf("%d %d %d %d", X_MAX, X_MIN, Y_MAX, Y_MIN);
	
	printf("\nCalculating graph\n");
	
	DWORD   dwThreadIdArray[THREADS];
	for (long i = 0; i < THREADS; i++) {
		HANDLE thread = CreateThread(NULL, 0, calculate_segment, data, 0, NULL);
	}
	
	long not_finished = THREADS;
	long old_finished = 0;
	while(not_finished) {
		not_finished = THREADS;
		
		for (long i = 0; i < THREADS; i++) {
			if(*(thread_table + i) == 2) {
				not_finished--;
			}
		}
		if(not_finished != old_finished) {
			printf("Threads finished: %d%%\n", (THREADS-not_finished)* 100/THREADS);
			old_finished = not_finished;
		}
	}	
	
	printf("\nPainting graph\n");	
	bmp_create("bild_MT.bmp", data, W, H);
	printf("\nSuccess!\n");
	
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Needed %f Seconds / %f Minutes / %f Hours\n\n", cpu_time_used, cpu_time_used / 60, cpu_time_used / 3600);
	
	Beep(540,200);
	
	free(data);
	
	return(0);
}

DWORD WINAPI calculate_segment(uint32_t* data) {
	long thread_nr = 0;
	for(long i = 0; i < THREADS; i++) {  	// Extremely intelligent method to assign the tread_nr
		if(*(thread_table + i) == 0) {
			*(thread_table + i) += 1;
			thread_nr = i;
			break;
		}
	}
	calculate_set(data, thread_nr, THREADS);
	
	*(thread_table + thread_nr) += 1; 		// marking as finished
}

void calculate_set(uint32_t *data, long thread_nr, long threads) { //thread_nr 0 ... n - 1, threads n
	double bX = 0.0;
	double bY = 0.0;
	double mX = 0.0;
	double mY = 0.0;
	
	long tiefe = 0;
	complex double c;
	
	for (long xw = W / threads * thread_nr; xw < W / threads * (thread_nr  + 1); xw++) {	// Splitting the picture in |THREADS| columns
		for (long yh = 0; yh <= H; yh++) {
			
			image_coordinates_to_math_coordinates(&xw, &yh, &mX, &mY);
			
			c = mX + I * mY;
			
			tiefe = recursion(c, 0, 0);
			
			draw_color(data, tiefe, &bX, &bY, &mX, &mY);
			
		}
	}
}

long recursion(double complex c, double complex z, long tiefe) {
	
	if (tiefe > N_MAX) {
		return(tiefe - 1);
	}
	if (cabs(z) >= 2.0) {
		return(tiefe);
	}
	
	z = z * z + c;
	tiefe++;
	
	return(recursion(c, z, tiefe));
}

void draw_color(uint32_t *data, long tiefe, double *bX, double *bY, double *mX, double *mY) {	// Write pixel_data to data
	math_coordinates_to_image_cooridnates(bX, bY, mX, mY);
	
	uint32_t r, g, b, in;
	in = map_value(log(tiefe), 0, log(N_MAX/2), 0, 359);
	
	HSV_to_RGB(&r, &g, &b, in, 100, 100);
	
	*(data + to_pos((long)round(*bX), (long)round(*bY))) = combine_color(r, g, b);
}

double image_coordinates_to_math_coordinates(long *bX, long *bY, double *mX, double *mY) {
	*mX = x_min + ((*bX * (x_max - x_min)) / (W));
	*mY = y_min + ((*bY * (y_max - y_min)) / (H));
	
	return(0);
}

double math_coordinates_to_image_cooridnates(double *bX, double *bY, double *mX, double *mY) {
	*mY = map_value(*mY, y_min, y_max, y_max, y_min); 
	*bX = ((*mX - x_min) * (W)) / (x_max - x_min);
	*bY = ((*mY - y_min) * (H-1)) / (y_max - y_min);
	
	return(0);
}

long to_pos(long x, long y) {
	return((y * W) + x);
}

void calculate_image_position(long x_middle, long y_middle, long zoom) {
	
	x_min = map_value(x_middle - 10000/zoom, -100, 100, X_MIN, X_MAX);
	x_max = map_value(x_middle + 10000/zoom, -100, 100, X_MIN, X_MAX);
	y_min = map_value(y_middle + 10000/zoom, -100, 100, Y_MIN, Y_MAX);
	y_max = map_value(y_middle - 10000/zoom, -100, 100, Y_MIN, Y_MAX);
	
}

uint32_t combine_color(uint32_t r, uint32_t g, uint32_t b) {
	return 16*16*16*16*r + 16*16*g + b;
}