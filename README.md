# PBSkip
A proof of concept of parsing protocol buffers data in parallel, led by Professor Zhijia Zhao from UCR.

The following folders are inside the src folder
- schema
	- schemas of protobuf messages
	- The files ends with .proto are the schemas
	- The other files are the generated by using the protoc command. 
- generated
	- generated code files by running `python genFromProto.py`, which reads a .proto file to generate our versions of the protobuf message parsing functions.
- google_api
	- Parses the entire message sequentially with Google's ParseFromIstream() function.
- sequential
	- Parses the entire message sequentially with our own ParseProfile() function.
- parallel
	- Parses the message by first traversing through the entire message sequentially and marking the positions and lengths of the submsesages. Then parse the submessages in parallel at its respective positions. 
	- This is done through parallel_for from the parlaylib library, which uses its own special scheduling algorithms for parallelism.
- pthreads
	- Similar method as parallel, but the scheduling is more straightforward. 
	- Based on the number of threads requested, each thread will parse approximately (size_of_Profile/num_threads) bytes of data. 
	- These parallel versions are faster than the sequentials when there are a lot of cores and the CPU is not as modern as the best CPUs.
- dac
	- Divide and Conquer Method
	- So far, it only uses 2 threads to parse the first and second halves of the encoded message
	- The first half will always parse correctly, as it starts from the 0th position.
	- The second thread will parse from (size_of_profile/2)th position. This may parse incorrectly, so the second thread will retry after attempting to read the tag and the field_id and wire_type does not exist in the schema. If it does exist, it will attempt to parse.
	- Whenever the parsing fails, it will start at 1 position to the right, and repeat.
	- Still under development.
		- Ideally, there can be $T$ arbitrary number of threads, and the threads will parse ~$n/T$ bytes, where $n$ is the total number of bytes in the message. The $i$-th thread should be able to parse and verify the validity of the starting position of the ($i+1)$-th thread.
