# serverClient
serverClient using Boost_1.66



The goal of this task is to implement a part of a server code that handles input commands and provided output results using via a binary protocol. Each command parameter <...> represents a four byte signed integer in network byte order* sent or received. All client -> server and server -> client commands are prefixed with the number of fields in the command (detailed below). All set ids, keys, and scores will be positive.
Your server should implement the following commands:
1. Enable: Server is ready process any input command, if a another command is received while the server is not enabled, it shall log an error and drop the command (no response)
Client: <1> <1>
Server: <0> 
2. Add: Add value <value> to a dictionary at the given key <key>, if <key> is already in the dictionary its value is updated.
Client: <3> <2> <key> <value>
Server: <0> 
3. Remove: Removes <key> from the dictionary.
Client: <2> <3> <key>
Server: <0>
4. Get Count: Returns the number of entries in the dictionary.
Client: <1> <4> 
Server: <1> <count>
5. Get value: Returns the value <value> of key <key> in the dictionary, and -1 if does not contain <key>.
Client: <2> <5> <key>
Server: <1> <value>
6. Disable: Server does not process any other input commands
Client: <1> <6>
Server: No response 
 Use C++ and std library for the implementation. 
One class shall be implemented with constructor, destructor and one public method taking as input a buffer and its size (e.g. ProcessCommand(uint8_t *buffer, int32_t size)). This method shall convert the byte buffer to a list of integers taking into account the endianness and checking the number of arguments in the command the buffer size, if the buffer is longer – shorter than expected or the command type is invalid  an error shall be logged and the command shall be dropped (no response)
For each type of command a private method shall be implemented taking the respective arguments
The server class shall have a method (namely SendResponse(uint8_t *buffer, int32_t size)) that shall log the output data to the console.
The main program function shall call the ProcessCommand method providing the following inputs
Sample Input (as integers not binary representation of input bytes)
1 1 
3 2 10 5000
1 4
3 2 11 11000
1 4
3 2 10 10000
1 4
2 5 10
2 5 11
2 3 11
1 4
1 6
Sample Output
0
0
1 1
0
1 2
0
1 2 
1 10000
1 11000
0
1 1
-

Explanation
The first line of input has the values
3 2 10 5000 
3 denotes the number of values in the command, 2 corresponds to the ADD command, 10 is the key and 5000 is the value to add.
