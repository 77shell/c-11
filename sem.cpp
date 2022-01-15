extern "C" {
#include <semaphore.h>
}

#include <iostream>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

int
task_a(sem_t *psem)
{
	for (;;)
	{
		if (sem_trywait(psem) == -1) {
			perror("task_a sem_trywait");
		}
		else {
			cout << __func__ << " sem_trywait successful\n";
		}
		this_thread::sleep_for(1s);
	}
}

int
task_b(sem_t *psem)
{
	int sval;

	for (;;)
	{
		if (sem_getvalue(psem, &sval) == 0) {
			cout << __func__ << " sval " << sval << endl;
		}
		else {
			perror("sem_getvalue");
		}

		if (sem_post(psem) == -1) {
			perror("task_b sem_post");
		}
		else {
			cout << __func__ << " sem post successful\n";
		}
		this_thread::sleep_for(2s);
	}
}	

int
main(int argc, char *argv[])
{
	sem_t sem;
	if (sem_init(&sem, 0, 1) == -1) {
		perror("main");
	}

	int sval;
	if (sem_getvalue(&sem, &sval) == -1) {
		perror("sem_getvalue");
	}
	else {
		printf("Semaphore value : %d\n", sval);
	}

	if (sem_wait(&sem) == -1) {
		perror("sem_wait");
	}
	else {
		printf("sem_wait successful\n");
	}

	if (sem_getvalue(&sem, &sval) == -1) {
		perror("sem_getvalue");
	}
	else {
		printf("Semaphore value : %d\n", sval);
	}

	for (int i = 0; i < 3; i++) {
		if (sem_trywait(&sem) == -1) {
			perror("sem_trywait");	
		}
		else {
			cout << "sem_trywait successful\n";
		}
	}

	thread tha {task_a, &sem};
	thread thb {task_b, &sem};
	tha.join();
	thb.join();
	return 0;
}
