Thread Synchronization Project

Files
   - buffer.h, buffer.c
   - Checksum.h, Checksum.c
   - main.c (the producer-consumer logic)

Dependencies
   - POSIX Threads (pthread)
   - POSIX Semaphores (semaphore.h, -lrt)
   - Standard C libraries (stdio, stdlib, fcntl, unistd)

Description
   This program demonstrates a producer-consumer system using POSIX threads and semaphores. It creates a bounded buffer (`buffer[]`) and coordinates concurrent access with:
   - A mutex (`pthread_mutex_t`) to protect critical sections.
   - Two semaphores (`empty` and `full`) to track available buffer slots and filled slots.
   - Producer threads that generate data items, calculate a checksum, and insert them into the buffer.
   - Consumer threads that remove items from the buffer and handle them as needed.

   Command-Line Arguments:
./prodcon <int delay> <int num_producers> <int num_consumers>

where:
- `delay` is the time (in seconds) the program waits before unlinking semaphores and terminating.
- `num_producers` is how many producer threads to create.
- `num_consumers` is how many consumer threads to create.

Compilation
Use the following commands to compile and run:
gcc *.c -o prodcon -lpthread -lrt ./prodcon <int delay> <int num_producers> <int num_consumers>
Example:
gcc *.c -o prodcon -lpthread -lrt ./prodcon 5 3 2

Usage Details
- **Producer Threads**: Sleep for a random time (100–1000 ms), generate 30 bytes of random data, compute a checksum, and insert the item into the shared buffer.
- **Consumer Threads**: Sleep for a random time (100–1000 ms) and remove an item from the shared buffer.
- **Buffer Synchronization**: 
  - `empty` semaphore tracks how many empty slots are in the buffer.
  - `full` semaphore (used in `remove_item` when waiting for non-zero items) was conceptually planned but is not actively posted in `insert_item`; the design primarily relies on `empty` and `count` checks. 
  - `mutex` is used to lock and unlock the buffer while producers and consumers operate.
- After `delay` seconds, the main thread closes and unlinks the semaphores, ending the program.

Notes
- Ensure you do not interrupt the program before the specified `delay` elapses if you want all producer/consumer threads to complete their tasks.
- The semaphores are named `/empty_sem` and `/full_sem`; on certain systems, they might persist if not properly unlinked.
