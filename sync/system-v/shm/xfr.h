#pragma once


extern char const *wrsem_name;
extern char const *rdsem_name;


#ifndef BUFFER_SIZE                 /* Allow cc -Directive to override this size from command line */
#define BUFFER_SIZE  (1024)         /* default size of the directive didnt appear in the cli */
#endif

#define SEM_KEY     (0x5678)       /*unique key for the named semaphore*/
#define SHM_KEY     (0x1234)       /*unique key for the shared memory IPC*/





#define WRITE_SEM        (0x0)
#define READ_SEM         (0x1)


struct shmseg_imposer{                /* the structure to be imposed on the shared memory bytes*/
    unsigned int used_in_buffer;      /* total count of currently bytes used in buffer */
    char buffer[BUFFER_SIZE];         /* Data being transfered  */
}; 