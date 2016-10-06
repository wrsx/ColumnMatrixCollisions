#include<stdio.h>
#include<omp.h>

int k=0;

int partition(void *a[], int low_index, int high_index, int (*cmp)(const void *, const void *)) {
    int i, j; 
    void *temp, *key;
    key = a[low_index];
    i = low_index + 1;
    j = high_index;
    while (1) {
        while (i < high_index && (*cmp)(key, a[i])  >= 0)
            i++;
        while ((*cmp)(key, a[i]) < 0)
            j--;
        if (i < j) {
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        } else {
            temp = a[low_index];
            a[low_index] = a[j];
            a[j] = temp;
            return (j);
        }
    }
}

void quicksort(void *a, int low_index, int high_index, int (*cmp)(const void *, const void *)) {
    int j;

    if (low_index < high_index) {
        j = partition(a, low_index, high_index, cmp);
        //printf("Pivot element with index %d has been found out by thread %d\n", j, k);

        #pragma omp parallel sections 
        {
            #pragma omp section 
            {
                k = k + 1;
                quicksort(a, low_index, j - 1, cmp);
            }

            #pragma omp section 
            {
                k = k + 1;
                quicksort(a, j + 1, high_index, cmp);
            }

        }
    }
}
