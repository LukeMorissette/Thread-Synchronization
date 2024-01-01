1)
a. Luke Morissette
b. 2433034
c. lmorissette@chapman.edu
d. 380
e. Thread Synchronization
2) buffer.h, buffer.c, Checksum.h, Checksum.c
3) None
4) Usd code from stack overflow to write mutex, sempahores, and error messages
Collaborated with Jason Bollinger
5) 
gcc *.c -o prodcon -lpthread -lrt
./prodcon <int delay> <int num_producers> <int num_consumers>