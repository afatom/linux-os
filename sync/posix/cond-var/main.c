#include <pthread.h>
#include <unistd.h>
#include <stdio.h>


#define SLEEP_FOR_1_SECOND  (1)
#define SLEEP_FOR_2_SECOND  (2)
#define SLEEP_FOR_3_SECOND  (3)


#define M_DO_WORK()  sleep(SLEEP_FOR_1_SECOND)


struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             shared_counter; 
} my_struct_t = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 3};


/* thread start routine */
void  *th_start_routine(void *arg)
{
    /*assume that we have did some useful work*/
    
    /* at this point we want to dec the shared counter and signal the main thread that we have reach 
       this point so he can resumes his work (only if counter = 0)
    */

   sleep(SLEEP_FOR_2_SECOND);
   pthread_t i = pthread_self();
   printf("Thread < %lu > Finished init part\n", i);

   pthread_mutex_lock(&my_struct_t.mutex);
   my_struct_t.shared_counter--;
   pthread_cond_signal(&my_struct_t.cond);
   pthread_mutex_unlock(&my_struct_t.mutex);
    


    /* main loop */
    for(;;)
    {
        M_DO_WORK();
    }
    return (void*)0;
}







int main()
{
    pthread_t lpw, hpw, mpw;
    pthread_create(&lpw, NULL, th_start_routine, NULL);
    pthread_create(&hpw, NULL, th_start_routine, NULL);
    pthread_create(&mpw, NULL, th_start_routine, NULL);
    
    pthread_mutex_lock(&my_struct_t.mutex);
    while(my_struct_t.shared_counter)
    {
        /* wait on the shared counter and the last thread will signal and wake up us*/
        pthread_cond_wait(&my_struct_t.cond, &my_struct_t.mutex);
    }
    pthread_mutex_unlock(&my_struct_t.mutex);
    
    /*all 3 threads have reached thier point so i can keep working */
    for(;;)
    {
        M_DO_WORK();
        puts("Main thread is working");
    }


    return 0;
}