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
	
	void init(int set_no_workers=0);
	void lock(void);
	void unlock(void);
	void destroy(void);

	void wait(void);
	void put(void);
	void join(void);
	bool isvalid();

	string get_filename();

	struct exchange_t* data;

private:

	key_t key;
	int shmid;
	int semid;
	int no_workers;


};




#endif
