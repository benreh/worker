/*	Copyright 2010 Benjamin Reh <ich@benjaminreh.de>
 * 	This file is part of 'Worker'.
 *
 *   'Worker' is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 3 of the License.
 *
 *    'Worker' is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with 'Worker'.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"
#include "shm.h"

Shm::Shm() {
	//default: one worker
	no_workers=1;
	string filename=get_filename();
	//create a file, if it does not already exist
	struct stat stFileInfo; 
	int intStat;
	intStat = stat(filename.c_str(),&stFileInfo);
	if (intStat!=0) {
		ofstream file;
		file.open(filename.c_str());
		file.close();
	}
	//this was a really bad hack...
	/*
	string cmd=string("touch ");
	cmd+=get_filename();
	system (cmd.c_str());*/
	
	lock();
	// Kcreate ey
	if ((key = ftok(filename.c_str(), 'R')) == -1) 
		perror("ftok");
	// connect to shared mem and create if not already existing
	if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) 
		perror("shmget");
	data = (struct exchange_t*)(shmat(shmid, (void *)0, 0));
	// create semaphors 
	if ((semid = semget(key, 5, 0666 | IPC_CREAT)) == -1) 
		perror("semget");
}
void Shm::init() {
	//if not initialized
	if (data->is_initialized!=INIT_DATA){
		//Trying to determine the number of workers
		ifstream cpuinfo;
		cpuinfo.open("/proc/cpuinfo");
		if (cpuinfo) {
			no_workers=0;
			cout << "reading from \"/proc/cpuinfo\"" << endl;
			while (!cpuinfo.eof()) {
				string info;
				cpuinfo >> info;
				//~ cout << info << endl;
				if (info == "processor")
					no_workers++;
			}
			cpuinfo.close();
		}
		// initialize
		if (semctl(semid, 0, SETVAL, 1) == -1) 
			perror("semctl");
		if (semctl(semid, 1, SETVAL, no_workers) == -1) 
			perror("semctl");
		data->worker_running=0;
		data->is_initialized=INIT_DATA;
		//some debugging stuff
		cout << "shmid " << shmid << " semid " << semid << endl;
		cout << "Number of Workers " << no_workers << endl;
	}
	unlock();

}

Shm::~Shm() {

}


void Shm::destroy(void) {
	// delete semaphores
	if (semctl(semid, 0, IPC_RMID, NULL) == -1) 
		perror("semctl");
	if (semctl(semid, 1, IPC_RMID, NULL) == -1) 
		perror("semctl");
	//delete shm 
	if (shmctl(shmid, IPC_RMID, NULL)==-1)
		perror("shmctl");
	data->is_initialized=0;
}

//Lock shm for exclusive access
void Shm::lock() {
	struct sembuf s_shm_lock = {0, -1, 0};
	semop(semid, &s_shm_lock, 1);
}

//unlock shm from exclusive access
void Shm::unlock() {
	struct sembuf s_shm_unlock = {0, 1, 0};
	semop(semid, &s_shm_unlock, 1);
}

//Wait for a "slot" to became ready
void Shm::wait() {
	struct sembuf s_shm_lock = {1, -1, 0};
	semop(semid, &s_shm_lock, 1);
	lock();
	data->worker_running++;
	unlock();
}
//Signalize to have finished working
void Shm::put()
{
	lock();
	data->worker_running--;
	unlock();
	struct sembuf s_shm_unlock = {1, 1, 0};
	semop(semid, &s_shm_unlock, 1);
}

//retrieve unique filename
string Shm::get_filename() {
	string f= "/tmp/worker.";
	f+=getenv("USER");
	return f;
}

//Wait for all workers to have finished
void Shm::join() {
	bool end=false;
	while (!end) {
		lock();
		if (data->worker_running==0)
			end=true;
		unlock();
		//sleep is not very beautiful, I know
		if (!end) {
			sleep(1);
		}
	}
}
