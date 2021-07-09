#include <stdio.h>
#include <windows.h>
#include <utilapiset.h>
#include "libBMP.h"
#include <math.h>
#include <complex.h>
#include <time.h>
#include <unistd.h>

#define W 26000.0
#define H 15000.0
#define X_MAX -0.546
#define X_MIN -0.572
#define Y_MAX -0.55
#define Y_MIN -0.565
/*
#define W 4000.0
#define H 3000.0
#define X_MAX 1.25
#define X_MIN -2.25
#define Y_MAX 1.25
#define Y_MIN -1.25*/

#define N_MAX 1000
#define THREADS 160


double toMath(long *bX, long *bY, double *mX, double *mY);
double toBMP(double *bX, double *bY, double *mX, double *mY);
long toPos(long x, long y);
double cut(double x, double low, double up);
double map(double x, double in_min, double in_max, double out_min, double out_max);

long reku(double complex c, double complex z, long tiefe);
void calc(uint32_t *data, long thread_nr, long threads);
void draw_colour(uint32_t *data, int tiefe, double *bX, double *bY, double *mX, double *mY);

DWORD WINAPI ThreadFunc(uint32_t* data);

int thread_table[THREADS] = {0};
clock_t start, end;
double cpu_time_used;

int main(void) {
	
	start = clock();
	Beep(440,100);
	//nice cc -c *.c;
	
	uint32_t *data = (uint32_t*) malloc(sizeof(uint32_t) * W * H); // Bilddaten
	
	
	printf("\nPainting background\n");
	for (long i = 0; i < W * H; i++) {
		*(data + i) = COLOR_WHITE;
	}
	
	printf("\nCalculating graph\n");
	
	DWORD   dwThreadIdArray[THREADS];
	for (long i = 0; i < THREADS; i++) {
		
		HANDLE thread = CreateThread(NULL, 0, ThreadFunc, data, 0, NULL);
		//usleep(1000);
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
		//sleep(1);
	}	
	
	printf("\nPainting graph\n");	
	bmp_create("bild_MT.bmp", data, W, H);
	printf("\nSuccess!\n");
	
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Needed %f Seconds / %f Minutes / %f Hours\n\n", cpu_time_used, cpu_time_used / 60, cpu_time_used / 3600);
	
	Beep(440,100);
	
	free(data);
	
	return(0);
}

DWORD WINAPI ThreadFunc(uint32_t* data) {
	long thread_nr = 0;
	for(long i = 0; i < THREADS; i++) {
		if(*(thread_table + i) == 0) {
			*(thread_table + i) += 1;
			thread_nr = i;
			break;
		}
	}
	//printf("Thread Nr. %d started\n", thread_nr+1);
	
	calc(data, thread_nr, THREADS);
	
	//printf("\tThread Nr. %d finished\n", thread_nr+1);
	*(thread_table + thread_nr) += 1;
}

void calc(uint32_t *data, long thread_nr, long threads) { //thread_nr 0 ... n - 1, threads n
	double bX = 0.0;
	double bY = 0.0;
	double mX = 0.0;
	double mY = 0.0;
	
	long tiefe = 0;
	complex double c;
	
	for (long xw = W / threads * thread_nr; xw < W / threads * (thread_nr  + 1); xw++) {
		for (long yh = 0; yh < H; yh++) {
			
			toMath(&xw, &yh, &mX, &mY);
			
			c = mX + I * mY;
			
			tiefe = reku(c, 0, 0);
			
			draw_colour(data, tiefe, &bX, &bY, &mX, &mY);
			
		}
	}
}

long reku(double complex c, double complex z, long tiefe) {
	
	if (tiefe > N_MAX) {
		return(tiefe - 1);
	}
	if (cabs(z) >= 2.0) {
		return(tiefe);
	}
	
	z = z * z + c;
	tiefe++;
	
	return(reku(c, z, tiefe));
}

double toMath(long *bX, long *bY, double *mX, double *mY) {
	*mX = X_MIN + ((*bX * (X_MAX - X_MIN)) / (W));
	*mY = Y_MIN + ((*bY * (Y_MAX - Y_MIN)) / (H));
	
	return(0);
}

double toBMP(double *bX, double *bY, double *mX, double *mY) {
	*mY = map(*mY, Y_MIN, Y_MAX, Y_MAX, Y_MIN); 
	*bX = ((*mX - X_MIN) * (W - 1)) / (X_MAX - X_MIN);
	*bY = ((*mY - Y_MIN) * (H - 1)) / (Y_MAX - Y_MIN);
	
	return(0);
}

long toPos(long x, long y) {
	return((y * W) + x);
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {		//credit to Arduino
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_colour(uint32_t *data, int tiefe, double *bX, double *bY, double *mX, double *mY) {
	toBMP(bX, bY, mX, mY);
	
	*(data + toPos((long)round(*bX), (long)round(*bY))) = (long) map(tiefe, 0, N_MAX, 0, 16777215);
}