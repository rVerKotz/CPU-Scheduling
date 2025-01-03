#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int input, result, ctr = 0, cond_wait = 0, wait_count = 0, **newArr, *wait;
pthread_mutex_t *mutex;
pthread_cond_t *cond;

int compare(const void *a, const void *b)
{
    const int *arr1 = (const int *)a;
    const int *arr2 = (const int *)b;
    int sum1 = arr1[3] + arr1[4] + arr1[2];
    int sum2 = arr2[3] + arr2[4] + arr2[2];
    return sum1 - sum2;
}

int sort(const void *a, const void *b)
{
    const int *arr1 = *(const int **)a;
    const int *arr2 = *(const int **)b;
    return arr1[0] - arr2[0];
}

void signal()
{
    for (int j = 0; j < wait_count; j++)
    {
        pthread_cond_signal(&cond[wait[j]]);
    }
}

void *thread_func(void *arg)
{
    int *thread_data = (int *)arg;
    pthread_mutex_lock(&mutex[thread_data[0]]);
    for (int i = 0; i < 2; i++)
    {
        newArr[ctr][i] = thread_data[i];
    }
    for (int i = 2; i < 5; i++)
    {
        if (i == 2)
        {
            newArr[ctr][i] = thread_data[i];
            result += thread_data[i];
        }
        else
        {
            while (1)
            {
                if (thread_data[i] == 0)
                {
                    newArr[ctr][i] = result;
                    break;
                }
                if (result == thread_data[7])
                {
                    if (thread_data[3] + thread_data[4] > thread_data[8] + thread_data[9])
                    {
                        (result < thread_data[7]) ? (thread_data[7] -= result) : (thread_data[7] = 0);
                        newArr[ctr][7] = thread_data[7];
                        cond_wait = 1;
                        wait[wait_count] = thread_data[0];
                        wait_count++;
                        pthread_mutex_unlock(&mutex[thread_data[5]]);
                        pthread_cond_wait(&cond[thread_data[0]], &mutex[thread_data[0]]);
                        cond_wait = 0;
                    }
                    cond_wait = 0;
                }
                result++;
                thread_data[i]--;
            }
        }
    }
    if (ctr < input - 1)
    {
        (result < thread_data[7]) ? (thread_data[7] -= result) : (thread_data[7] = 0);
        newArr[ctr][7] = thread_data[7];
        if (cond_wait == 0)
        {
            pthread_mutex_unlock(&mutex[thread_data[5]]);
        }
        else
        {
            signal();
        }
        ctr++;
    }
    pthread_exit(NULL);
}

int main(void)
{
    printf("How many times: ");
    scanf("%d", &input);

    int arr[input][5];
    for (int i = 0; i < input; i++)
    {
        printf("Task %d: ", i + 1);
        arr[i][0] = i;
        for (int j = 1; j < 5; j++)
        {
            scanf("%d", &arr[i][j]);
        }
    }
    printf("\n");
    int rows = sizeof(arr) / sizeof(arr[0]);
    qsort(arr, rows, sizeof(arr[0]), compare);

    newArr = malloc(input * sizeof(int *));
    for (int i = 0; i < input; i++)
    {
        newArr[i] = malloc(5 * sizeof(int));
    }

    wait = (int *)malloc(input * sizeof(int));
    if (wait == NULL)
    {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    mutex = malloc(input * sizeof(pthread_mutex_t));
    for (int i = 0; i < input; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
        pthread_mutex_lock(&mutex[i]);
    }

    cond = malloc(input * sizeof(pthread_cond_t));
    for (int i = 0; i < input; i++)
    {
        pthread_cond_init(&cond[i], NULL);
    }

    pthread_t thread_ids[input];
    pthread_mutex_unlock(&mutex[arr[0][0]]);
    for (int i = 0; i < input; i++)
    {
        pthread_create(&thread_ids[i], NULL, thread_func, (void *)arr[i]);
    }

    for (int i = 0; i < input; i++)
    {
        pthread_join(thread_ids[i], NULL);
    }

    printf("\n");
    for (int i = 0; i < input; i++)
    {
        for (int j = 1; j < 5; j++)
        {
            if (i > 0 && (j == 2 || j == 3))
            {
                newArr[i][j] = newArr[i - 1][4];
            }
        }
    }

    qsort(newArr, rows, sizeof(int *), sort);
    for (int i = 0; i < input; i++)
    {
        printf("Thread %d: ", newArr[i][0] + 1);
        for (int j = 1; j < 5; j++)
        {
            printf("%d ", newArr[i][j]);
        }
        printf("\n");
    }

    float AWT, ART, ATAT;
    for (int i = 0; i < input; i++)
    {
        AWT += newArr[i][2];
        ART += newArr[i][2] - arr[i][2];
        ATAT += newArr[i][4] - arr[i][2];
    }
    ART /= input;
    AWT /= input;
    ATAT /= input;
    printf("\nAverage = %.2f %.2f %.2f \n", AWT, ART, ATAT);

    for (int i = 0; i < input; i++)
    {
        pthread_mutex_destroy(&mutex[i]);
        pthread_cond_destroy(&cond[i]);
    }

    return 0;
}