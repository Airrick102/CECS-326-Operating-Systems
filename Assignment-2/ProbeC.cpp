#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <time.h>
#include "MsgBuffer.h"
#include "MagicSeeds.h"
#include "kill_patch.h"

using namespace std;

int main()
{
  //Intialize Randomizer
  srand(time(NULL));

  //Message Buffer & Message kill
  buffer msg, msgkill;
  int size = sizeof(msg) - sizeof(long);

  //Generate qid for the queue
  int qid = msgget(ftok(".", 'u'), 0);

  // First message sent from ProbeC (Terminating message)
  msgkill.mtype = 777;

  //copy the pid to kill Probe C
  strncpy(msgkill.message, "Probe C Terminated", size);

  //intializes kill_patch to send to the queue
  kill_patch(qid, (struct msgbuf *)&msgkill, size, msgkill.mtype);

  msg.mtype = rhoSeed; // magic seed set as 251

  //Condition to terminate Probe C
  bool terminateProbeC = false;

  // ProbeC will continue sending messages until user sends the kill command
  while (!terminateProbeC)
  {
    int seedCheck = rand();         // Generating random value
    msg.mtype = rhoSeed;            // Probe C
    if (seedCheck % msg.mtype == 0) // If random value is divisible by magic seed and if so it will be sent to the Hub
    {
      strncpy(msg.message, "Hello DataHub, I'm Probe C", size); // copy the ProbeC's pid to the char array in the message
      msgsnd(qid, (struct msgbuf *)&msg, size, 0);              //sends the message to the queue
      cout << "(" << getpid() << ") "
           << "Message Sent: " << msg.message << " Type: " << msg.mtype << " " << endl;
    }
  }
  return 0;
}
