#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

#define PRODUCER_NUM 2  //生产者数目
#define CONSUMER_NUM 2  //消费者数目
#define BUFFER_SIZE	 (64/sizeof(int))   //缓冲池大小,可存放int类型数
int buffer[BUFFER_SIZE];  //缓冲区
int bRead = 0; //缓冲池读取标记
int bWrite = 0; //缓冲池写入标记
sem_t	room_sem;       //同步信号信号量，表示缓冲区有可用空间
sem_t	product_sem;        //同步信号量，表示缓冲区有可用产品
pthread_mutex_t mutex;


using namespace std;

void producer_run(void *arg)
{
	while (true)
	{
		sleep(1);
		sem_wait(&room_sem);
		pthread_mutex_lock(&mutex);
		//生产者往缓冲池中写入数据
		buffer[bWrite] = 1;
		bWrite = (bWrite + 1) % BUFFER_SIZE;
		printf("producer %d write to buffer\n", *(int*)arg);
		printf("buffer size is %d\n",(bWrite-bRead+BUFFER_SIZE)%BUFFER_SIZE);
		pthread_mutex_unlock(&mutex);
		sem_post(&product_sem);
	}
}


void consumer_run(void *arg)
{
	while (1)
	{
		int data;
		sleep(2);
		sem_wait(&product_sem);
		pthread_mutex_lock(&mutex);
		//消费者从缓冲池读取数据
		data = buffer[bRead];
		bRead = (bRead + 1) % BUFFER_SIZE;
		printf("consumer %d read from buffer\n", *(int*)arg);
		printf("buffer size is %d\n",(bWrite-bRead+BUFFER_SIZE)%BUFFER_SIZE);
		pthread_mutex_unlock(&mutex);
		sem_post(&room_sem);
	}
}

int main()
{
	pthread_t producer_id[PRODUCER_NUM];
	pthread_t consumer_id[CONSUMER_NUM];
	pthread_mutex_init(&mutex, NULL);	//初始化互斥量
	//初始化信号量room_sem为缓冲池大小
	if (sem_init(&room_sem, 0, BUFFER_SIZE) != 0)
	{
		printf("sem_init error");
		return 1;
	}
	//初始化信号量product_sem为0，开始时缓冲池中没有数据
	if (sem_init(&product_sem, 0, 0) != 0)
	{
		printf("sem_init error");
		return 2;
	}
	for (int i = 0; i < PRODUCER_NUM; ++i)
	{
		//创建生产者线程
        int* thread_num = new int(i);
		if (((pthread_create(&producer_id[i], NULL, &producer_run, thread_num)) != 0)
		{
			printf("producer_id error");
			return 3;
		}
    }
    for (int i =0; i < CONSUMER_NUM; ++i)
    {
		//创建消费者线程
        int* thread_num = new int(i);
		if ((pthread_create(&consumer_id[i], NULL, &consumer_run, thread_num)) != 0)
		{
			printf("consumer_id error");
			return 4;
		}
	}

	for(int i=0;i<PRODUCER_NUM;i++)
	{
		pthread_join(producer_id[i],NULL);
		pthread_join(consumer_id[i],NULL);
	}

    return 0;
}
