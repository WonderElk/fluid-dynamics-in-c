/* Definitions*/
#define _POSIX_C_SOURCE 200809L

/* Includes */
#include <stdio.h> // Standard
#include <stdlib.h> // Standard
#include <stdbool.h> // For booleans
#include <math.h> // I.a. gives the "to the power off" function via pow(base, exp)
#include <pthread.h> // Multithreading
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_statistics.h>

#include <time.h> // For testing time taken


/* Shared var.s and structs */
typedef struct {
    double *axis;
    size_t size;
    double end;
} ThreadArgs; 

/* Data */

/* Functions */
// Modify array to comprise elements from value "start" to value "end" 
//     with evenly spaced steps
void linspace(double *arr, size_t size, double start, double end) {
    if (size == 1) {
        arr[0] = start;
        return;
    }
    else{
        double step = (end - start)/(size - 1);
        for (size_t i = 0; i < size; i++) {
            arr[i] = start + i*step;
        }
    }
}

// Modify array to comprise elements from value "start" to value "end"
//     with logmarithmicly spaced steps
void logSpace(double *arr, size_t size, double start, double end){
    if(size > 1 && start > 0 && end > 0){
        for (size_t i = 0; i < size; i++) {
            arr[i] = start*pow(end/start, (double)i/(size - 1));
        }
    }
    else{
        printf("logSpace got shitty input!!! Abort!");
    }
}

// Child thread function used to deligate processes to respective thread
void *cFunc(void *arg)
{
    ThreadArgs *args = arg; // Void pointer must be defined

    linspace(args->axis, args->size, 0.0, args->end);

    return NULL;
}

// Writes taken from ChatGPT
void write_xy_data(const char *filename, double *x, double *y, size_t n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) { perror("fopen"); return; }

    for (size_t i = 0; i < n; i++) {
        fprintf(fp, "%g %g\n", x[i], y[i]);
    }

    fclose(fp);
}
void write_diff_data(const char *filename, double *x, double *y1, double *y2, size_t n) {
    FILE *fp = fopen(filename, "w");
    if (!fp) { perror("fopen"); return; }

    for (size_t i = 0; i < n; i++) {
        fprintf(fp, "%g %g %g %g\n", x[i], y1[i], y2[i], y1[i] - y2[i]);
    }

    fclose(fp);
}

// Time how long it takes to create two threads, each creating an axis each (length ny, nx respectively), 
//      and to finally "free" the threads (have them join the master thread)
double solveParallel(size_t nx, size_t ny){
    const double xend = 100;
    const double yend = 100;
    pthread_t mThread[2]; // Init. main thread

    double *x = malloc(nx * sizeof(double));
    double *y = malloc(ny * sizeof(double));

    ThreadArgs args[2] = {
        {x, nx, xend},
        {y, ny, yend}
    };
    
    // Time taken for child threads to initiate and do linspace of nx and ny each
    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&mThread[0], NULL, cFunc, &args[0]);
    pthread_create(&mThread[1], NULL, cFunc, &args[1]);
    pthread_join(mThread[0], NULL);
    pthread_join(mThread[1], NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; 
    // Note: tv_sec is whole seconds while tv_nsec is fractional numbers
    free(x); free(y);
    return t;
}

// Time how long it takes to create two axes of timensions nx and ny respectively
double solveSeries(size_t nx, size_t ny){
    const double xend = 100;
    const double yend = 100;

    double *x = malloc(nx * sizeof(double));
    double *y = malloc(ny * sizeof(double));

    // Time taken to do linspace of dim nx and ny
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    linspace(x, nx, 0, xend);
    linspace(y, ny, 0, yend);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    free(x); free(y);
    return t;
}

/*  Func: timeTest
        Solves linspace with increasing dimensions in either 
        parallel or series and modifies given file with the 
        data: time taken vs. dimension of array.
    Input: 
        * func: A function that returns a double and takes two size_t arguments
        * fName: The name of the file that the data is written to.
*/
void timeTest(double (*func)(size_t, size_t), char *fName, double maxDim){
    const size_t statSamples = 30;
    const size_t dataPoints = 50;
    double myHardwareLimit;
    if (maxDim > 0){
        myHardwareLimit = maxDim;
    }
    else{
        myHardwareLimit = 2e7; // Actuall hardware limit: 2e8; //Which takes minutes to resolve
    }
    double *y = malloc(dataPoints*sizeof(double));
    double *N = malloc(dataPoints*sizeof(double));
    logSpace(N, dataPoints, 1.0, myHardwareLimit);
    double x[statSamples];
    for(size_t i = 0; i < dataPoints; i++){
        for(size_t j= 0; j < statSamples; j++){
            x[j] = func((size_t)N[i], (size_t)N[i]);
        }
        y[i] = gsl_stats_mean(x, 1, statSamples);
    }
    write_xy_data(fName, N, y, dataPoints);
    free(y);
    free(N);
}

/* Main */
int main(int argc, char *argv[]){
    if(argc == 2){
        double maxDim = atof(argv[1]);
        timeTest(solveParallel, "solveParallel.dat", maxDim);
        timeTest(solveSeries, "solveSeries.dat", maxDim);
    }
    else{
        timeTest(solveParallel, "solveParallel.dat", 0);
        timeTest(solveSeries, "solveSeries.dat", 0);
    }

    return 0;
}