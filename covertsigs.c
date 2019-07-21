
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>


// CONSTANTS

#ifndef SINGLE  // Using -DSINGLE value
#define SINGLE 0
#endif
#define ALARM_MICRODELAY 10000 // Constant for delay inputs

// GLOBAL VARIABLES

char message[512]; // char array for receiving a message
int alphabet = 0; // Counter for receiving a character
int charCount = 2;  // index variable for char array
int senderCount = 0; // counter for total signals being send
int recCount = 0; // counter for total signals received
int checkerCount = 0; // counter for checking test count
int flag = 0; // flag to indicate if a message is currently being received
int sigUserCount = 0; // flag to indicate we have sent entire message
int currently_checking = 0; // flag to indicate we are currently error checking
// Delay function used for communication process
void delay(unsigned int mseconds)
{
	clock_t goal = mseconds + clock();
	while (goal > clock());
}

// SIGNAL HANDLER
static void SigHandler(int signo){
	switch (signo) {
		case SIGUSR1:
			// are we currently error checking?
			if(currently_checking){
				checkerCount++;
				if (SINGLE) {
					// send SIGALRM signal
					ualarm(ALARM_MICRODELAY , 0);
				}
			}else{
				// set flag to indicate user is receiving a message
				// and senderCount = 0 and recCount == 0, tried and failed?
				if(!flag && senderCount == 0 && recCount == 0){
					flag = 1;
				}
				// Increment counters
				alphabet++;
				recCount++;
				
				if (SINGLE) {
					// send SIGALRM signal
					ualarm(ALARM_MICRODELAY , 0);
				} else {
					sigUserCount = 0; // set flag to 0 to receive next message
				}
			}
			break;

		case SIGUSR2:
			// check if this is the first SIGUSR2 or the SIGUSR2 being sent 	
	
			// if single char being sent	
			if(sigUserCount < 1){
				// Push character to message char array
				message[charCount] = alphabet;
				message[charCount + 1] = '\n';
				message[charCount + 2] = '\0';
				charCount++;  // increment pointer to next char
				alphabet = 0; // reset letter counter
				sigUserCount++; // set flag to 1
			// if entire message sent
			}else{
				if (!currently_checking){
					// start checking for errors
					currently_checking = 1;
				}else{					
					if (checkerCount == recCount % 71) { 
					message[0] = '!';
					} else{
					message[0] = '?';
					}

					message[1] = ' ';
					// display message and flush stdout
					printf("%s", message);
					fflush(stdout);
					// reset counters
					charCount = 2;
					recCount = 0;
					checkerCount = 0;
					currently_checking = 0;
					flag = 0; 
				}
			}
			break;

		case SIGALRM:
			// if single char being sent
			if (alphabet > 20) {
				// Push character to message char array
				message[charCount] = alphabet;
				message[charCount+1] = '\n';
				message[charCount+2] = '\0';
				charCount++;
				alphabet = 0;  // reset letter counter
			} else {
				// pad beginning of message and perform error check

				// Are we currently checking for errors?
				if (!currently_checking){
					// start checking for errors
					currently_checking = 1;
				}
				else{
					if (checkerCount == (recCount - 1) % 71) { 
					message[0] = '!';
					} else{
					message[0] = '?';
					}

					message[1] = ' ';
					// display message and flush stdout
					printf("%s", message);
					fflush(stdout);
					// reset counters
					alphabet = 0; 
					charCount = 2;  
					recCount = 0;
					checkerCount = 0;
					flag = 0;
					currently_checking = 0;
				}
			}
			break;
	}
}


int main(void) {
	// Initialize main loop variables
	char input;  // char for input
	int* usercompid; // int for the id of the user to be communicated with
	usercompid = malloc(sizeof(int));
	struct sigaction action; // initialize sigaction
	action.sa_handler = SigHandler; // Define signal handler function

	// Initialize set for the sigaction mask
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGUSR1);

	// Initialize Sa_flags
	action.sa_flags = 0;
	action.sa_flags |= SA_RESTART;
	
	// Display PID to user
	printf("Own PID: %d\n", getpid());

	// Prompt user to enter pid of other messager
	scanf("%d", usercompid);
	
	// sigaction (What type of signal to handle, what handles the signals in the future)
	sigaction(SIGTSTP, &action, NULL);
	sigaction(SIGUSR1, &action, NULL);
	// If DSINGLE used, use SIGALRM for communication. SIGUSR2 otherwise
	if (SINGLE) {			
		sigaction(SIGALRM, &action, NULL);
	} else {
		sigaction(SIGUSR2, &action, NULL); 
	}

	scanf("\n");

	// MAIN LOOP
	for(;;) {
		// Use scanf to take message input
		scanf("%c", &input);
		
		// if message is being sent, wait for message to be finished receiving
		while(flag != 0) {
			scanf("%c", &input);
		}
		
		// check if user is exiting program
		if (input == '.') {
			// exit program if user entered '.'
			free(usercompid);
			return 0;
		}

		// Delay before sending message
		if (SINGLE) {
			delay(ALARM_MICRODELAY + 1500);
		} else {
			delay(1000);
		}

		// while loop for sending signals to other user
		while (input != '\n') {
			// use counter to count number of signals being sent with respect to char's number value
			int i = 0;
			// Sending single char
			while (i<input) {
				// send sigusr1 i amount of times for each char
				kill(*usercompid, SIGUSR1);
				delay(100);                        
				i++;
				senderCount++;
			}
			// communicate char has been sent
			if (SINGLE) {
				scanf("%c", &input);
				delay(ALARM_MICRODELAY + 1500);
			} else {
				kill(*usercompid, SIGUSR2);
				scanf("%c", &input);
				delay(1000);
			}
		}
		// Communicate entire message has been sent
		if (SINGLE) {
			delay(100);
			kill(*usercompid, SIGUSR1);
			delay(100);
			delay(ALARM_MICRODELAY + 1500);
			

			senderCount %= 71;
			int j = 0;
			// Sending count
			while (j < senderCount) {
				// send sigusr1 j amount of times for sendercount
				kill(*usercompid, SIGUSR1);
				delay(100);                        
				j++;
			}
			// communicate char has been sent
			senderCount = 0;
			delay(ALARM_MICRODELAY + 1500);

		} else {
			kill(*usercompid, SIGUSR2);
			delay(100);
			senderCount %= 71;
			int j = 0;
			// Sending count
			while (j < senderCount) {
				// send sigusr1 j amount of times for sendercount
				kill(*usercompid, SIGUSR1);
				delay(100);                        
				j++;
			}
			// communicate char has been sent
			senderCount = 0;
			kill(*usercompid, SIGUSR2);
			delay(100);
		}

	}

	free(usercompid);
	return 0;
}
