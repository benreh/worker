#ifndef SHM_H
#define SHM_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_SIZE (sizeof(struct exchange_t))

#define NO_WORKER 8
#define INIT_DATA 123456

struct exchange_t
{
	int is_initialized;
	//~ int worker_requested;
	//~ int worker_available;
	int worker_running;
	char cmdline[1024];
};




class Shm 
{
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


};




#endif
