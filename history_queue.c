/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "history_queue.h"

//Local Variables
static HistoryEntry queue[MAXHIST];

static int queue_start = 0;

static int queue_next = 0;

static int queue_size = 0;
//terminating string
char terminate[] = " ";
/**
 * adds a HistoryEntry struct into the circular queue as well as handles the
 * queue
 *
 * @param entry		The given historical entry to be added
 */
void add_queue(HistoryEntry entry) {
	//First checks if start of queue is empty
	if (queue_size == 0){
		queue[queue_start] = entry;
		queue_next = queue_start + 1;
		queue_size ++;
	}
	//If queue is not full, add entry to queue_next index
	else if (queue_size < MAXHIST){
		queue[queue_next] = entry;
		queue_next ++;
		queue_size ++;
	}
	//Past this point the queue is full
	//so check if you loop around the circqueue and update start and next
	//accordingly after adding entry
	else {
		if (queue_next >= MAXHIST){
			queue_next = 0;
			queue[queue_next] = entry;
		}
		if (queue_next == queue_start){
			queue_start = (queue_start+1) % MAXHIST;
			queue[queue_next] = entry;
		}
		queue_next ++;
	}
}

/**
 * Prints out the given circular queue
 *
 */
void print_queue() {
	printf("history\n");
	int i;
	int index = queue_start;
	for (i = 0; i < queue_size; i++){
		if(index == MAXHIST){ 
			index = 0;
		}
		printf("\t%d\t%s\n", queue[index].cmd_num, queue[index].cmdline);
		index ++;
	}
	printf("\n\n");
}

/**
 * Given a command line number, searches queue if it is in it and returns the
 * given command line non-parsed
 *
 * @param num		The command line number to search for
 * @return 			The non-parsed command line
 */
char *giveNumberCommand(unsigned int num) {
	int i; 
	//first check if given id num is smaller then the queue start num or greater then queue end 
	//or the id num given is 0 
	//since we start at line 1
	//printf("queue_size: %d\n", queue_size);
	//printf("Number at beginning of queue: %d\n", queue[queue_start].cmd_num);
	//printf("number of end of queue: %d\n", queue[(queue_start + queue_size -1) % queue_size].cmd_num); 
	if (num > queue[(queue_start + queue_size -1) % queue_size].cmd_num 
			|| num < queue[queue_start].cmd_num) {
		printf("Given command line number: %d not valid!\n", num);
		return terminate;
	}
	else {
		for (i = 0; i < queue_size; i++) {
			if (num == queue[i].cmd_num) {
		   		break;
			}
		}
		//printf("Corresponding Command Line: %s\n", queue[i].cmdline);
		return queue[i].cmdline;
	}
}
