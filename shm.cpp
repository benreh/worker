#include "global.h"
#include "shm.h"



Shm::Shm() {
	//this is a really bad hack...
	string filename=get_filename();
	string cmd=string("touch ");
	cmd+=get_filename();
	system (cmd.c_str());
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
	if (data->is_initialized!=INIT_DATA)
	{
		// initialize
		if (semctl(semid, 0, SETVAL, 1) == -1) 
			perror("semctl");
		if (semctl(semid, 1, SETVAL, NO_WORKER) == -1) 
			perror("semctl");
		data->worker_running=0;
		data->is_initialized=INIT_DATA;
		//some debugging stuff
		cout << "shmid " << shmid << " semid " << semid << endl;
		cout << "Number of Workers " << NO_WORKER << endl;
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
	f+=getenv("USERNAME");
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
