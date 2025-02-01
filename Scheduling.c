#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>

typedef struct {
    int task_id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int completion_time;
} Task;

int input, current_time = 0;
pthread_mutex_t *mutex;
pthread_cond_t cond[2];
Task *tasks;

int compare(const void *a, const void *b) {
    const Task *task1 = (const Task *)a;
    const Task *task2 = (const Task *)b;

    if (task1->arrival_time < task2->arrival_time) {
        return -1;
    } else if (task1->arrival_time > task2->arrival_time) {
        return 1;
    } else {
        return (task1->task_id < task2->task_id) ? -1 : (task1->task_id > task2->task_id);
    }
}

void *FCFS(void *arg) {
    Task *task = (Task *)arg;
    bool is_completed = false;
    int ctr = 0;

    pthread_mutex_lock(&mutex[0]);
    while (current_time < task->arrival_time) {
        current_time++;
    }

    while (true) {
        if (task->remaining_time > 0) {
            printf("Task %d: %d %d\n", task->task_id + 1, current_time, current_time + 1);
            task->remaining_time--;
            usleep(100000);
            if(++ctr && ctr == 1){
                task->start_time = current_time;
            };
            current_time++;

            if (task->remaining_time == 0) {
                is_completed = true;
                task->completion_time = current_time;
                pthread_mutex_unlock(&mutex[0]);
            }
        }
        if (is_completed) break;
    }
    return NULL;
}

int min_time_scheduler() { 
    int min_time = INT_MAX, shortest_task = -1;
    
    for (int i = 0; i < input; i++) {
        if (tasks[i].arrival_time <= current_time && tasks[i].remaining_time > 0) {
            if (tasks[i].remaining_time < min_time) {
                min_time = tasks[i].remaining_time;
                shortest_task = i;
            }
        }
    }
    return shortest_task;
}

void *SRTF(void *arg) {
    Task *task = (Task *)arg;
    bool is_completed = false;
    int ctr = 0;

    pthread_mutex_lock(&mutex[1]);
    while (current_time < task->arrival_time) {
        current_time++;
    }

    while (true) {
        int next_task = min_time_scheduler();
        if (next_task == -1) {
            pthread_mutex_unlock(&mutex[1]);;
            break;
        }

        while (task->task_id != next_task) {
            pthread_cond_wait(&cond[1], &mutex[1]);
            next_task = min_time_scheduler();
            if (next_task == -1) {
                pthread_mutex_unlock(&mutex[1]);
                return NULL;
            }
        }

        if (task->remaining_time > 0) {
            printf("Task %d: %d %d\n", task->task_id + 1, current_time, current_time + 1);
            task->remaining_time--;
            usleep(100000);
            if(++ctr && ctr == 1){
                task->start_time = current_time;
            };
            current_time++;

            if (task->remaining_time == 0) {
                is_completed = true;
                task->completion_time = current_time;
            }
        }

        pthread_cond_signal(&cond[1]);
        pthread_mutex_unlock(&mutex[1]);

        if (is_completed) break;
    }
    return NULL;
}

void *RR(void *arg) {
    Task *task = (Task *)arg;
    int time_quantum = 4;
    bool is_completed = false;
    int ctr = 0;

    if (pthread_mutex_lock(&mutex[2]) != 0) {
        perror("Failed to lock mutex");
    }

    while (true) {
        if (current_time < task->arrival_time) {
            current_time++;
            pthread_mutex_unlock(&mutex[2]);
            usleep(100000);
            continue;
        }

        if (task->remaining_time > 0) {
            printf("Task %d: ", task->task_id + 1);
            if (task->remaining_time > time_quantum) {
                current_time += time_quantum;
                task->remaining_time -= time_quantum; 
                if(++ctr && ctr == 1){
                    task->start_time = current_time;
                };
                printf("%d %d\n", current_time - time_quantum, current_time);
            } else {
                current_time += task->remaining_time;
                printf("%d %d\n", current_time - task->remaining_time, current_time);
                task->remaining_time = 0;
                is_completed = true;
                task->completion_time = current_time;
            }
        }
        pthread_mutex_unlock(&mutex[2]);
        usleep(100000); 

        if (is_completed) {
            break;
        }
    }

    return NULL;
}

int min_priority_scheduler(){
    int min_priority = INT_MAX, shortest_priority = -1;
    
    for (int i = 0; i < input; i++) {
        if (tasks[i].arrival_time <= current_time && tasks[i].remaining_time > 0) {
            if (tasks[i].priority < min_priority) {
                min_priority = tasks[i].priority;
                shortest_priority = i;
            } else if (tasks[i].priority == min_priority) {
                if (tasks[i].remaining_time < tasks[shortest_priority].remaining_time) {
                    shortest_priority = i;
                }
            }
        }
    }
    return shortest_priority;
}

void *PS(void *arg) {
    Task *task = (Task *)arg;
    bool is_completed = false;
    int ctr = 0;

    pthread_mutex_lock(&mutex[1]);
    while (current_time < task->arrival_time) {
        current_time++;
    }

    while (true) {
        int next_task = min_priority_scheduler();
        if (next_task == -1) {
            pthread_mutex_unlock(&mutex[1]);;
            break;
        }

        while (task->task_id != next_task) {
            pthread_cond_wait(&cond[1], &mutex[1]);
            next_task = min_priority_scheduler();
            if (next_task == -1) {
                pthread_mutex_unlock(&mutex[1]);
                return NULL;
            }
        }

        if (task->remaining_time > 0) {
            printf("Task %d: %d %d\n", task->task_id + 1, current_time, current_time + 1);
            task->remaining_time--;
            usleep(100000);
            if(++ctr && ctr == 1){
                task->start_time = current_time;
            };
            current_time++;

            if (task->remaining_time == 0) {
                is_completed = true;
                task->completion_time = current_time;
            }
        }

        pthread_cond_signal(&cond[1]);
        pthread_mutex_unlock(&mutex[1]);

        if (is_completed) break;
    }
    return NULL;
}

float calculate_avg_TAT(Task tasks[]) {
    int total_TAT = 0;

    for (int i = 0; i < input; i++) {
        int TAT = tasks[i].completion_time - tasks[i].start_time;
        total_TAT += TAT;
    }

    return (float)total_TAT / input;
}

float calculate_avg_WT(Task tasks[]) {
    int total_WT = 0;

    for (int i = 0; i < input; i++) {
        int WT = tasks[i].completion_time - tasks[i].arrival_time - tasks[i].burst_time;
        total_WT += WT;
    }

    return (float)total_WT / input;
}

int main(void) {
    int option;
    printf("Select Scheduling Algorithm:\n");
    printf("1. FCFS\n");
    printf("2. SRTF\n");
    printf("3. RR\n");
    printf("4. PS\n");
    printf("Enter your choice: ");
    scanf("%d", &option);

    printf("How many tasks: ");
    scanf("%d", &input);

    tasks = malloc(input * sizeof(Task));
    if (tasks == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    mutex = malloc((input + 1) * sizeof(*mutex));
    for (int i = 0; i < input; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    if(option != 4){
        printf("\t Arrival Time \t Burst Time\n");
        for (int i = 0; i < input; i++) {
            printf("Task %d: ", i + 1);
            tasks[i].task_id = i;
            scanf("%d %d", &tasks[i].arrival_time, &tasks[i].burst_time);
            tasks[i].remaining_time = tasks[i].burst_time;
        }
    } else {
        printf("\t Arrival Time \t Burst Time \t Priority\n");
        for (int i = 0; i < input; i++) {
            printf("Task %d: ", i + 1);
            tasks[i].task_id = i;
            scanf("%d %d %d", &tasks[i].arrival_time, &tasks[i].burst_time, &tasks[i].priority);
            tasks[i].remaining_time = tasks[i].burst_time;
        }
    }
    printf("\n");
    qsort(tasks, input, sizeof(Task), compare);

    printf("Hasil Quick Sort:\n");
    for (int i = 0; i < input; i++) {
        printf("Task %d: ", tasks[i].task_id + 1);
        printf("%d %d", tasks[i].arrival_time, tasks[i].burst_time);
        printf("\n");
    }
    printf("\n");

    pthread_t threads[input];

    if (option == 1) {
        printf("FCFS Scheduling:\n");
        for (int i = 0; i < input; i++) {
            pthread_create(&threads[i], NULL, FCFS, (void *)&tasks[i]);
        }
    } else if (option == 2) {
        printf("SRTF Scheduling:\n");
        for (int i = 0; i < input; i++) {
            pthread_create(&threads[i], NULL, SRTF, (void *)&tasks[i]);
        }
    } else if (option == 3) {
        printf("RR Scheduling:\n");
        for (int i = 0; i < input; i++) {
            pthread_create(&threads[i], NULL, RR, (void *)&tasks[i]);
        }
    } else if (option == 4) {
        printf("Preemptive PS Scheduling:\n");
        for (int i = 0; i < input; i++) {
            pthread_create(&threads[i], NULL, PS, (void *)&tasks[i]);
        }
    } else {
        printf("Invalid option selected.\n");
        free(tasks);
        return 1;
    }

    for (int i = 0; i < input; i++) {
        pthread_join(threads[i], NULL);
    }

    float avg_TAT = calculate_avg_TAT(tasks);
    printf("Avg Turnaround Time: %.2f\n", avg_TAT);

    float avg_WT = calculate_avg_WT(tasks);
    printf("Avg Waiting Time: %.2f\n", avg_WT);

    printf("\n");
    for(int i = 0; i < input; i++){
        printf("Task %d: %d %d\n", tasks[i].task_id, tasks[i].start_time, tasks[i].completion_time);
    }
    printf("\n");
    free(tasks);
    
    for (int i = 0; i < input; i++) {
        pthread_mutex_destroy(&mutex[i]);
    }

    for (int i = 0; i < 2; i++){
        pthread_cond_destroy(&cond[i]);
    }
    free(mutex);
    return 0;
}
