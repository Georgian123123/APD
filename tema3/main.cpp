#include "main.h"

/* Function which reads the entire paragraph for a gender
    Depends on the gender, we read till the new line and
    send to the workers
 */
void send_message(ifstream &in_file, struct thread_data mythread, string type)  
{   
    string line = "";
    int id_paragraph = 0;
    while (getline(in_file, line)) {
        if (line.empty() || in_file.eof()) {
            id_paragraph++;
        }
        if (line == type) {
            int number_of_lines = 0;
            /* Send the specific paragraph */
            while (getline(in_file, line) && !line.empty()) {
                mythread.paragraph += line;
                mythread.paragraph += "\n";
                number_of_lines++;
            }
            /* Send data to workers */
            int length = mythread.paragraph.size();
            char *to_send = (char*)mythread.paragraph.c_str();
            /* Length of the message */
            MPI_Send(&length, 1, MPI_INT, mythread.id, 0, MPI_COMM_WORLD);
            /* The message */
            MPI_Send(to_send, length, MPI_CHAR , mythread.id, 0, MPI_COMM_WORLD);
            /* the number of lines*/
            MPI_Send(&number_of_lines, 1, MPI_INT, mythread.id, 0, MPI_COMM_WORLD);
            /* The id of the paragraph*/
            MPI_Send(&id_paragraph, 1, MPI_INT, mythread.id, 0, MPI_COMM_WORLD);
            /* The type of the paragraph*/
            MPI_Send(type.c_str(), 20, MPI_CHAR, mythread.id, 0, MPI_COMM_WORLD);
            mythread.paragraph = "";
            id_paragraph++;
            
            /* Wait for the worker's message */
            get_message_from_worker(mythread.id);
        }
    }
    /* Send the message to workers to announce them that Master finished the messages */
    int message = -1;
    MPI_Send(&message, 1, MPI_INT, mythread.id, 0, MPI_COMM_WORLD);
}

/* Function that get the data that one worker sent*/
void get_message_from_worker(int type_data)
{   
    /* Initialize the data for the recv*/
    MPI_Status status;
    int type, length;
    char *paragraph = NULL;
    int id_par;
    char message_type[20];
    
    /* Receive datas from master */

    /* The id of the paragraph */
    MPI_Recv(&id_par, 1, MPI_INT, type_data, 0, MPI_COMM_WORLD, &status);
    /* The length of the paragraph */
    MPI_Recv(&length, 1, MPI_INT, type_data, 0, MPI_COMM_WORLD, &status);
    paragraph = (char*)malloc((length + 1) * sizeof(char));
    /* The paragraph */
    MPI_Recv(paragraph, length, MPI_CHAR, type_data, 0, MPI_COMM_WORLD, &status);
    /* The type of the paragraph*/
    MPI_Recv(message_type, 20, MPI_CHAR, type_data, 0, MPI_COMM_WORLD, &status);

    paragraph[length] = '\0';
    string str(paragraph);

    string type_message(message_type);

    /* Insert the data into the sliding window*/
    pthread_mutex_lock(&mutex_lock);
    window.insert(make_pair(id_par, make_pair(str, type_message)));
    pthread_mutex_unlock(&mutex_lock);

    pthread_mutex_lock(&mutex_lock);
    while (window.find(counter_paragraph) != window.end()) {
        /* Wait till we do not have messages anymore to print */
        out << (char*)window[counter_paragraph].second.c_str();
        out << endl;
        out << (char*)window[counter_paragraph].first.c_str();
        out << endl;
        window.erase(counter_paragraph);
        counter_paragraph++;
    }
    free(paragraph);
    pthread_mutex_unlock(&mutex_lock);
}

/*
    Thread_Function.
    For every thread we open the file and read the paragraph
    that we care about.
*/
void *read_p(void *buffer)
{
    struct thread_data mythread = *(struct thread_data*)buffer;
    ifstream in_file;
    in_file.open(mythread.file_name);
    string line = "0";

    /* Depending on the type of worker, read the specific paragraph*/
    if (mythread.id == HORROR) {
        send_message(in_file, mythread, "horror");
    } else if (mythread.id == COMEDY) {
        send_message(in_file, mythread, "comedy");
    } else if (mythread.id == FANTASY) {
        send_message(in_file, mythread, "fantasy");
    } else if (mythread.id == SCIFI) {
        send_message(in_file, mythread, "science-fiction");
    }
    in_file.close();

    pthread_exit(NULL);
}

/* Verify if a character is consonant */
bool isConsonant(char c)
{
   // evaluates to 1 if variable c is a lowercase vowel
    int lowercase_vowel = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');

    // evaluates to 1 if variable c is a uppercase vowel
    int uppercase_vowel = (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');

    return !(lowercase_vowel || uppercase_vowel);
}

/* if a character is consonant double it, else let it be */
string horror_modify(string str)
{   
    vector<string> result;
    string str_result;
    for (auto character : str) {
        if (isConsonant(character) && character != ' ' && 
            character != '\n' && character != '!' &&
            character != '?'  && character != '.' &&
            character != ',') {
            str_result += character;
            str_result += tolower(character);
        } else {
            str_result += character;
        }
    }
    return str_result;
}

/* if a character is on an even position upper it */
string comedy_modify(string str)
{
    int length = 1;
    string str_result;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == ' ' || str[i] == '\n') {
            length = 1;
            str_result += str[i];
        } else {
            if (length % 2 == 0) {
                str_result += toupper(str[i]);
            } else {
                str_result += str[i];
            }
            length++;
        }
    }
    return str_result;
}

/* Letter from the beggining of a word gets upper */
string fantasy_modify(string str)
{
    int length = 1;
    string str_result;
    str_result += toupper(str[0]);
    for (int i = 1; i < str.size(); ++i) {
        if (str[i - 1] == ' ' || str[i - 1] == '\n') {
            str_result += toupper(str[i]);
        } else {
            str_result += str[i];
        }
    }
    return str_result;
} 

/* Seventh word is inverted */
string scifi_modify(string str)
{
    int length = 1;
    string str_result;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == ' ') {
            length++;
            str_result += str[i];
        }
        if (str[i] == ' ' && length % 7 == 0) { 
            int j = i + 1;
            string to_reverse = "";
            while (str[j] != ' ' && str[j] != '\n') {
                to_reverse += str[j];
                j++;
            }
            reverse(to_reverse.begin(), to_reverse.end());
            str_result += to_reverse;
            i = j - 1;
        } else if (str[i] == '\n') {
            length = 1;
            str_result += str[i];
        } else if (str[i] != ' ') {
            str_result += str[i];
        }
    }
    return str_result;
}

/* Send data to master */
void send_to_master(string data_to_send, int id_par, char message_type[20])
{
    int length = data_to_send.size();
    /* The id of the paragraph */
    MPI_Send(&id_par, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    /* The length of the paragraph */
    MPI_Send(&length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    /* The paragraph */
    MPI_Send(data_to_send.c_str(), length, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    /* The type of the message */
    MPI_Send(message_type, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
}

/* Divide each paragraph in twenty lines or less depends the size */
void get_lines(vector<string>& chunks, string &str)
{
    string chunk = "";
    string line;
    stringstream delimitator(str);
    int count = 0;
 
    while(true) {
        /* Tokenize it by new line*/
        getline(delimitator, line, '\n');

        /* Go untill the end of the string */
        if(delimitator.eof()) {
            /* Add line by line */
            chunk += line;
            /* Push it in vector */
            chunks.push_back(chunk);
            break;
        } else {
            /* In the case that we reached the 20 lines, reset*/
            count++;
            chunk += line + "\n";
            if (count == 20) {
                chunks.push_back(chunk);
                chunk = "";
                count = 0;
            }
        }
    }
}

/* function to read and modify the paraggraphs */
void *thread_function(void *buffer)
{
    struct thread_details *thread = (struct thread_details*)buffer;

    for (int i = thread->start; i < thread->end; i++) {
        string str;
        /* depends on the worker modify it by its rules */
        if (thread->type == HORROR) {
            str =  horror_modify((*thread->chunks)[i]);
        } else if (thread->type == COMEDY) {
            str =  comedy_modify((*thread->chunks)[i]);
        } else if (thread->type == FANTASY) {
            str =  fantasy_modify((*thread->chunks)[i]);
        } else if (thread->type == SCIFI) {
            str =  scifi_modify((*thread->chunks)[i]);
        }
        (*thread->chunks)[i] = str;
    }

    pthread_exit(NULL);
}

/* Every worker waits for the messages from the MASTER */
void get_paragraphs(int type)
{
    MPI_Status status;
    int value, number_of_lines, id_paragraph;
    char *paragraph;
    int id_par;
    while(MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status) == MPI_SUCCESS) {
        if (value == -1) break;

        /* Receive messages from the Master */
        paragraph = (char*) malloc((value + 1) * sizeof(char));
        /* The paragraph */
        MPI_Recv(paragraph, value, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        paragraph[value] = '\0';

        /* The number of lines */
        MPI_Recv(&number_of_lines, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
        /* The id of the paragraph */
        MPI_Recv(&id_par, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        char message_type[20];
        /* The type of the message */
        MPI_Recv(message_type, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        
        /* Calculate the number of lines that needs to be pushed*/
        string str(paragraph);
        int twenty_lines = (number_of_lines < 20) ? 0 : number_of_lines / 20; 
        int remained = (number_of_lines % 20 == 0) ? 0 : 1;
        int num_cores = std::min(twenty_lines + remained, NUM_THREADS);
        
        vector<string> chunks;
        /* Add in the chunks vector */
        get_lines(chunks, str);
        int array_size = chunks.size();

        /* Start the threads which will modify the chunks*/
        pthread_t tid[num_cores];
        vector<struct thread_details> threads(num_cores);
        for (int i = 0; i < num_cores; ++i) {
            /* Start and end for the chunks vector that are divided for each thread */
            int start = i * (double)array_size / num_cores;
            int end = MIN((i + 1) * (double)array_size / num_cores, array_size);
            threads[i].id = i;
            threads[i].chunks = &chunks;
            threads[i].start = start;
            threads[i].end = end;
            threads[i].type = type;
            
            /* Start the thread */
            if (pthread_create(&tid[i], NULL, thread_function, &threads[i])) {
                printf("eroare la crearea threadului");
                exit(-1);
            }
        }

        /* Wait untill the threads finishes */
        for (int i = 0; i < num_cores; ++i) {
            if (pthread_join(tid[i], NULL)) {
                printf("eroare la inchiderea thread-ului threadului");
                exit(-1);
            }
        }

        /* Build the modified string */
        string result = "";
        for (int i = 0; i < chunks.size(); ++i) {
            result += chunks[i];
        }

        /* Depends on the worker send it */
        if (type == HORROR) {
            send_to_master(result, id_par, message_type);
        } else if (type == COMEDY) {
            send_to_master(result, id_par, message_type);
        } else if (type == FANTASY) {
            send_to_master(result, id_par, message_type);
        } else if (type == SCIFI) {
            send_to_master(result, id_par, message_type);
        }
        free(paragraph);
    }
}

int main(int argc, char *argv[])
{
    int rank, procs, provided;
    /* Initialize the MPI World */
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Initialize the mutex and barrier*/
    pthread_mutex_init(&mutex_lock, NULL);    
    string str(argv[1]);

    string out_file_ = "./tests/in/" + str.substr(11, 6) + ".out";
    out.open(out_file_);

    if (rank == MASTER) {
        /* Initialize the treads which we use to read the
           text.
         */        
        pthread_t tid[MASTER_THREADS];
        int thread_id[MASTER_THREADS];
        vector<struct thread_data> threads(MASTER_THREADS);

        /* For every thread we send the file name and the id to the 
           thread function.
           Create the threads and wait them to finish
         */
        for (int i = 0; i < MASTER_THREADS; ++i) {
            thread_id[i] = i;
            threads[i].id = i + 1;
            threads[i].file_name = str;
            if (pthread_create(&tid[i], NULL, read_p, &threads[i])) {
                printf("eroare la crearea threadului");
                exit(-1);
            }
        }

        /* Close the threads */
        for (int i = 0; i < MASTER_THREADS; ++i) {
            if (pthread_join(tid[i], NULL)) {
                printf("eroare la inchiderea thread-ului threadului");
                exit(-1);
            }
        }
    } else {
        /* Depending on the worker start his job*/
        if (rank == HORROR) {
            get_paragraphs(HORROR);
        } else if (rank == COMEDY) {
            get_paragraphs(COMEDY);
        } else if (rank == FANTASY) {
            get_paragraphs(FANTASY);
        } else if (rank == SCIFI) {
            get_paragraphs(SCIFI);
        }
    }
    out.close();
	pthread_mutex_destroy(&mutex_lock);
    MPI_Finalize();
    return 0;
}