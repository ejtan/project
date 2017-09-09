// Written by : Eric Tan

#ifndef SORT_H
#define SORT_H


void hybrid_quicksort(double *A, int lo, int hi, int cutoff);
void task_quicksort(double *A, int lo, int hi, int cutoff, bool is_parallel=false);
void random_quicksort(double *A, int lo, int hi);
void quicksort(double *A, int lo, int hi);
void insertsort(double *A, int lo, int hi);
void mergesort(double *A, int lo, int hi);


#endif
