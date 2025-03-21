# OSS Process Simulation

## Overview
This project simulates an Operating System Scheduler (OSS) managing multiple worker processes using shared memory, message queues, and process control in C++.

## Features
- Shared Clock: Maintains a global system time.
- Process Control Block (PCB): Tracks process details.
- Message Queue: Enables inter-process communication.
- Process Scheduling: Creates, tracks, and terminates worker processes.

## Prerequisites
Ensure you have the following installed:
- GCC (g++)
- GNU Make
- Linux (Ubuntu recommended)

## Folder Structure
```
Backend/oss/
├── src/
│   ├── oss.cpp      # Main scheduler
│   ├── worker.cpp   # Worker process
├── bin/
│   ├── oss         # Compiled binary for OSS
│   ├── worker      # Compiled binary for worker
├── Makefile        # Build automation script
├── README.md       # Documentation
```

## Compilation & Execution
### Build the project:
```bash
make
```

### Run the OSS system:
```bash
./bin/oss
```

### Clean the build:
```bash
make clean
```

## Expected Output
```bash
OSS: Initializing shared memory and message queue...
OSS: Launching process 1 at time 0:0
OSS: Process 1 completed at time 2:500000000
OSS: Cleaning up shared memory and message queue...
```

## Cleanup Resources
To manually remove shared memory and message queue:
```bash
ipcrm -M <shm_id>
ipcrm -Q <msg_id>
```
Replace `<shm_id>` and `<msg_id>` with actual IDs from `ipcs -m` and `ipcs -q`.

## Author
Talha

