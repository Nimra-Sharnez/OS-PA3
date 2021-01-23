Nimra Sharnez
CSCI 3573 
PA3
> multi-lookup.c-> 
>> Progam to resolve a set of hostnames to IP address. 

> multi-lookup.h-> 
>> Header file for multi-lookup.c containing prototypes functions written

> stack.c-> 
>> This is the shared data structure between the requester and resolver threads. It is a stack with pop and push functionality to remove/ add to it.

> stack.h-> 
>> This is the header file for stack.h. It contains a struct that the multi-lookup file utilized to communicate with the stack and have information regarding the stacks current size and the actual stack itself.

> Makefile->
>> Allow compilation and linkage to implementation

> performance.txt->
>> 1 requester thread and 1 resolver thread
>> 1 requester thread and 3 resolver threads
>> 3 requester threads and 1 resolver thread
>> 3 requester threads and 3 resolver threads
>> 5 requester threads and 5 resolver threads
