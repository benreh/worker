/*	Copyright 2010 Benjamin Reh <ich@benjaminreh.de>
 * 	This file is part of 'Worker'.
 *
 *   'Worker' is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 3 of the License.
 *
 *    'Worker is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

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
