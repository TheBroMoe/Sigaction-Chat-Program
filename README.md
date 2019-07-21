
# How to run program:

*   Enter in the terminal 'make covertsigs.single or make covertsigs.double'
  to compile the program
* Execute program by running './covertsigs.single' or './covertsigs.double'
   

# Design Questions:

1. How do you code messages for the onesignal option (only SIGUSR1 used)?
   We use SIGUSR1 to send the number of signals equal to each specifc character's ascii number.
   In our SIGUSR1 handler, we increment a counter that keeps track of the number of received signal
   and push the resulting number into a character array that contains our message string that we input.
   We used SIGALRM and delays to assist with receiving and displaying the message, using the timings
   to mark the end of a character and end of the message.

2. How do you code messages for the two signals option (both SIGUSR1 and SIGUSR2 used)?
   Our process for covertsigs.double is similar to the process used in covertsigs.single except
   we used SIGUSR2 instead of SIGALRM to assist with receiving and displaying the message. SIGUSR2 
   gets counted to know when its the end of a character versus the end of a message.
   
3. How do you represent message boundaries?
   We currently have our character array bounded at 512 chars as such users will lose their message 
   if they input a message that surpasses that bound.
   
4. How do you check for errors?
   After sending our message, we send a modulated integer (modded by 71) that contains the total number of signals sent.
   we also count the number of signals received as well as seperately count the modulated integer sent by the sender.
   If the numbers match, no errors have occured and the message was received successfully.
   
5. What errors may you not be able to catch?
   It is not possible to be sure if every single signal was successfully checked or if there was an issue with our sigalrm delay
   check and as a result not all signals will necessarily be accounted for and somtimes are sent when they should not be. 
   Similarly our error checking uses a small hash (modding by 71) which would result in many collisions, and therefore
   some errors may pass as being a success.
   
6. How do you handle the interleaving of input/output?
   We print out outputs from within the signal handler and inputs go into the main loop in the main function. 
   We used a flag to indicate if we are currently in the process of receiving a message and refrain from being 
   able to send signals or messages if we are receiving a message.

7. Is there any other signal you had to use(and why)? -- such signal is of course not allowed for communication
   We had to use SIGALRM for dealing with delays within our main loop function for covertsigs.single.
   
8. How do you ensure that you do not spend unnecessary CPU cycles?
   We try to acheive multiple tasks simultaneously. Though our design could likely be optimized if we
   used smaller delays, reducing CPU cycles.

Work Balance between Partners:
Mohammad Kebbi:
    * Implemented initializing sigaction
    * Implemented covertsigs.double
    * Implemented error checking
    * Implemented blocking functionality
    
Zachary Kist:
    * Implemented initializing sigaction
    * Implemented covertsigs.double
    * Implemented error checking
    * Implemented blocking functionality
