/**
 * @Project  :   concurrente_tp3
 * @File     :   main.c
 * @Revision :   1.0.1
 * @Created  :   30/12/2015 11:45
 * @Author   :   Frederick NEY's WEBMASTER
 */

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/**
 * list of global variable
 */
/// array to hold integers
int *ARRAY;
int LENGHT;
int ALL_DONE = 0;
const char *USAGE =
        "usage:\n"
                "%s <size of the array> <number of threads>\n";
/// structure to be passed to thread function
struct bubble_data {
    int start; /// start index of array
    int end; /// end index of the array
    int id; /// id of each thread
};
/// an array of mutext variables for adjacent threads
pthread_mutex_t **MUTEX;

/**
 * List of all functions used.
 */
void display_usage(char * cmd_name);
int sorted_array_checker(int lenght);
void random_array(int lenght);
void print_array(int lenght);
void sorting(int *tab, int lenght);
void *bubble_sort(void *arg);

/**
 * function for displaying usage
 * @param string cmd_name its the name of the program
 */
void display_usage (char *cmd_name) {
    printf(USAGE, cmd_name);
}

/**
 * The check if array is sorted.
 *
 * function to check if array is properly sorted
 * it returns 1 on success and 0 on failure
 *
 * @param int length - lenght of the array (of type int)
 * @return 1 if all went well 0 on error, array is not sorted.
 */
int sorted_array_checker(int lenght) {
    /// for loop detecting if the array is sorted
    for(int i = 0;i < lenght-1; i++) {
        if(ARRAY[i] > ARRAY[i+1]) {
            return 0;
            ALL_DONE = 0;
        }
    }
    /// the sort is finished
    ALL_DONE = 1;
    return 1;
}

/**
 * Function which auto generate the array
 *
 * @param int length - lenght of the array
 *
 */
void random_array(int lenght) {
    int i;
    srand(time(NULL)); // initialising the random
    for(i = 0;i <lenght ; i++)	{
        ARRAY[i] = rand()%1000; // generating random value for the array
    }
}


/**
 * Function printing the array
 *
 * @param int length - lenght of the array
 *
 */
void print_array(int lenght) {
    int i;
    for(i =0 ; i < lenght; i++)
        printf("%d ",ARRAY[i]);
    printf("\n");
}


/**
 * Function of bubble sort.
 *
 * @param int *tab -  pointer to the array
 * @param int length - lenght of the array
 *
 */
void sorting(int *tab, int lenght) {
    int i,j;
    for ( i = lenght-1; i > 0; i--) {
        for ( j = 0; j < i; j++) {
            if (tab[j] > tab[i]) {
                int tmp = tab[j];
                tab[j] = tab[i];
                tab[i] = tmp;
            }
        }
    }
}

/**
 * Thread function to do the sorting.
 *
 * @param void *argv - the bubble_data struct
 *
 */
void *bubble_sort(void *argv) {
    struct bubble_data *bubble_sort_data = (struct bubble_data*)argv;

    /// undelimited loop
    while(!ALL_DONE) {
        ///attributing range for threads
        if(bubble_sort_data->start == 0) {
            int lenght = bubble_sort_data->end - bubble_sort_data->start + 1;
            ///sorting two case of the array for the first range
            sorting(ARRAY+bubble_sort_data->start,lenght);
        }
        else {
            int lenght = bubble_sort_data->end - bubble_sort_data->start;
            ///sorting two case of the array for all non first range
            sorting(ARRAY+bubble_sort_data->start+1,lenght); 
        }

        /// locking access to adjacent case
        pthread_mutex_lock(MUTEX[bubble_sort_data->id]);
        pthread_mutex_lock(MUTEX[bubble_sort_data->id + 1]);

        /// checking if array is sorted
        sorted_array_checker(LENGHT);

        if(bubble_sort_data->end != LENGHT-1)
            sorting(ARRAY+bubble_sort_data->end,2);

        /// unlocking access to adjacent case
        pthread_mutex_unlock(MUTEX[bubble_sort_data->id]);
        pthread_mutex_unlock(MUTEX[bubble_sort_data->id + 1]);

    }
    pthread_exit(NULL);
}

/**
 * main program.
 *
 * @param int *argc - number of total arguments.
 * @param char **argv - values to input.
 *
 * @return int EXIT_SUCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[]) {
    if(argc != 3) {
        display_usage(argv[0]);
        return EXIT_FAILURE;
    }
    LENGHT = atoi(argv[1]);
    int nb_thread = atoi(argv[2]);
    // allocating the pthread_mutex_t array pointer
    ARRAY = malloc(sizeof(int) * LENGHT);
    MUTEX = malloc(sizeof(pthread_mutex_t *) * nb_thread + 1);
    for (int i = 0; i <= nb_thread; i++)
        MUTEX[i] = malloc(sizeof(pthread_mutex_t));
    /// generating random number for tha array
    random_array(LENGHT);
    /// printing the array
    printf("The auto generated array: \n");
    print_array(LENGHT);
    printf("Starting sort...\n");
    /// initialising threads
    pthread_t threads[nb_thread];
    /// thread attribute
    pthread_attr_t attr;

    /// creating mutex
    for(int i = 0;i <= nb_thread; i++) {
        pthread_mutex_init(MUTEX[i],NULL);
    }
    int part = LENGHT/nb_thread;
    int start = 0;
    int end = start + part;

    /// initialising thread attribute
    pthread_attr_init(&attr);
    for(int i = 0; i < nb_thread; i++) {
        /// allocating struct
        struct bubble_data *bubble_sort_data = malloc(sizeof(struct bubble_data));
        /// generating value for struct
        bubble_sort_data->id = i;
        bubble_sort_data->start = start;
        bubble_sort_data->end = end;
        printf("Tread %d range is from %d to %d\n",bubble_sort_data->id, bubble_sort_data->start, bubble_sort_data->end);
        /// creating threads
        if ( (pthread_create(&threads[i],&attr,bubble_sort,(void *)bubble_sort_data)) == 0) {
            if(i != nb_thread - 2) {
                start = end;
                end += part;
            }
            else {
                start = end ;
                end  = LENGHT - 1;
            }
        }
        else {
            perror("thread creation :");
            return EXIT_FAILURE;
        }
    }

    /// waiting for all thread finished
    for (int i = 0; i < nb_thread; i++) {
        pthread_join(threads[i], NULL);
    }
    if(sorted_array_checker(LENGHT)) {
        /// printing the array sorted
        printf("Ending sort...\n");
        printf("The sorted array:\n");
        print_array(LENGHT);
    }
    else {
        printf("Array not sorted...\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


