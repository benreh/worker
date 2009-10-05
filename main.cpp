#define MAIN_C
#include "global.h"
#include "shm.h"


int main(int argc, char *argv[])
{
	string cmdline;
	for(int i=1; i < argc; i++)
	{
		cmdline+=argv[i];
		cmdline+=" ";
	}
	Shm shm;
	if (argc>1 && ((string) argv[1]) == "--remove")
	{
		cout << "deleting shm" << endl;
		shm.destroy();	
		exit(0);	
	}
	shm.init();
	if (argc>1 && ((string) argv[1]) == "--join") {
		shm.join();
	} else {
		shm.wait();
		int f=fork();
		if (f==0)
		{
			cout << "Calling " << cmdline << endl;
			system(cmdline.c_str());
			shm.put();	
		}
	}




}
