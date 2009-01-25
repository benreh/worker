#include "global.h"
#include "shm.h"



Shm::Shm()
{
	string filename;
	filename = "/tmp/worker.";
	filename+=getenv("USERNAME");
	lock();
	// Key erstellen
	if ((key = ftok(filename.c_str(), 'R')) == -1) 
		perror("ftok");
	// mit Shared mem verbinden und erstellen, falls nicht vorhanden
	if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) 
		perror("shmget");
	data = (struct exchange_t*)(shmat(shmid, (void *)0, 0));
	// Semaphoren erstellen
	if ((semid = semget(key, 5, 0666 | IPC_CREAT)) == -1) 
		perror("semget");
}
void Shm::init()
{
	if (data->is_initialized!=INIT_DATA)
	{
		// Inititialisieren
		if (semctl(semid, 0, SETVAL, 1) == -1) 
			perror("semctl");
		if (semctl(semid, 1, SETVAL, NO_WORKER) == -1) 
			perror("semctl");
		data->is_initialized=INIT_DATA;
		cout << "shmid " << shmid << " semid " << semid << endl;
	}
	unlock();

}

Shm::~Shm()
{

}


void Shm::destroy(void)
{
	// Semaphore löschen
	if (semctl(semid, 0, IPC_RMID, NULL) == -1) 
		perror("semctl");
	if (semctl(semid, 1, IPC_RMID, NULL) == -1) 
		perror("semctl");
	//shm löschen
	if (shmctl(shmid, IPC_RMID, NULL)==-1)
		perror("shmctl");
	data->is_initialized=0;
}

void Shm::lock()
{
	struct sembuf s_shm_lock = {0, -1, 0};
	semop(semid, &s_shm_lock, 1);
}
void Shm::unlock()
{
	struct sembuf s_shm_unlock = {0, 1, 0};
	semop(semid, &s_shm_unlock, 1);
}

void Shm::wait()
{
	struct sembuf s_shm_lock = {1, -1, 0};
	semop(semid, &s_shm_lock, 1);
}
void Shm::put()
{
	struct sembuf s_shm_unlock = {1, 1, 0};
	semop(semid, &s_shm_unlock, 1);
}

