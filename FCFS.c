#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int input, AWT, *WT, ATAT,*TAT, ART, *RT,  ctr = 0, result = 0;
sem_t *print_semaphores;

int compare(const void *a, const void *b) {
    const int *arr1 = (const int *)a;
    const int *arr2 = (const int *)b;
    return arr1[1] - arr2[1];
}

void *thread(void *arg) {
    int *thread_data = (int*)arg;
    sem_wait(&print_semaphores[thread_data[0]]); 
    
    printf("Thread %d: ", ctr+1);
    for (int i = 0; i < 4; i++) {
        if (i == 0) {
            printf("%d ", thread_data[i]);
        } 
        else if (i == 3) {
            result += thread_data[i];
            TAT[ctr] = result - TAT[ctr];  
            printf("%d ", result);
        }
        else {
            if (i == 1){
                TAT[ctr] = thread_data[i];
                RT[ctr] -= thread_data[i];
            }
            printf("%d ", result);
            result += thread_data[i];
        }
    }
    printf("\n");

    if (ctr < input){
        (result < thread_data[5])? (thread_data[5] -= result) : (thread_data[5] = 0);
        sem_post(&print_semaphores[thread_data[4]]);
        ctr++;
        WT[ctr] = result;
        RT[ctr] = result;
    }
    
    pthread_exit(NULL);
}

int main(void) {
    printf("How many times?: ");
    scanf("%d", &input);

    printf("Please input the ID: \n");
    int arr[input][4];

    for (int j = 0; j < input; j++) {
        printf("Task %d: ", j + 1);
        for (int i = 0; i < 4; i++) {
            scanf("%d", &arr[j][i]);
        }
    }
    printf("\n");
    int rows = sizeof(arr) / sizeof(arr[0]);
    qsort(arr, rows, sizeof(arr[0]), compare);

    int max_thread_id = arr[0][0];
    for (int j = 1; j < input; j++) {
        if (arr[j][0] > max_thread_id) {
            max_thread_id = arr[j][0];
        }
    }
    WT = malloc(input * sizeof(int));
    TAT = malloc(input * sizeof(int));
    RT = malloc(input * sizeof(int));
    WT[0] = RT[0] = 0;

    // Initialize the semaphores for printing
    print_semaphores = malloc((max_thread_id + 1) * sizeof(sem_t));
    if (print_semaphores == NULL) {
        fprintf(stderr, "Failed to allocate memory for semaphores\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < input; i++) {
        if (sem_init(&print_semaphores[arr[i][0]], 0, 0) != 0) {
            fprintf(stderr, "Failed to initialize semaphore\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_t threads[input];

    for (int i = 0; i < input; i++) {
        pthread_create(&threads[i], NULL, thread, (void*)arr[i]);
    }

    sem_post(&print_semaphores[arr[0][0]]);

    for (int i = 0; i < input; i++) {
        pthread_join(threads[i], NULL);
        AWT += WT[i];
        ATAT += TAT[i];
        ART += RT[i];
    }
    printf("Average: %.2f %.2f %.2f\n", (float)(AWT/input),  (float)(ART/input), (float)(ATAT/input));

    for (int i = 0; i < input; i++) {
        sem_destroy(&print_semaphores[arr[i][0]]);
    }
    free(print_semaphores);
    return 0;
}