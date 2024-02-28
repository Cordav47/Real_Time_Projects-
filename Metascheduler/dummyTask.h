/*
 * dummyTask.h
 *
 *  Author: Davide Corroppoli

#ifndef DUMMYTASK_H_
#define DUMMYTASK_H_

/* --> set this to LAB2 if in Lab2 <-- */
#define DTD

/* dummy task parameters, do not change them */
#define LOWER_LIMIT 1000
#define UPPER_LIMIT 2000
#define UP 1
#define DOWN 0

/* approximately OPSMSEC cycles correspond to 1 msec computation on target architecture*/
#ifdef MACBOOK
#define OPSMSEC 446000
#else
#ifdef IMAC
#define OPSMSEC 730000
#else
#ifdef DTD /* DeskTop Davide */
#define OPSMSEC 1300000
#endif
#endif
#endif

/* only for event tracing purposes */
void all_done(void);

/* to simulate actual computation: keeps CPU busy for n cycles */
void get_busy(int n);

/* Robotic Application */
/*struct trash {
	Size s;
	Kind k;
} t;*/
int beltk[2][2];
int belts[2][2];

int availability[5]; /*Computer vision doesn't care about availability but I don't want to modify the index*/
int bigt, smallt, processed_trash;
int plastic_trash, card_trash, glass_trash;

void get_belt(int task);
void pick_trash(int task);
void trajectory(int size,int far); /* Far is an int that is 1 for the closest bucket, 2 for the medium and 3 fr the furthest*/
void throw();
void return_home(int size, int far);
int visionk();
int visions();





#endif /* DUMMYTASK_H_ */
