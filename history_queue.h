/*
 * Header file for circular queue of HistoryListItems.
 */
#ifndef __HISTQUEUE_H__
#define __HISTQUEUE_H__

#include "parse_args.h"

#define MAXHIST 10   // max number of commands in history list

/*
 * A struct to keep information one command in the history of 
 * command executed
 */
struct HistoryEntry {              
    unsigned int cmd_num;
    char cmdline[MAXLINE]; // command line for this process
};

// You can use "HistoryEntry" instead of "struct HistoryEntry"
typedef struct HistoryEntry HistoryEntry;


// TODO: fill in prototypes like you did for circqueue.h in lab session 10

/**
 * adds given value to the circular queue.
 * @param entry, the value to be added to the queue.
 * 
 */
void add_queue(HistoryEntry entry);

/**
 *prints out the history queue
 *
 */
void print_queue();

/**
 * gives the specified command line by given cmndline number the given command line in 
 * history specified by !
 *
 * @param num	The command line number to give the given command line
 */
char *giveNumberCommand(unsigned int num);


#endif
