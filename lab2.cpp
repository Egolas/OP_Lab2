#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>

#define SHM_SIZE 64	//共享内存大小
#define SHM_FLG 0644	//共享内存访问权限
#define PRODUCER_NUM 2  //生产者数目
#define CONSUMER_NUM 2  //消费者数目
#define BUFFER_SIZE	 (64/sizeof(int))   //缓冲池大小,可存放int类型数

using namespace std;



int main()
{
	shmget(SHM_KEY,SHM_SIZE,SHM_FLG);

}
