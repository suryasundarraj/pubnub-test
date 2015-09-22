//Import the Libraries Required 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <termios.h> 
#include <pthread.h>
#include "pubnub_sync.h"

//Function Prototypes used in this program
void *pubnub_receive(void* p_unused);

/*********************************************************************************

Function Name 		:	pubnub_receive
Description		:	Suscribe from the "parkingstatus-req" channel using the 
				pubnub and The Request is sent fromt the Android App to 
				get the present status of the parking lots.
				This is seperate thread along with the main thread.
Parameters 		:	void
Return 			:	(void *) when the pubnub connection fails returns -1

*********************************************************************************/

void *pubnub_receive(void* p_unused)
{
	char const *l_msg = NULL;
	enum pubnub_res l_res;
	char const *l_requestChannel = "parkingstatus-req";
	pubnub_t *l_receive = pubnub_alloc();
    if (NULL == l_receive) 
    {
        printf("Failed to allocate Pubnub context!\n");
        return (void *)-1;
    }
    pubnub_init(l_receive, "demo", "demo");
	while(1)
    {
		l_res = pubnub_subscribe(l_receive, l_requestChannel, NULL);
		if (l_res != PNR_STARTED) 
		{
	        printf("pubnub_subscribe() returned unexpected: %d\n", l_res);
	        pubnub_free(l_receive);
	        return (void *)-1;
	    }
		
		l_res = pubnub_await(l_receive);
		if (l_res == PNR_STARTED) 
		{
	        printf("pubnub_await() returned unexpected: PNR_STARTED(%d)\n", l_res);
	        pubnub_free(l_receive);
	        return (void *)-1;
	  	}
		if (PNR_OK == l_res) 
		{
	        puts("Subscribed! Got messages:");
	        for (;;) 
	        {
	            l_msg = pubnub_get(l_receive);
	            if (NULL == l_msg) 
	            {
	                break;
	            }
	            puts(l_msg);
		  	}
	    }
	    else 
	    {
	    	printf("Subscribing failed with code: %d\n", l_res);
	    } 
	}
	pubnub_free(l_receive);
    return NULL;
}

/*********************************************************************************

Function Name 		:	main
Description		:	Initalize UART, Thread and publish if any status change
				in the parking lots
Parameters 		:	void
Return 			:	int, if error in the function returns -1 else 0

*********************************************************************************/

int main(void)
{
	pthread_t thread_id;
	pthread_create(&thread_id,NULL,&pubnub_receive,NULL);
   	pthread_join(thread_id,NULL);
	return 0;
}

//End of the Program

