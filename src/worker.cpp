#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdlib>

struct SharedClock {
    int seconds;
    int nanoseconds;
};

struct Message {
    long mtype;
    int data;
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./worker <seconds> <nanoseconds>" << std::endl;
        return 1;
    }

    int lifetimeSec = atoi(argv[1]);
    int lifetimeNano = atoi(argv[2]);

    key_t shmKey = ftok("shmfile", 65);
    int shmId = shmget(shmKey, sizeof(SharedClock), 0666);
    SharedClock *sysClock = (SharedClock*) shmat(shmId, (void*)0, 0);
    
    key_t msgKey = ftok("msgqueue", 75);
    int msgId = msgget(msgKey, 0666);

    int terminateSec = sysClock->seconds + lifetimeSec;
    int terminateNano = sysClock->nanoseconds + lifetimeNano;
    if (terminateNano >= 1000000000) {
        terminateSec++;
        terminateNano -= 1000000000;
    }

    std::cout << "WORKER PID:" << getpid() << " PPID:" << getppid()
              << " SysClockS:" << sysClock->seconds << " SysClockNano:" << sysClock->nanoseconds
              << " TermTimeS:" << terminateSec << " TermTimeNano:" << terminateNano
              << " --Just Starting" << std::endl;

    int iterations = 0;
    while (true) {
        Message msg;
        msgrcv(msgId, &msg, sizeof(msg.data), getpid(), 0);

        std::cout << "WORKER PID:" << getpid() << " SysClockS:" << sysClock->seconds
                  << " SysClockNano:" << sysClock->nanoseconds
                  << " --" << ++iterations << " iterations passed" << std::endl;

        if (sysClock->seconds > terminateSec || (sysClock->seconds == terminateSec && sysClock->nanoseconds >= terminateNano)) {
            msg.data = 0;
            msgsnd(msgId, &msg, sizeof(msg.data), 0);
            std::cout << "WORKER PID:" << getpid() << " --Terminating after " << iterations << " iterations." << std::endl;
            break;
        }
        msg.data = 1;
        msgsnd(msgId, &msg, sizeof(msg.data), 0);
    }
    return 0;
}
