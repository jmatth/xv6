#include "user.h"
#include "mypthread.h"

#define NULL 0x0

int *arr;
volatile short int sorted = 0;
pthread_t *threads;
pthread_mutex_t *mutexes;

void* swap(void *ptrindex)
{
    int tmp;
    int index = (int)ptrindex;

    int *left = &(arr[index]);
    int *right = &(arr[index + 1]);

    pthread_mutex_t *leftm = &mutexes[index];
    pthread_mutex_t *rightm = &mutexes[index+1];

    while(!sorted)
    {
        pthread_mutex_lock(leftm);
        pthread_mutex_lock(rightm);

        if (*left < *right)
        {
            tmp = *left;
            *left = *right;
            *right = tmp;
        }

        pthread_mutex_unlock(rightm);
        pthread_mutex_unlock(leftm);

        pthread_yield();
    }

    pthread_exit(NULL);

    return NULL;
}

int main(int argc, char *argv[])
{
    int i;
    int nelems = argc - 1;
    arr = malloc(sizeof(int) * nelems);

    threads = malloc(sizeof(pthread_t) * (nelems - 1));
    mutexes = malloc(sizeof(pthread_mutex_t) * nelems);

    if (argc <= 1)
    {
        exit();
    } else if (argc == 2) {
        printf(1, "%s\n", argv[1]);
        exit();
    }

    for (i = 0; i < argc - 1; ++i)
    {
        arr[i] = atoi(argv[i + 1]);
        pthread_mutex_init(&(mutexes[i]), NULL);
    }

    for (i = 0; i < nelems - 1; ++i)
    {
        pthread_create(&(threads[i]), NULL, &swap, (void*)i);
    }

    i = 0;
    while (i < nelems - 1)
    {
        pthread_mutex_lock(&mutexes[i]);
        pthread_mutex_lock(&mutexes[i+1]);

        if (arr[i] < arr[i+1])
        {
            pthread_mutex_unlock(&mutexes[i+1]);
            pthread_mutex_unlock(&mutexes[i]);
            // Start over if it's still not sorted.
            i = 0;
        }
        else
        {
            pthread_mutex_unlock(&mutexes[i+1]);
            pthread_mutex_unlock(&mutexes[i]);
            i++;
        }
    }

    // Checked the whole array and it's good, set the sorted flag.
    sorted = 1;

    void* retval;
    for (i = 0; i < nelems - 1; ++i)
    {
        pthread_join((threads[i]), &retval);
    }

    for (i = 0; i < nelems; ++i)
    {
        printf(1, "%d ", arr[i]);
    }
    printf(1, "\n");

    free(arr);
    free(threads);
    free(mutexes);

    exit();
}
