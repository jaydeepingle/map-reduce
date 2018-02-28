#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mapreduce.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#include "common.h"
#define FILESIZE 16

void mapreduce(MAPREDUCE_SPEC * spec, MAPREDUCE_RESULT * result)
{
    int fd;
    struct timeval start, end;
    
    if (NULL == spec || NULL == result) {
        EXIT_ERROR(ERROR, "NULL pointer!\n");
    }
    
    gettimeofday(&start, NULL);

    // partition, map, reduce, signal handler
    // partition
    
    int split_num = spec->split_num;
    DATA_SPLIT data_split[split_num];
    int status[split_num];
    
    int file_size = 0, counter = 0, inc = 0, diff = 0;// worker_fd = 0;
    int bytes_processed = 0, bytes_read = 0, chunk_index = 0;
    int chunk_size = 0, fix_chunk_size = 0;
    int index = 0, temp_index = 0, brk = 0, flag = 0;
    char *buffer;
    // pid_t parent_id = getpid();
    counter = spec->split_num;

    if((fd = open(spec->input_data_filepath, O_RDONLY)) == -1) {
        printf("Failed to open the file\n");
    }

    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    chunk_size = (int) (file_size / counter); 
    fix_chunk_size = chunk_size;
    do {
        counter--;
        if(counter == 0) {
            diff = (file_size - bytes_processed);
            buffer = (char *)calloc(sizeof(char), diff + 1);
            bytes_read = read(fd, buffer, diff);
        } else {
            buffer = (char *)calloc(sizeof(char), fix_chunk_size + 1);
            bytes_read = read(fd, buffer, fix_chunk_size);
        }
        
        int worker_fd = open(spec->input_data_filepath, O_RDONLY);
        lseek(worker_fd, bytes_processed, SEEK_SET);
        data_split[inc].fd = worker_fd;
        //dup2(worker_fd, data_split[inc].fd);
        data_split[inc].usr_data = spec->usr_data;

        index = 0;
        temp_index = 0;
        while(index < bytes_read && buffer[index] != EOF) {
            index += 1;
        }
        bytes_processed += index;
        chunk_index = 0;
        if(fix_chunk_size != file_size && counter != 0) {
            while(chunk_index < fix_chunk_size) {
                if(buffer[fix_chunk_size - chunk_index] == '\n') {
                    flag = 1;
                    temp_index = chunk_index;
                    if(buffer[fix_chunk_size - chunk_index] == EOF) {
                        brk = 1;
                    }
                    break;
                }
                chunk_index = chunk_index + 1;
            }
        }
        data_split[inc].size = index - temp_index;
        
        buffer[index - temp_index] = '\0';
        bytes_processed -= temp_index;
        if(flag == 1) {
            bytes_processed += 1;
        }
        if(brk == 1) {
            break;
        }
        lseek(fd, bytes_processed, SEEK_SET);
        if(buffer != NULL) {
            free(buffer);
            buffer = NULL;    
        }
        inc++;
    } while(index <= bytes_read && bytes_processed < file_size);

    // process fork logic
    int file_desc[2];
    int ret = pipe(file_desc);
    int fork_index = 0;
    for (fork_index = 0; fork_index < split_num; fork_index++) {
        ret = fork();
        if (ret == -1) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if(ret == 0) {
            fprintf(stdout, "mapreduce():\tchild[%d]: child process created!\n", getpid());
            
            if(1) {
                // map 
                char *intermediate_file = (char *)malloc(sizeof(char) * FILESIZE + 1);
                sprintf(intermediate_file, "mr-%d.itm", fork_index);
                int intermediate_file_desc = open(intermediate_file, O_RDWR | O_CREAT | O_APPEND | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
                if(intermediate_file_desc == -1) {
                    fprintf(stderr, "Unable to open the file: %s\n", intermediate_file);
                }
                
                //fprintf(stdout, "mapreduce():\tparent[%d]: map input fd %d opened!\n", getppid(), data_split[fork_index].fd);
                //fprintf(stdout, "mapreduce():\tparent[%d]: map output fd %d opened!\n", getppid(), intermediate_file_desc);    
                
                spec->map_func(&data_split[fork_index], intermediate_file_desc);
                if(intermediate_file != NULL) {
                    free(intermediate_file);    
                    intermediate_file = NULL;
                }
                close(intermediate_file_desc);
            }

            if (fork_index == 0) {
                close(file_desc[1]); // close WRITE end
                char c = 0;
                read(file_desc[0], & c, 1);
                close(file_desc[0]); // close READ  end
                // reduce 
                if(1) {
                    // reduce
                    int *p_fd_in;
                    int fd_in_num = split_num; 
                    int fd_out = open(result->filepath, O_RDWR | O_CREAT | O_APPEND | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
                    p_fd_in = (int *)malloc(sizeof(int) * fd_in_num + 1);
                    int reduce_index = 0;
                    for(reduce_index = 0; reduce_index < fd_in_num; reduce_index++) {
                        char *input_file_name = (char *)malloc(sizeof(char) * FILESIZE + 1);
                        sprintf(input_file_name, "mr-%d.itm", reduce_index);
                        p_fd_in[reduce_index] = open(input_file_name, O_RDONLY);
                        if(input_file_name != NULL) {
                            free(input_file_name);
                            input_file_name = NULL;
                        }
                    }

                    spec->reduce_func(p_fd_in, fd_in_num, fd_out);
                    close(fd_out);
                    reduce_index = 0;
                    for(reduce_index = 0; reduce_index < fd_in_num; reduce_index++) {
                        close(p_fd_in[reduce_index]);
                    }
                    if(p_fd_in != NULL) {
                        free(p_fd_in);
                        p_fd_in = NULL;
                    }
                }
            } 
            if(fork_index != 0) {
                // pass
            }
            fprintf(stdout, "mapreduce():    parent[%d]: child %d terminated.\n", getppid(), getpid());
            exit(0);
        } else if(ret > 0) {
            result->map_worker_pid[fork_index] = ret;
            if(fork_index == 0) {
                result->reduce_worker_pid = ret;
            }
        } 
    }
    
    close(fd);
    
    close(file_desc[1]); // close WRITE end
    close(file_desc[0]); // close READ end   
    for (int i = 0; i < split_num; i++) {
        waitpid(result->map_worker_pid[i], &status[i], 0);
    }
    fprintf(stdout, "mapreduce():    parent[%d]: reduce worker[%d] terminated.\n", getpid(), result->reduce_worker_pid);

    gettimeofday(&end, NULL);   

    result->processing_time = (end.tv_sec - start.tv_sec) * US_PER_SEC + (end.tv_usec - start.tv_usec);
}
