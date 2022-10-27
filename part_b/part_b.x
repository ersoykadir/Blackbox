/*
* How to save compile and run:
* rpcgen -C add.x // -C for creating all the files, generate code in ANSI C.
* rpcgen -a add.x // -a generate all the files including sample code for client and server side.
* make -f Makefile.add // -f to use given file as a makefile.
*/


/* Holds input data send from client to server */
struct inputData{
	string path<>;
	string outfile_name<>;
	int a;
	int b;
};
/* Holds output data send from server to client */
struct outputData{
	int result;/* result of the blackbox */
	int error;/* binary number to check if an error occured */
	string error_message<>;
};


/*
* 1.	Name the program and give it a unique number.
* 2.	Specify the version of the program.
* 3.	Specify the signature of the program.
*/

program PART_B_PROG{
	version PART_B_VERS{
		/* Takes a numbers structure and gives the integer result. */
		outputData part_b(inputData) = 1;
	}=1;
}=0x12345678;