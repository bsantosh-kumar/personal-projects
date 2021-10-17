#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define NO_OF_READERS 3
pthread_mutex_t rw_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r_mutex=PTHREAD_MUTEX_INITIALIZER;
int readers_count=0;
void *writer_function(void *argv){
	int no_of_writes=5;
	while(no_of_writes){
		pthread_mutex_lock(&rw_mutex);
		printf("Writing...\n");
		no_of_writes--;
		sleep(2);
		pthread_mutex_unlock(&rw_mutex);
		sleep(5);
	}	
}
void *reader_function(void *id){
	int no_of_readings=2;
	int index=*(int *)id;
	while(no_of_readings){
		pthread_mutex_lock(&r_mutex);
		readers_count++;
		if(readers_count==1)
			pthread_mutex_lock(&rw_mutex);
		pthread_mutex_unlock(&r_mutex);
		printf("%d reading...\n",index);
		no_of_readings--;
		sleep(rand()%3);
		pthread_mutex_lock(&r_mutex);
		readers_count--;
		if(readers_count==0)
			pthread_mutex_unlock(&rw_mutex);
		pthread_mutex_unlock(&r_mutex);
	}
}
int main() {
	srand(time(0));
	pthread_t writer;
	pthread_t readers[NO_OF_READERS];
	pthread_create(&writer,NULL,writer_function,NULL);
	int reader_id[NO_OF_READERS];	
	for(int i=0;i<NO_OF_READERS;i++){
		reader_id[i]=i;
		pthread_create(&readers[i],NULL,reader_function,&reader_id[i]);	
	}
	pthread_join(writer,NULL);
	for(int i=0;i<NO_OF_READERS;i++){
		pthread_join(readers[i],NULL);
	}
}
