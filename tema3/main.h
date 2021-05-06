#ifndef MAIN_H
#define MAIN_H

#include "mpi.h"
#include <bits/stdc++.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

/* Some macros */
#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SCIFI  4
#define MASTER_THREADS 4
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y)) 

/* Outfile file */
ofstream out;

/* Mutex */
pthread_mutex_t mutex_lock;

/* Counter for sliding window */
int counter_paragraph = 0;

/* Maximum number of threads on this computer */
int NUM_THREADS = sysconf(_SC_NPROCESSORS_CONF);

/* Sliding window */
unordered_map<int, pair<string, string>> window;

/* The main thread's detail for Master */
struct thread_data {
    string paragraph;
    int id;
    string file_name;
} data;

/* The thread's detail for wokers */
struct thread_details {
    int id;
    vector<string> *chunks;
    int start;
    int end;
    int type;
} details;

/* Function to modify the string */
string horror_modify(vector<string> data);
string comedy_modify(vector<string> data);
string fantasy_modify(vector<string> data);
string scifi_modify(vector<string> data);

/* Send functions */
void send_message(ifstream &in_file, struct thread_data mythread, string type);
void send_to_master(string data_to_send, int id_paragraph, char message_type[20]);
void get_message_from_worker(int type_data);

/* Thread's functions */
void *read_p(void *buffer);
void *thread_function(void *buffer);

/* Utilites functions */
void print_data(vector<string> data);

/* Helpers */
void get_paragraphs(int type);
bool isConsonant(char c);
void get_lines(vector<string>& chunks, string &str);

#endif
