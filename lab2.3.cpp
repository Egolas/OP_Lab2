#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include "data_struct.h"
#include <fcntl.h>
#include <fstream>


//消费者1



#define PRODUCER_NUM 2  //生产者数目
#define CONSUMER_NUM 2  //消费者数目
#define SHARED_SIZE 64
#define BUFFER_SIZE	 (SHARED_SIZE/sizeof(CommonData))   //缓冲池大小,可存放int类型数
#define P_READ bufptrs[0] //缓冲池读取指针
#define P_WRITE bufptrs[1] //缓冲池写入指针

using namespace std;

int main()
{
  
	void *shared_memory = (void *)0;
	CommonData *shared_stuff;
  void *pids_memory = (void *)0;
  pid_t  *pids;
  void *bufptr_memory = (void *)0;
  int *bufptrs;

	int shmid1,shmid2,shmid3;
  
	srand((unsigned int)getpid());
	shmid1 = shmget((key_t)1234, SHARED_SIZE, 0666 | IPC_CREAT);
  shmid2 = shmget((key_t)12345, 2*sizeof(pid_t), 0666 | IPC_CREAT);
  shmid3 = shmget((key_t)123456, 2*sizeof(int), 0666 | IPC_CREAT);
  
	if (shmid1 == -1 || shmid2 == -1 || shmid3 == -1) 
	{
		printf("shmget failed\n");
		exit(EXIT_FAILURE);
	}
  shared_memory = shmat(shmid1, (void *)0, 0);
  pids_memory = shmat(shmid2, (void *)0, 0);
  bufptr_memory = shmat(shmid3, (void *)0, 0);


	if (shared_memory == (void *)-1 || pids_memory == (void *)-1 || bufptr_memory == (void *)-1) 
	{
		printf( "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Memory attached\n");
  shared_stuff = (CommonData*)shared_memory;
  pids = (pid_t*)pids_memory;
  bufptrs = (int*)bufptr_memory;

  sem_t *room_sem = sem_open("room_sem",O_CREAT, 0644, BUFFER_SIZE);		//同步信号信号量，表示缓冲区有可用空间
  sem_t *product_sem = sem_open("product_sem",O_CREAT,0644, 0);		//同步信号量，表示缓冲区有可用产品
  sem_t *mutex = sem_open("mutex",O_CREAT,0644, 1);
  if (room_sem == SEM_FAILED || product_sem == SEM_FAILED || mutex == SEM_FAILED)
  {
    printf("sem initial failed.");
    sem_unlink("room_sem");
    sem_unlink("product_sem");
    sem_unlink("mutex");
    exit(EXIT_FAILURE);
  }

  fstream file_out("b.out", fstream::out);
  if(file_out.is_open())
  {
    for (int i = 0; i < 26; ++i)
    {
      usleep(rand() % 500000 + 500000);
      sem_wait(product_sem);
      sem_wait(mutex);
      CommonData* data = &shared_stuff[P_READ];
      if (data->program_id == pids[1])
      {
        file_out << (char)(data->data) << "\t";
        P_READ= ( P_READ + 1) % BUFFER_SIZE;
        int pro_num = (int)((P_WRITE-P_READ+BUFFER_SIZE)%BUFFER_SIZE);
        printf("Consumer 2 read from buffer, with data %c\n", (char)(data->data));
	    	printf("product number is %d\n",pro_num == 0? 8:pro_num);
        sem_post(mutex);
        sem_post(room_sem);
      }
      else
      {
        --i;
        sem_post(mutex);
        sem_post(product_sem);
      }
    }
  }
  else
  {
    printf("Open file failed.");
  }
  file_out.close();
  sem_close(room_sem);
  sem_close(product_sem);
  sem_close(mutex);
  sem_unlink("room_sem");
  sem_unlink("product_sem");
  sem_unlink("mutex");
  if (shmdt(shared_memory) == -1) 
  {
    printf("shmdt1 failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmdt(pids_memory) == -1) 
  {
    printf("shmdt2 failed\n");
    exit(EXIT_FAILURE);
  }
  if (shmdt(bufptr_memory) == -1) 
  {
    printf("shmdt1 failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmctl(shmid1, IPC_RMID, 0) == -1) 
  {
    printf("shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid2, IPC_RMID, 0) == -1) 
  {
    printf("shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }
  
  if (shmctl(shmid3, IPC_RMID, 0) == -1) 
  {
    printf("shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }
  printf("Process 4 done.");
  getchar();
  return 0;
}
