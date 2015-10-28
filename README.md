# OS_Scheduler
Multi-threaded Operating System Scheduler

This project involved three types of Operating System schedulers: Round Robin, FIFO, Priority Queue.
Each scheduler was then analyzed to determine which type works best given the number of CPUs and type of processes.


The processes alternated between "Ready", "Running", and "Waiting" depending on the type of scheduling paradigm.
Each thread accessed the same data structures so they were all managed by a mutex lock so that the structures are protected.
