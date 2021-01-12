#include <pthread.h>
#include <semaphore.h>
#include "err_handlers.h"
#include <stdio.h>

#define OP_ENDS_WITH_FAILURE     (-1)
#define OP_SUCCESS               (0)
#define SHARE_SEM_BETWEEN_THREADS  (0)
#define BIN_SEMAPHORE   (1)
static int g_var = 0;


#if !defined(DYN_SEM) && !defined(ST_SEM)  /* if no directives passed through cli define ST_SEM as default choice */
  #define ST_SEM 
#elif defined DYN_SEM
  #ifdef ST_SEM
    #undef ST_SEM
  #endif
  #include <stdlib.h>
#endif

#ifdef ST_SEM
  static sem_t sem;
#endif

#ifdef DYN_SEM
  static sem_t *sem;
#endif

static void *
f_inc_thread(void *arg)
{
    int loops = *((int*)arg);
    int loc, j, ret;
    
    for(j = 0; j < loops; ++j)
    {
        /* lock sem before operating on the shared var */
        #ifdef ST_SEM
        ret = sem_wait(&sem);
        #else
        ret = sem_wait(sem);
        #endif
        if(ret == OP_ENDS_WITH_FAILURE)
        {
            f_handle_err_and(E_ABORT, "sem_wait");
        }
        
        ++g_var;

        #ifdef ST_SEM
        ret = sem_post(&sem);
        #else
        ret = sem_post(sem);
        #endif
        
        if(ret == OP_ENDS_WITH_FAILURE)
        {
            f_handle_err_and(E_ABORT, "sem_post");
        }
    }

    return (void*)0;
}




int
main(int argc, char *argv[])
{
    pthread_t incthx, incthy;
    int t, cnt = 10,ret;
    
    /* Initialize a thread-shared mutex with the value 1 */
    #ifdef ST_SEM
    ret = sem_init(&sem, SHARE_SEM_BETWEEN_THREADS, BIN_SEMAPHORE);
    #else
    sem = (sem_t*)malloc(sizeof(sem_t));
    ret = sem_init(sem, 0, 1);
    #endif
    if (ret == OP_ENDS_WITH_FAILURE)
    {
        f_handle_err_and(E_ABORT, "sem_init");
    }

    /* Create two threads that increment g_var */
    if((t = pthread_create(&incthy, 0x0, f_inc_thread, &cnt)) != OP_SUCCESS)
    {
        f_handle_err_and(E_ABORT, "pthread_create");
    }

    if((t = pthread_create(&incthx, 0x0, f_inc_thread, &cnt)) != OP_SUCCESS)
    {
        f_handle_err_and(E_ABORT, "pthread_create");
    }
    

    /* wait for threads */
    if((t = pthread_join(incthx, NULL))!= OP_SUCCESS)
    {
        f_handle_err_and(E_ABORT, "pthread_join");
    }

    if((t = pthread_join(incthy, NULL))!= OP_SUCCESS)
    {
        f_handle_err_and(E_ABORT, "pthread_join");
    }

    #ifdef ST_SEM
    ret = sem_destroy(&sem);
    #else
    ret = sem_destroy(sem);
    free(sem);
    #endif

    printf("g_var value : %d\n", g_var);

    return 0;
}
