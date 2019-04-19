#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<math.h>
#include<unistd.h>
#include<pthread.h>
struct station 
{
int external_passengers;
int internal_passengers;
int boarded_passengers;
pthread_mutex_t lock;
pthread_cond_t passengers_seated_cond;
};
void station_init (struct station *s);
void station_load_train(struct station *s,int count);
void *station_wait_for_train(struct station *s);
void station_on_board(struct station *s);
void *thread_passenger(void *args);//passenger function
void *train_thread(void *args);// train loading function
//initializing the waiting passengers,in train passengers,boareded passengers,lock,condition statements;
void station_init(struct station *s)
{
s->external_passengers = 0;
s->boarded_passengers =0;
s->internal_passengers =0;
pthread_mutex_init(&(s->lock),NULL);
pthread_cond_init(&(s->passengers_seated_cond),NULL);
}
struct train_loading {
	struct station *s;
	int seats_empty;
};
void *train_thread(void *args)
{
	struct train_loading *obj = (struct train_loading*)args;
	station_load_train(obj->s, obj->seats_empty);
	return NULL;
}
volatile int thread_completed =0;
int c;
int outvalue;
int main()
{
struct station s;
station_init(&s);
int i;
printf("\t\t\t\t\t\t--RAILWAY AUTOMATION--\n\n");
printf("Enter the no of waiting passengers at the station \n");//no of waiting passengers taken from the user
scanf("%d",&c);

int total_passengers =c;
for(i=0;i<c;i++)
{
pthread_t thread1;
int res =pthread_create(&thread1,NULL,thread_passenger,&s);//passenger threads
res=pthread_join(thread1,NULL);
if(res!=0)
{
perror("pthread_create");
exit(1);
}
}
printf("\nTotal no of passengers waiting at the station are = %d\n\n",s.external_passengers);//printing the no of waiting passenger
int j;
LOOP :do
{
int f,value;
f=10;
struct train_loading args = { &s,  f};
pthread_t thread2;
int e = pthread_create(&thread2, NULL,train_thread, &args);
if (e != 0) {
perror("pthread_create");
exit(1);
        }
printf("No of passengers entering inside the train =  %d\n",s.internal_passengers);
station_on_board(&s);
printf("Boarded passengers = %d\n",s.boarded_passengers);
printf("Total passengers = %d\nUpdated boarded passengers  are = %d\n\n",total_passengers,s.boarded_passengers);//it shows the updated values of the   boarded passengers
}
while(total_passengers > c);
if(s.boarded_passengers == total_passengers)//checking the whether the enter input passengers are equal to boarded or not
{
printf("All passengers are boarded\n");
}
else
{
if(s.boarded_passengers < c )
{
goto LOOP;
}
}
}
void *thread_passenger(void *args)
{
struct station *s = (struct station*)args;
station_wait_for_train(s);
thread_completed++;
}
void *station_wait_for_train(struct station *s)
{
pthread_mutex_lock(&(s->lock));
s->external_passengers++;
/*here in this function it is adding the waiiting passengers at the station 
which the value is taken from the user
*/
pthread_mutex_unlock(&(s->lock));
}
void station_load_train(struct station *s,int count)
{
pthread_mutex_lock(&s->lock);
while(count >0 && s->external_passengers  >0)
{
s->internal_passengers++;//inside train passengers increases
count--;//empty seats decreasing
}
if(count == 0)
{}
pthread_cond_wait(&(s->passengers_seated_cond),&(s->lock));
//all the passengers in the train are in seated condition
pthread_mutex_unlock(&(s->lock));
}
void station_on_board(struct station *s)
{
pthread_mutex_lock((&s->lock));
while(s->internal_passengers >0 && s->boarded_passengers < c)
{
pthread_cond_signal((&s->passengers_seated_cond));
//
s->internal_passengers--;//internal passengers decreasing
s->boarded_passengers++;//boarded passen=ngers increasing
}
pthread_cond_broadcast(&s->passengers_seated_cond);
//unblocking all the passengers fron seated condition to boarded 
pthread_mutex_unlock(&s->lock);

}

