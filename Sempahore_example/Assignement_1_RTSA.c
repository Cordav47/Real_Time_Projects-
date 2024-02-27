/*
 ============================================================================
 Name        : Assignement1.c
 Author      : Davide Corroppoli
 Version     :
 Copyright   :
 Description : Semaphore assignment
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#define FOREVER while(!time_up)
#define N_THREADS 19
#define GROUP_SIZE 5

/* Refill and mingle time are the max time could a person take to fill the glass or drink */
#define REFILL_TIME 5
#define MINGLE_TIME 10

#define END_OF_TIME 120


typedef char name_t[20];
typedef enum {FALSE, TRUE} boolean;


time_t big_bang;
boolean time_up=FALSE;

// GLOBAL VARS, TYPE DEFS ETC
int contatore[N_THREADS];
char group[GROUP_SIZE][20]; //Buffer of names representing a group

char drink_group[GROUP_SIZE][20]; //Group that drink, if it's correct should be the same of group per each group

boolean flag[N_THREADS];

int group_index = 0;

int group_counter = 1; //Count how many groups are formed

int people_drinking = 0;

// The array which contain the thread_name of who drunk the most and the least
char maxs[N_THREADS][20];
char mins[N_THREADS][20];


// Semaphores
sem_t sem_group; /*Initialized to five, when the thread join the group they "wait" until of course the value it's zero
so other thread cannot join the group anymore, once the group has toasted a signl is given and the formation of a new group is again
allowed*/
sem_t sem_forward; //As soon as the group it's formed are given 5 signals that the threads of the group can go ahead
sem_t sem_drink; //Similar to sem_group, initialized to 5, when the thread is ready to drink there's a wait
sem_t sem_forward2; //Similar to sem_forward, when the group is ready to drink, they drink together

void wait_for_toasting(char *thread_name) {
	// YOUR CODE HERE
	int id=((char *)thread_name)[0]-'A';
	sem_wait(&sem_group); //if Thread pass this sem then is part of the group
	flag[id] = TRUE;
	if (flag[id] == TRUE) {
		strcpy(group[group_index], thread_name); //Thread_name enter in the buffer representing the group
		group_index++; //Group index is incremented
	}
	if (group_index == 5) { //Group is completed

		printf("\nTime:[%4.0f s], the group number %d is formed by %s, %s, %s, %s and %s\n", difftime(time(NULL),big_bang), group_counter, group[0], group[1], group[2], group[3], group[4]);
		group_counter++; // The number of total groups is incrementes
		for (int i = 0; i < GROUP_SIZE; i++) {
					sem_post(&sem_forward); //Are generated five signals to allow the thread of the group to go ahead
				}
		group_index = 0; // The index turn back to zero for the formation of a new group
	}
	flag[id]=FALSE;
	sem_wait(&sem_forward); //If the group is formed the thread can go ahead
}

void wait_for_drinking(char *thread_name) {
	// YOUR CODE HERE
	int id=((char *)thread_name)[0]-'A';
	//Next 2 lines analog to group in wait_for_toasting
	/* a mechanism such as flag[id] == TRUE is not included because here there are people from a group already formed
	 that just proceed doing their stuff*/
	sem_wait(&sem_drink);
	strcpy(drink_group[people_drinking], thread_name);


	people_drinking++; //Counter of drinking is incremented
    if (people_drinking == 5){ //The whole group is arrived
    	printf("\nTime:[%4.0f s], %s, %s, %s, %s, %s are going to drink", difftime(time(NULL),big_bang), drink_group[0], drink_group[1], drink_group[2], drink_group[3], drink_group[4]);
    	for (int i = 0; i < GROUP_SIZE; i++) {
    				sem_post(&sem_forward2); //They can start drinking together
    			}
    	people_drinking = 0; //Counter go back to zero
    }
    sem_wait(&sem_forward2); //They go ahead and drink

    /*sem_wait(forward) must be given before the sem post because otherwise other threads have authorization
    to drink and forming a new group (that actually have, but we wanna make things more complicated and tightly synchronized)
    and going ahead could increment people_drinking, people_drinking would become > 5, we couldn't manage the group and all the system would stuck
    */
    sem_post(&sem_group); /* forming a new group now is allowed again */
    sem_post(&sem_drink); // Here is not forbidden that 2 groups drink almost simultaneously so the sem_drink signal is given after the wait, in the same funcion

}

void initialize() {
	// YOUR CODE HERE
	time(&big_bang);
	//All semaphores are initialized
	sem_init(&sem_group, 0, 5);
	sem_init(&sem_drink, 0, 5);
	sem_init(&sem_forward, 0, 0);
	sem_init(&sem_forward2, 0, 0);
}

void drink(char *thread_name, char *action_name, int max_delay) {
	// you can use thread_name and action_name if you wish to display some output
	int delay=rand()%max_delay+5;
	// Drinking operation is fulfilled in a random time between 5 and 10 seconds
	sleep(delay);

	printf("\n%s has drunk", thread_name);
}

void refill(char *thread_name, char *action_name, int max_delay) {
	// you can use thread_name and action_name if you wish to display some output
	int delay=rand()%max_delay+2;
	// Fill operation is fulfilled in a random time between 2 and 5 seconds
	sleep(delay);

	printf("\n%s has filled the glass", thread_name);
}


void *buddy(void *thread_name) {
	int id=((char *)thread_name)[0]-'A';
	// LOCAL VARS ETC
	printf("%s joined the party, id = %d\n", (char*) thread_name, id);
	FOREVER {
		refill(thread_name, "go refill glass", REFILL_TIME);
		wait_for_toasting(thread_name);
		if(!time_up)
			printf("\t%s: Skol!\t", (char*) thread_name);
		wait_for_drinking(thread_name);
		if(!time_up) {
			printf("\t%s drinks\t", (char*) thread_name);
			drink(thread_name, "drink and mingle", MINGLE_TIME);
			// UPDATE STATISTICS
			contatore[id]++; //The counter that counts how many times has a thread "drunk" is incremented

		}
	}
	printf("\n%s left the party", (char*) thread_name);
	pthread_exit(NULL);
}


int main(void) {
	int i;
	pthread_t tid[N_THREADS];
	name_t thread_name[N_THREADS] = {"Ali","Burhan","Cristina","Daniele",
	"Enrica","Filippo","Girish","Heidi","Ivan","Jereney","Kathy","Luca",
	"Mehran", "Nick", "Orazio", "Prem", "Quentin", "Reza", "Saad"};


	initialize();

	puts("\nWELCOME BUDDIES\n");

	for(i=0;i<N_THREADS;i++) {
		pthread_create(&tid[i],NULL,buddy,thread_name[i]);
		//Thread are created and they run
	}

	sleep(END_OF_TIME); //After END_OF_TIME time is over and time_up become true
	time_up=TRUE;

	// UNLOCK WAITING THREADS?
	for(i=0;i<N_THREADS-1;i++) {
            /* The signal that allow the buddies to terminate their thread even if they haven't formed a group, there are
             * atmost 4 threads waiting (N_THREADS - 1) because if they were 5 they would have formed a group
             */

			sem_post(&sem_forward);
			sem_post(&sem_forward2);
			/* The semaphores drink and group instead are ok because each thread when it's possible to build a new group or
			when it's ready to drink give the signal that another thread can fulfill the action*/
	}

	//Variables used in statistics
	int c, max, min;
	char mostdrunk[20], lessdrunk[20];
	int indmax = 0;
    int indmin = 0;

	for(i=0;i<N_THREADS;i++) {
		pthread_join(tid[i], NULL);
        // Wait for all threads to terminate
	}
	// Party statistics
	printf("\nPARTY STATISTICS:");
	for(i=0;i<N_THREADS;i++) {
		//Examinate all threads' statistics singularly
		printf("\n %s has drunk %d glasses", thread_name[i], contatore[i]);
	    c = contatore[i];

		if (i == 0) {
				max = c;
				strcpy(mostdrunk, thread_name[i]);
				min = c;
				strcpy(lessdrunk, thread_name[i]);
				strcpy(maxs[indmax], thread_name[i]);
				strcpy(mins[indmin], thread_name[i]);
			}
		if (i >=1) {
			if (c == max) { //if it's equal to max enter in max's vector
					indmax++;
					strcpy(maxs[indmax], thread_name[i]);

				}
			if (c == min) { //if it's equal to min enter in min's vector
					indmin++;
					strcpy(mins[indmin], thread_name[i]);

				}
			if (c > max) {  //if it's bigger than max define a new max
				max = c;
				indmax = 0;
				strcpy(mostdrunk, thread_name[i]);
				strcpy(maxs[indmax], thread_name[i]);

			    }
			if (c < min) {  //if it's lower than min define a new min
				min = c;
				indmin = 0;
				strcpy(lessdrunk, thread_name[i]);
				strcpy(mins[indmin], thread_name[i]);

			    }

			}
	}

    if (indmin == 0) { // Means that there's only one min
    	printf("\n- The most sober participant was %s, had only %d glasses", lessdrunk, min);
    }
    else { // More than one person have drunk the least number of glasses
    	printf("\n- There were more than one people that drink just %d glasses, they were: ", min);
    	for (i = 0; i <= indmin; i++) {
    	    printf("%s,\t", mins[i]);
    	}
    }
    if (indmax == 0) { // Means that there's only one max
        	printf("\n- The thirstiest participant was %s, who had %d glasses", mostdrunk,  max);
        }
    else { // More persons that drunk the most
        printf("\n- There were more than one people that drink %d glasses, they were: ", max);
       	for (i = 0; i <= indmax; i++) {
        	printf("%s,\t", maxs[i]);
        	}
        }

	puts("\nGOODNIGHT BUDDIES\n");
	return EXIT_SUCCESS;
}
