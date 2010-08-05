#define MAIN_C
#include "global.h"
#include "shm.h"


int main(int argc, char *argv[]) {
	//cmdline is the string, that will be executed later
	string cmdline;
	for(int i=1; i < argc; i++) {
		cmdline+=argv[i];
		cmdline+=" ";
	}
	//Creating shared mem and semaphores
	Shm shm;
	//Special case, remove shm
	if (argc>1 && ((string) argv[1]) == "--remove") {
		cout << "deleting shm" << endl;
		shm.destroy();	
		exit(0);	
	}
	//initializing shm
	shm.init();
	//Special case, waiting for all processes to have finished.
	if (argc>1 && ((string) argv[1]) == "--join") {
		shm.join();
	} else {
		//Wait, if there are too many workers around
		shm.wait();
		//fork itself
		int f=fork();
		//in case of being the child...
		if (f==0) {
			cout << "Calling " << cmdline << endl;
			//execute commadline
			system(cmdline.c_str());
			shm.put();	
		}
	}




}
