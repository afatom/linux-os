#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include "err_handlers.h"

#define SHARE_SEM_BETWEEN_PROC   (1)
#define BIN_SEMAPHORE   (1)
#define OP_ENDS_WITH_FAILURE     (-1)
#define OP_SUCCESS               (0)
#define SHM_MODE_0 
#define RDWR_ENSURE_CREATE      (O_RDWR|O_CREAT|O_EXCL)
#define SHM_OP_MODE             (S_ISUID|S_IRUSR|S_IWUSR)  
#define SHM_BUFFER_SIZE         1024
#define MAP_ZERO_OFFSET         0

#define M_SEM_WAIT(ret,sem)      \
if((ret = sem_wait(sem)) == OP_ENDS_WITH_FAILURE)\
{\
    abort_with("childs_sem_wait");\
}\

/*
 * CAUTION dont change the enums order since
 * they are index entries to the psync_status_table below 
 */
enum psync_status_type
{
    PSYNC_STATUS_SHM_CREATE_FAIL = 0,
    PSYNC_STATUS_SHM_FTRUNCATE_FAIL,
    PSYNC_STATUS_SEM_WAIT_FAIL,
    PSYNC_STATUS_SEM_INIT_FAIL,
    PSYNC_STATUS_SEM_POST_FAIL,
    PSYNC_STATUS_SEM_DESTROY_FAIL,
    PSYNC_STATUS_MEM_MAP_SYS_CALL_FAIL,
    PSYNC_STATUS_SEM_ALLOCATE_FAIL,
    PSYNC_STATUS_FORK_SYS_CALL_FAIL,
    PSYNC_STATUS_FILE_OPEN_FAIL,
    PSYNC_STATUS_WAIT_SYS_CALL_FAIL,
    PSYNC_STATUS_SUCCESS
};

/*
 * CAUTION dont change the string order since
 * they are aligned to the entries of the enum psync_status_type
 */
static const char *psync_status_table[] = 
{
    "shm create","shm ftruncate","sem wait", "sem init", 
    "sem post", "sem destroy", "memory map sys call", "malloc", 
    "fork sys call", "fopen", "wait sys call"
};

struct shm_set{
    void   *buf_addr;  /* the return address after mem map */
    sem_t  *sem;       /* the semaphore for mutual exclusion on the shm buffer */
};

static enum psync_status_type 
shm_region_init(const char *shm_name, struct shm_set *retval);

static int
write_from_file_to_shm(const char *fname, size_t size, void *addr);

static int
safe_shm_write(const char *fn, sem_t *sem, void *addr, size_t size);






int
main(int argc, char *argv[])
{
    void *addr = NULL;
    sem_t *sem; 
    pid_t p;
    int ret, wstatus, fd;
    FILE *fp;
    const char *hello = "Hello World";
    const char *fname = "foo.txt";
    const char *shm_name = "/dev/shm/temp_shm";
    const char *story = "";
    enum psync_status_type e; 
    struct shm_set set;

    if((e = shm_region_init(shm_name, &set)) != PSYNC_STATUS_SUCCESS)
    {
        abort_with(psync_status_table[e]);
    }
    
    p = fork();
    
    if(-1 == p)
    {
        abort_with("fork fails");
    }
    else if(0 == p)
    {
        /* child code */
        void *addr;
        char *p_buf;
        int c;
        size_t len =  SHM_BUFFER_SIZE - sizeof(sem_t) + 1;

        fd = shm_open(shm_name, O_RDONLY, 0);
        if (fd == -1)
        {
            abort_with("child shm open");
        }

        addr = mmap(NULL, SHM_BUFFER_SIZE, PROT_READ, MAP_SHARED, fd, 0);
        
        if (addr == MAP_FAILED)
        {
            abort_with("childs mmap");
        }
        
        if (close(fd) == -1)
        {
            abort_with("child close fd");
        }
        
        while(1)
        {
            p_buf = (char*)(((struct shm_set*)addr) + 1);
            if(*p_buf=='\0')
            {
                return 0;
            }
            len =  SHM_BUFFER_SIZE - sizeof(sem_t) + 1;

            if((ret = sem_wait(((struct shm_set*)addr)->sem)) == OP_ENDS_WITH_FAILURE) 
            {
                abort_with("childs_sem_wait");
            }            
            
            while(len-- && ((c=*p_buf)!= '\0'))
            {
                putchar(c);
                ++p;
            }
            
            if((ret = sem_post(((struct shm_set*)addr)->sem)) == OP_ENDS_WITH_FAILURE) 
            {
                abort_with("childs_sem_post");
            }            
        }
    }
    else
    {   
        int g = safe_shm_write(story, set.sem, ((sem_t*)(set.buf_addr) + 1), SHM_BUFFER_SIZE - sizeof(sem_t));
        /*if(g==EOF) 
        */
        for(;safe_shm_write(NULL, set.sem, ((sem_t*)(set.buf_addr) + 1), SHM_BUFFER_SIZE - sizeof(sem_t))!= EOF;){ }
        
        if(wait(&wstatus) == OP_ENDS_WITH_FAILURE) 
        {
            abort_with("wait_for_child");
        }
        
        if(sem_destroy(sem)== OP_ENDS_WITH_FAILURE)
        {
            abort_with("sem_destroy");
        }
    }    
    
    return 0;
}

static enum psync_status_type 
shm_region_init(const char *shm_name, struct shm_set *retval)
{
    int fd, ret;

    retval->sem = retval->buf_addr = NULL;

    if((fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666)) == -1)
    {
        return PSYNC_STATUS_SHM_CREATE_FAIL;
    }
    
    if (ftruncate(fd, SHM_BUFFER_SIZE) == -1)
    {
        return PSYNC_STATUS_SHM_FTRUNCATE_FAIL;
    }

    /* map the shared mem object in the virtual address space of the calling process 
       starting at address add, with the length SHM_BUFFER_SIZE  
    */
    retval->buf_addr = mmap(NULL, SHM_BUFFER_SIZE + sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, MAP_ZERO_OFFSET);
    if (retval->buf_addr == MAP_FAILED)
    {
        return PSYNC_STATUS_MEM_MAP_SYS_CALL_FAIL;
    }

    retval->sem = (sem_t*)(retval->buf_addr);
    
    if(!retval->sem)
    {
        /**
         * at exec() or termination all mem mappings will be unmapped automatically
         * Thereis no need to call munmap()
         */
        return PSYNC_STATUS_SEM_ALLOCATE_FAIL;
    }
    
    ret = sem_init(retval->sem, SHARE_SEM_BETWEEN_PROC, BIN_SEMAPHORE);
    
    if(ret == OP_ENDS_WITH_FAILURE)
    {
        /**free(retval->sem);*/
        return PSYNC_STATUS_SEM_INIT_FAIL;
    }
    
    return PSYNC_STATUS_SUCCESS;
}


static int
write_from_file_to_shm(const char *fname, size_t size, void *addr)
{
    char c; size_t counter = size + 1;
    static FILE *fp;
    char *p=(char*)addr;
    
    if(fname)
    {
        fp = fopen(fname, "r");
        if(!fp)
            return -1;
    }
    
    while((counter--) && ((c = (char)fgetc(fp)) != EOF))
    {
        *p = c;
        ++p;
    }

    return (feof(fp));
}

static int
safe_shm_write(const char *fname, sem_t *sem, void *addr, size_t size)
{
    int ret;

    if((ret = sem_wait(sem)) == OP_ENDS_WITH_FAILURE)
    {
        abort_with("parent_sem_wait");
    } 

    ret = write_from_file_to_shm(fname, size, addr);

    if((ret = sem_post(sem)) == OP_ENDS_WITH_FAILURE)
    {
        abort_with("parent_sem_post");
    }

    return ret; 
}









