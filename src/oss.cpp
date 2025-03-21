#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>

#define MAX_PROCESSES 20

struct SharedClock {
    int seconds;
    int nanoseconds;
};

struct Message {
    long mtype;
    int data;
};

struct PCB {
    bool occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
    int messagesSent;
};

std::vector<PCB> processTable(MAX_PROCESSES);

int main(int argc, char *argv[]) {
    key_t shmKey = ftok("shmfile", 65);
    int shmId = shmget(shmKey, sizeof(SharedClock), 0666 | IPC_CREAT);
    SharedClock *sysClock = (SharedClock*) shmat(shmId, (void*)0, 0);
    sysClock->seconds = 0;
    sysClock->nanoseconds = 0;

    key_t msgKey = ftok("msgqueue", 75);
    int msgId = msgget(msgKey, 0666 | IPC_CREAT);

    int activeProcesses = 0;
    int totalProcesses = 5;  // Example limit

    while (activeProcesses < totalProcesses) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (!processTable[i].occupied) {
                pid_t pid = fork();
                if (pid == 0) {
                    execl("./worker", "./worker", "2", "500000", NULL);
                    exit(0);
                }
                processTable[i] = {true, pid, sysClock->seconds, sysClock->nanoseconds, 0};
                activeProcesses++;
                break;
            }
        }
    }

    while (activeProcesses > 0) {
        sysClock->nanoseconds += 250000000 / activeProcesses;
        if (sysClock->nanoseconds >= 1000000000) {
            sysClock->seconds++;
            sysClock->nanoseconds -= 1000000000;
        }

        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processTable[i].occupied) {
                Message msg = {processTable[i].pid, 1};
                msgsnd(msgId, &msg, sizeof(msg.data), 0);
                processTable[i].messagesSent++;
                
                msgrcv(msgId, &msg, sizeof(msg.data), processTable[i].pid, 0);
                if (msg.data == 0) {
                    waitpid(processTable[i].pid, NULL, 0);
                    processTable[i].occupied = false;
                    activeProcesses--;
                }
            }
        }
    }
    
    msgctl(msgId, IPC_RMID, NULL);
    shmctl(shmId, IPC_RMID, NULL);
    return 0;
}
