#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include "common.h"
#include "usr_functions.h"

#define DELIMITERS "\n"
#define LETTERS 26
char *strcasestr(const char *haystack, const char *needle);
/* User-defined map function for the "Letter counter" task.  
   This map function is called in a map worker process.
   @param split: The data split that the map function is going to work on.
                 Note that the file offset of the file descripter split->fd should be set to the properly
                 position when this map function is called.
   @param fd_out: The file descriptor of the itermediate data file output by the map function.
   @ret: 0 on success, -1 on error.
 */
int letter_counter_map(DATA_SPLIT * split, int fd_out)
{
    fprintf(stdout, "word_finder_map():\tpid[%d] ===== Running user defined finder map function =====\n", getpid());
    //fprintf(stdout, "map_work():\tchild[%d]: map input fd %d (chunksize %d), map output fd %d, ready\n", getpid(), split->fd, split-> size, fd_out);

    /*
    Algorithm:
    1. read the split->size using split->fd
    2. traverse each char and increse the count
    3. write the result into file
    */
    int byte_count = 0, i = 0, index = 0, value = 0;
    char *read_buffer, *write_buffer;
    int count_array[LETTERS] = {0};

    read_buffer = (char *)malloc(sizeof(char) * split->size + 1);
    write_buffer = NULL;
    
    if((byte_count = read(split->fd, read_buffer, split->size)) == -1) {
        fprintf(stderr, "Unable to read from file descriptor!\n");
    } 
    
    i = 0;
    while(i < byte_count) {
        if(read_buffer[i] <= 'Z' && read_buffer[i] >= 'A') {
            index = read_buffer[i] - 'A';
            count_array[index] += 1;
        } else if(read_buffer[i] <= 'z' && read_buffer[i] >= 'a') {
            index = read_buffer[i] - 'a';
            count_array[index] += 1;
        }
        i += 1;
    }

    value = 16;
    write_buffer = (char *)malloc(sizeof(char) * value + 1);
    i = 0;
    while(i < LETTERS) {
        value = sprintf(write_buffer, "%c %d\n", i + 'A', count_array[i]);
        write(fd_out, write_buffer, value);
        i += 1;
    }

    if(read_buffer != NULL) {
        free(read_buffer);
        read_buffer = NULL;
    }

    if(write_buffer != NULL) {
        free(write_buffer); 
        write_buffer = NULL;
    }
    return 0;

}

/* User-defined reduce function for the "Letter counter" task.  
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data files' file descriptors.
                   The imtermeidate data files are output by the map worker processes, and they
                   are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file descriptor is 
             identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2] respectively.

*/
int letter_counter_reduce(int * p_fd_in, int fd_in_num, int fd_out)
{
    fprintf(stdout, "reduce_work():  Reduce worker[%d] start working!\n", getpid());
    fprintf(stdout, "word_finder_reduce():\t===== Running user defined finder reduce function =====\n");

    /*
    Algorithm:
    1. Read all the files one by one
    2. Sum up all the counts
    3. Write into the final output file
    */
    int i = 0;
    int count_array[LETTERS] = {};
    char *read_buffer = NULL;
    char *write_buffer = NULL;
    for(i = 0; i < fd_in_num; i++) {
        int file_size = lseek(p_fd_in[i], 0, SEEK_END);
        lseek(p_fd_in[i], 0, SEEK_SET);
        if(read_buffer != NULL) {
            free(read_buffer);
            read_buffer = NULL;
        }
        read_buffer = (char *)malloc(sizeof(char) * file_size + 1);
        int ret = read(p_fd_in[i], read_buffer, file_size);

        if(ret > 0) {

        }
        read_buffer[file_size] = '\0';
        char *ptr;
        const char *delim = "\n";
        ptr = strtok(read_buffer, delim);
        while (ptr != NULL) {
            char target[2][10];
            int n = 0, i, j = 0;
            int sampleFlag = 0;
            char ch;
            for(i = 0; 1; i++) {
                if(ptr[i] != '\0'){
                    if(ptr[i] == ' ') {
                        sampleFlag = 1;
                    } else if (sampleFlag == 1) {
                        target[n][j++] = ptr[i];    
                    } else {
                        // store character for index
                        ch = ptr[i];
                    }
                } else {
                    target[n][j++] = '\0';
                    n++;
                    j = 0;
                    sampleFlag = 0;
                }
                if(ptr[i] == '\0')
                    break;
            }
            for(i = 0; i < n; i++) {
                count_array[ch - 'A'] = count_array[ch - 'A'] + atoi(target[i]);
            }
            ptr = strtok(NULL, delim);
        }

        if(read_buffer != NULL) {
            free(read_buffer);    
            read_buffer = NULL;
        }
    }
    // print count array
    write_buffer = (char *)malloc(sizeof(char) * 16 + 1);
    for(int sample = 0; sample < LETTERS; sample++) {
        int data_length = sprintf(write_buffer, "%c %d\n", sample + 'A', count_array[sample]);
        write(fd_out, write_buffer, data_length);
    }
    
    // write logic here
    if(write_buffer != NULL) {
        free(write_buffer);    
        write_buffer = NULL;
    }
    return 0;
}

/* User-defined map function for the "Word finder" task.  
   This map function is called in a map worker process.
   @param split: The data split that the map function is going to work on.
                 Note that the file offset of the file descripter split->fd should be set to the properly
                 position when this map function is called.
   @param fd_out: The file descriptor of the itermediate data file output by the map function.
   @ret: 0 on success, -1 on error.
 */
int word_finder_map(DATA_SPLIT * split, int fd_out)
{
    fprintf(stdout, "word_finder_map():\tpid[%d] ===== Running user defined finder map function =====\n", getpid());
    //fprintf(stdout, "map_work():\tchild[%d]: map input fd %d (chunksize %d), map output fd %d, ready\n", getpid(), split->fd, split-> size, fd_out);

    /*
    Algorithm:
    1. read the split->size using split->fd
    2. tokenize and check
    3. write the result into file
    */
    int byte_count;
    char *ptr;
    char *buffer;
    const char *delim = "\n";
    buffer = (char *)malloc(sizeof(char) * split->size + 1);
    if((byte_count = read(split->fd, buffer, split->size)) == -1) {
        printf("Error reading file...!!!\n");
    } 
    
    buffer[split->size] = '\0';
    ptr = strtok(buffer, delim);
    while (ptr != NULL) {
        char *ch;
        ch = strstr((char *)ptr, (char *)split->usr_data);
        if(ch != NULL) {
            char *line_buffer = (char *)malloc(sizeof(char) * strlen(ptr) + 2);
            sprintf(line_buffer, "%s\n", ptr);
            write(fd_out, line_buffer, strlen(line_buffer));
            if(line_buffer != NULL) {
                free(line_buffer);    
                line_buffer = NULL;
            }
        }
        ptr = strtok(NULL, delim);
    }
    if(buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }
    
    if(ptr != NULL) {
        free(ptr);
        ptr = NULL;
    }
    return 0;
}

/* User-defined reduce function for the "Word finder" task.  
   This reduce function is called in a reduce worker process.
   @param p_fd_in: The address of the buffer holding the intermediate data files' file descriptors.
                   The imtermeidate data files are output by the map worker processes, and they
                   are the input for the reduce worker process.
   @param fd_in_num: The number of the intermediate files.
   @param fd_out: The file descriptor of the final result file.
   @ret: 0 on success, -1 on error.
   @example: if fd_in_num == 3, then there are 3 intermediate files, whose file descriptor is 
             identified by p_fd_in[0], p_fd_in[1], and p_fd_in[2] respectively.

*/
int word_finder_reduce(int * p_fd_in, int fd_in_num, int fd_out)
{
    fprintf(stdout, "reduce_work():\tReduce worker[%d] start working!\n", getpid());
    fprintf(stdout, "word_finder_reduce():\t===== Running user defined finder reduce function =====\n");

    // printf("word_finder_reduce...!!!\n");
    /*
    Algorithm
    1. read all the files and write result into the final file
    */
    int i = 0, byte_count;
    char *buffer = NULL;
    i = 0;
    while(i < fd_in_num) {
        int file_size = lseek(p_fd_in[i], 0, SEEK_END);
        lseek(p_fd_in[i], 0, SEEK_SET);
        buffer = (char *)malloc(sizeof(char) * file_size + 1);
        if(buffer == NULL) {
            fprintf(stderr, "Malloc failed\n");
            return -1;
        }
        if((byte_count = read(p_fd_in[i], buffer, file_size)) == -1) {
            return -1;
        } else {
            write(fd_out, buffer, byte_count);
        }
        i++;
        if(buffer != NULL) {
            free(buffer);
            buffer = NULL;    
        }
    }
    if(buffer != NULL) {
        free(buffer);
        buffer = NULL;    
    }
    return 0;
}