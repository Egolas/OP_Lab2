#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include "data_struct.h"
#include <fcntl.h>

#define PRODUCER_NUM 2  //生产者数目
#define CONSUMER_NUM 2  //消费者数目
#define SHARED_SIZE 64
#define BUFFER_SIZE	 (SHARED_SIZE/sizeof(CommonData))   //缓冲池大小,可存放int类型数
#define SEMS_SIZE sizeof(Sems)

using namespace std;



int p_read=0;	//缓冲池读取指针
int p_write=0; //缓冲池写入指针


int main()
{
	void *shared_memory = (void *)0;
	CommonData *shared_stuff;

	int shmid;
  
	srand((unsigned int)getpid());
	shmid = shmget((key_t)1234, SHARED_SIZE, 0666 | IPC_CREAT);

	if (shmid == -1) 
	{
		printf("shmget failed\n");
		exit(EXIT_FAILURE);
	}
  shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) 
	{
		printf( "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Memory attached\n");
  shared_stuff = (CommonData*)shared_memory;

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

/*
 for (int i = 0; i < 10000; ++i)
  {
    CommonData* data = new CommonData(getpid(), i);
    sleep(rand() % 5);
    sem_wait(&room_sem);
    sem_wait(&mutex);
    shared_stuff[pWrite] = *data;
  }
  */
  sem_close(room_sem);
  sem_close(product_sem);
  sem_close(mutex);
  sem_unlink("room_sem");
  sem_unlink("product_sem");
  sem_unlink("mutex");
  if (shmdt(shared_memory) == -1) 
  {
    printf("shmdt failed\n");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, 0) == -1) 
  {
    printf("shmctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  
  getchar();
  getchar();
  return 0;
}
