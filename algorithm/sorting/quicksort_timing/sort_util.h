// Writtn by : Eric Tan

#ifndef SORT_UTIL_H
#define SORT_UTIL_H


int partition(double *A, int lo, int hi);
void swap_median(double *A, int lo, int hi);
void merge(double *A, int lo, int mid, int hi);
void swap_arrays(double *A, double *B, int N);
bool is_sorted(double *A, int N);


#endif
