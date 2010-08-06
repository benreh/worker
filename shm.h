#ifndef SHM_H
#define SHM_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fstream>

#define SHM_SIZE (sizeof(struct exchange_t))
//Still hardcoded, will change in the hopefully future
//~ #define NO_WORKER 8
//Some Magic-bytes
#define INIT_DATA 123456

//struct that contains the data in the shared mem
struct exchange_t {
	int is_initialized;
	int worker_running;
	//~ char cmdline[1024];
};



//Class for handling shm and semaphore actions
class Shm {
public:
	Shm(void);
	~Shm(void);
	
	void init(void);
	void lock(void);
	void unlock(void);
	void destroy(void);

	void wait(void);
	void put(void);
	void join(void);

	string get_filename();

	struct exchange_t* data;

private:

	key_t key;
	int shmid;
	int semid;
	int no_workers;


};




#endif
