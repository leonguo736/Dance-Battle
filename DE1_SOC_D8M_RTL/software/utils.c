#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10

typedef struct {
    int array[ARRAY_SIZE];
    int current_size;
    int oldest_index;
} ArrayMedian;

ArrayMedian *new_array_median() {
    ArrayMedian *am = (ArrayMedian *) malloc(sizeof(ArrayMedian));
    am->current_size = 0;
    am->oldest_index = 0;
    return am;
}

void add_to_array(ArrayMedian *am, int value) {
    if (am->current_size < ARRAY_SIZE) {
        am->array[am->current_size] = value;
        am->current_size++;
    } else {
        am->array[am->oldest_index] = value;
        am->oldest_index = (am->oldest_index + 1) % ARRAY_SIZE;
    }
}

int compare(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}

float get_median(ArrayMedian *am) {
    int copy[am->current_size];
    int i;

    for (i = 0; i < am->current_size; i++) {
        copy[i] = am->array[i];
    }

    qsort(copy, am->current_size, sizeof(int), compare);

    if (am->current_size % 2 == 0) {
        return (copy[am->current_size / 2 - 1] + copy[am->current_size / 2]) / 2.0;
    } else {
        return copy[am->current_size / 2];
    }
}

int main() {
    ArrayMedian *am = new_array_median();

    add_to_array(am, 5);
    add_to_array(am, 2);
    add_to_array(am, 7);
    add_to_array(am, 4);

    printf("Median: %.2f\n", get_median(am));

    add_to_array(am, 1);
    add_to_array(am, 8);

    printf("Median: %.2f\n", get_median(am));

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double distance(double* point1, double* point2, int dim) {
    double sum = 0.0;
    for (int i = 0; i < dim; i++) {
        double diff = point1[i] - point2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

double median(double* arr, int n) {
    double med;
    int mid = n / 2;
    if (n % 2 == 0) {
        med = (arr[mid - 1] + arr[mid]) / 2.0;
    } else {
        med = arr[mid];
    }
    return med;
}

int main() {
    // Define the set of points
    double points[4][2] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    int n = 4;
    int dim = 2;
    
    // Calculate pairwise distances
    double distances[n][n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distances[i][j] = distance(points[i], points[j], dim);
        }
    }
    
    // Calculate median distances
    double medians[n];
    for (int i = 0; i < n; i++) {
        double sorted_distances[n];
        for (int j = 0; j < n; j++) {
            sorted_distances[j] = distances[i][j];
        }
        qsort(sorted_distances, n, sizeof(double), (int (*)(const void*, const void*)) &double_compare);
        medians[i] = median(sorted_distances, n);
    }
    
    // Find the point with the smallest median distance
    int min_index = 0;
    for (int i = 1; i < n; i++) {
        if (medians[i] < medians[min_index]) {
            min_index = i;
        }
    }
    double* median_point = points[min_index];
    
    printf("Median point: (%f, %f)\n", median_point[0], median_point[1]);
    
    return 0;
}
