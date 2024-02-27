/*
 ============================================================================
 Name        : Assignement_2.c
 Author      : Davide Corroppoli
 Version     :
 Copyright   : Your copyright notice
 Description : Bounded buffer assignement
 ============================================================================
 */



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

// CONSTANTS AND MACROS

#define N_THREADS 13
#define N 5
// #define END_OF_TIME 30
#define FOREVER while(!time_up)
#define MAX 100 // We define the max number we get for simplicity
// DEFINITIONS OF NEW DATA TYPES

typedef char thread_name_t[10];

typedef enum {LOW, HIGH} lowhigh_t;
typedef enum {FALSE, TRUE} boolean;

typedef struct {
	int value;
	lowhigh_t p;
	boolean full;
}
value_t;

// monitor also defined as a new data type
typedef struct {
    value_t buffer[N];
    int in, out; // indexes for upload/download operations
    int n_low, n_high; // number of full slots of each type
    // monitor lock (one mutex lock per monitor)
    pthread_mutex_t m;


    pthread_cond_t downloadable_high;
    // downloadable high is true if it's uploaded one value high (n_high become 1) to a buffer with no high values

    pthread_cond_t downloadable_low;
    // downloadable low is if it's uploaded one value low (n_low become 1, n_high is 0) to an empty buffer

    pthread_cond_t add2_buffer;
	// If the buffer is full with 5 elements, the signal is given when I download a value
    //so I free spot and uploading is again available


} monitor_t;

// GLOBAL VARIABLES
// the monitor should be defined as a global variable
monitor_t mon;

boolean time_up=FALSE; //Become true if time is over




//  MONITOR API
void upload(monitor_t *mon, char* name, int value, lowhigh_t p);
int download(monitor_t *mon, char* name, lowhigh_t t);
void monitor_init(monitor_t *mon);
void monitor_destroy(monitor_t *mon);

// OTHER FUNCTION DECLARATIONS
// functions corresponding to thread entry points
void *producer(void *arg);
void *consumer(void *arg);
// void *final(void *arg);

// spend_some_time could be useful to waste an unknown amount of CPU cycles, up to a given top
double spend_some_time(int);
// simulate_action_* to be replaced by function names according to application: e.g., pick_up, put_down, ...
int produce(char* name) { return rand()%MAX; }
void consume(char* name, int value, monitor_t *mon) {
     spend_some_time(15);
        }

void copy_to_buffer(monitor_t *mon, int value, lowhigh_t p);
int copy_from_buffer(monitor_t *mon, lowhigh_t t);
lowhigh_t f(int value);


// IMPLEMENTATION OF MONITOR API
void upload(monitor_t *mon, char *name, int value, lowhigh_t p) {
    pthread_mutex_lock(&mon->m);

    // YOUR CODE HERE
    //Upload the value only if the buffer is not full
    while ((mon->n_high+mon->n_low) >= 5) {
    	pthread_cond_wait(&mon->add2_buffer, &mon->m);
    }


    // now it can upload values to the buffer, it uploads in the first slot available

    for (int i = 0; i < N; i++) {
    	if (mon->buffer[i].full == FALSE) {
    		mon->in  = i; //The indexes where to perform the upload
    		break;
    	}
    }
    copy_to_buffer(mon, value, p); //upload the value into the buffer in the slot in

    //After the upload I have to wake up suspended threads due to the lack of downloadable items
    if (p == HIGH && mon->n_high == 1) { //if there weren't downloadable high values (n_high = 0), now one has been added
    	pthread_cond_signal(&mon->downloadable_high);
    }

    if (p == LOW && mon->n_low == 1 && mon->n_high == 0) { //if there weren't downloadable low values (n_low = 0), now one has been added
    	// Condition n_high = 0 is added because low can download also high values, in fact thread was stopped for n_high = 0 and n_low = 0
    	pthread_cond_signal(&mon->downloadable_low);
    }
    pthread_mutex_unlock(&mon->m); //lock is released
    printf("Value: %d (%s) uploaded in position %d by producer %s\n", value, p==LOW?"LOW":"HIGH", (mon->in+1), name);
    printf("Now the buffer is:\t"); //Print the actual state of the buffer
    for (int i = 0; i < N; i++) {
        	if (mon->buffer[i].full == TRUE) {
        		printf("%d: %d\t", (i+1), mon->buffer[i].value);
        	}
    }
    printf("\n");

}

int download(monitor_t *mon, char *name, lowhigh_t p) {
	int retval;
	int i;
    pthread_mutex_lock(&mon->m);

    // YOUR CODE HERE
    if (p == HIGH) {
    	// Can take only high
        while (mon->n_high == 0 && !time_up) {
    	    pthread_cond_wait(&mon->downloadable_high, &mon->m);
    	    // if there are no downloadable values, thread has to wait

        }

    	for (i = 0; i< N; i++) {
    		if (mon->buffer[i].full == TRUE && mon->buffer[i].p == HIGH) {
    			mon->out = i; //Indexes where to perform the download
    			break;
    		}
    	}
    	retval = copy_from_buffer(mon, HIGH); //Return the value selected in position out

    }
    else {
    	// if p == low Can take both high and low
    	while ((mon->n_low == 0) && (mon->n_high == 0) && !time_up) { //If there are no available values it has to wait
    	    pthread_cond_wait(&mon->downloadable_low, &mon->m);

    	}

    	for (i = 0; i< N; i++) {
    	    	if (mon->buffer[i].full == TRUE) { //Given that both high and low values are ok, it's enough that the buffer[i] is full
    	    		mon->out = i;
    	    		break;
    	    	}
        }
    	retval = copy_from_buffer(mon, LOW); //Return the value selected in position out
    }
    //Awake a thread suspended due to the full buffer
    if ((mon->n_high+mon->n_low)==4)  { /*If the buffer was full and I download an item means that now there are 4 items
        in fact we are still inside the locked zone*/
    	pthread_cond_signal(&mon->add2_buffer);
    }

    pthread_mutex_unlock(&mon->m);

    //Print the actual state of the buffer
    printf("Now the buffer is:\t");
    for (int i = 0; i < N; i++) {
        if (mon->buffer[i].full == TRUE) {
             printf("%d: %d\t", (i+1), mon->buffer[i].value);
        }
    }
    printf("\n");
    return retval;
}

void monitor_init(monitor_t *mon) {
    // YOUR CODE HERE
    // set initial value of monitor data structures, state variables, mutexes, counters, etc.
    // typically can use default attributes for monitor mutex and condvars
	int i;
    for(i=0;i<N;i++)
    	mon->buffer[i].full=FALSE;
    pthread_mutex_init(&mon->m,NULL);
    mon->in=0;
    mon->out=0;

    // set all condvar
    pthread_cond_init(&mon->downloadable_high, NULL);
    pthread_cond_init(&mon->downloadable_low, NULL);
    pthread_cond_init(&mon->add2_buffer, NULL);

}

void monitor_destroy(monitor_t *mon) {
    // YOUR CODE HERE
    pthread_cond_destroy(&mon->downloadable_high);
    pthread_cond_destroy(&mon->downloadable_low);
    pthread_cond_destroy(&mon->add2_buffer);

    pthread_mutex_destroy(&mon->m);
}

// MAIN FUNCTION
int main(void) {
    // thread management data structures
    pthread_t my_threads[N_THREADS];
    thread_name_t my_thread_names[N_THREADS];
    int i;

    // initialize monitor data structure before creating the threads
    monitor_init(&mon);

    for (i=0;i<6;i++) { //Six producers
        sprintf(my_thread_names[i],"p%d",i);
        // create N_THREADS thread with same entry point
        // these threads are distinguishable thanks to their argument (their name: "t1", "t2", ...)
        // thread names can also be used inside threads to show output messages
        pthread_create(&my_threads[i], NULL, producer, my_thread_names[i]);
    }
    for (;i<N_THREADS;i++) { //Six consumers
        sprintf(my_thread_names[i],"c%d",i);
        // create N_THREADS thread with same entry point
        // these threads are distinguishable thanks to their argument (their name: "t1", "t2", ...)
        // thread names can also be used inside threads to show output messages
        pthread_create(&my_threads[i], NULL, consumer, my_thread_names[i]);
    }
     //sprintf(my_thread_names[12],"final %d",12);
    // pthread_create(&my_threads[12], NULL, final, my_thread_names[12]);


    /* Could be put a sleep(Time) to make the threads last Time seconds, defining forever as
     * while !time_up and putting time_up = TRUE after the sleep. Then must be signaled the threads
     * waiting on a condvar and let them exit the thread by putting if (time_up) pthread_exit(NULL)
     * immediately after the condvar
     */
    for (i=0;i<N_THREADS;i++) {

        pthread_join(my_threads[i], NULL);

    }

    // free OS resources occupied by the monitor after creating the threads
    monitor_destroy(&mon);

    return EXIT_SUCCESS;
}

void *producer(void *arg) {
	char *name=arg;
    int value;
    lowhigh_t p;
    FOREVER {

        value = produce(name); //value generated randomly between 0 and 100
        p = f(value); //Say if the value generated randomly it's low or high
        printf("producer %s: uploading value %d (%s)\n", name, value, p==LOW?"LOW":"HIGH");
        upload(&mon, name, value, p); //Value uploaded
        sleep(rand()%N+2); // After uploading, the producer rest for a random time between 0 and 5 seconds

    }
    printf("producer %s has done\n", name);

    pthread_exit(NULL);
}

void *consumer(void *arg) {
	char *name=arg;
    int value;
    lowhigh_t threshold;
    threshold=rand()%2; //Generates randomly the threshold
    printf("I am consumer %s (%s)\n", name, threshold==LOW?"LOW":"HIGH");
    FOREVER {

        value = download(&mon, name, threshold); //The value is downloaded
        printf("consumer %s: downloaded value %d (%s)\n", name, value, threshold==LOW?"LOW":"HIGH");
        consume(name,value, &mon);
        sleep(rand()%N+2); //Thread rest for a random time between 0 and 5 after downloading
    }
    printf("consumer %s has done\n", name);

    pthread_exit(NULL);
}

/*void *final(void*arg){
	char *name = arg;
	sleep(END_OF_TIME);
	sleep(1);
	int i;
	while (time_up == TRUE){
		for (i = 0; i < 4; i++){

		pthread_cond_signal(&mon.add2_buffer);

		sleep(1);
		 // hold the lock as suggested in the slides
		pthread_cond_signal(&mon.downloadable_low);

		sleep(1);
		 // hold the lock as suggested in the slides
		pthread_cond_signal(&mon.downloadable_high);

		sleep(2);

	}
		pthread_exit(NULL);
	}
	pthread_exit(NULL);
}*/

// AUXILIARY FUNCTIONS
double spend_some_time(int max_steps) {
    double x, sum=0.0, step;
    long i, N_STEPS=rand()%(max_steps*1000000);
    step = 1/(double)N_STEPS;
    for(i=0; i<N_STEPS; i++) {
        x = (i+0.5)*step;
        sum+=4.0/(1.0+x*x);
    }
    return step*sum;
}

lowhigh_t f(int value) {
	lowhigh_t p=(lowhigh_t) rand()%2;
	//printf("f(%d)=%s\n", value, p==LOW?"LOW":"HIGH");
	return p;
}

void copy_to_buffer(monitor_t *mon, int value, lowhigh_t p) {
	int count=0;
	while(mon->buffer[mon->in].full) {
		mon->in=(mon->in+1)%N;
		if(++count>N) {
			perror("buffer is inconsistent");
			abort();
		}
	}
	/*if ((mon->n_low + mon->n_high) >= 5) {
		perror("buffer is inconsistent");
		abort();
	}*/
	mon->buffer[mon->in].value=value;
	mon->buffer[mon->in].p=p;
	mon->buffer[mon->in].full=TRUE;
	if(p==HIGH)
		mon->n_high++;
	else mon->n_low++;
}

int copy_from_buffer(monitor_t *mon, lowhigh_t t) {
	int retval, count=0;
	while((mon->buffer[mon->out].full==FALSE)||(mon->buffer[mon->out].p<t)) {
		mon->out=(mon->out+1)%N;
        if(++count>N) {
            perror("buffer is inconsistent");
            abort();
        }
    }
	retval=mon->buffer[mon->out].value;
	mon->buffer[mon->out].full=FALSE;
	if(mon->buffer[mon->out].p==HIGH)
		mon->n_high--;
	else mon->n_low--;

	return retval;
}


