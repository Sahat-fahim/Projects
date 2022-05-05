#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define GREEN 5
#define YELLOW 6
#define RED 7
#define pRED 9
#define pGREEN 8

volatile int state= RED;
volatile int pState= pRED;
int button;

void* trafficLight();
void* pedestrianLight();


int main()
{

  srand(time(NULL));
  button = rand()%2;
  printf("Pedestrian button state= %d\n",button);

  pthread_t traffic, pedestrian; 
  pthread_create(&traffic, NULL, &trafficLight, NULL);
  pthread_create(&pedestrian, NULL, &pedestrianLight, NULL);
  pthread_join(traffic, NULL);
  pthread_join(pedestrian, NULL);

  return 0;
}



void* trafficLight()
{
  while (button==0) // if button is not pressed we are concurrently running this loop 1
  {
    switch (state)
  {
    case RED:
    printf("Red\n");
    sleep(2);
    
    case YELLOW:
    printf("Yellow\n");
    sleep(2);
       
    case GREEN:
    printf("Green\n");
    sleep(2);

    printf("Yellow\n");
    sleep(2);
  }
  }
  
  while (button == 1) // if button is pressed we are concurrently running this loop 2
  {
    printf("Red on\n");
    sleep(2);
  }

}

void* pedestrianLight()
{

  while (button== 0) // if button is not pressed we are concurrently running this loop 1
  {
  switch(pState)
  {
   case pRED:
   printf("pRed on\n");
   sleep(2);

   case pGREEN:
   printf("pGreen off\n");
   sleep(2);
  }
  }
  while (button== 1) // if button is pressed we are concurrently running this loop 2
  {
  switch(pState)
  {
   case pRED:
   printf("pRed off\n");
   sleep(2);

   case pGREEN:
   printf("pGreen on\n");
   sleep(2);
  }
  }
}
