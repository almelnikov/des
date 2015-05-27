#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <thread>
#include "generators.hpp"
#include "scheduler.hpp"

void Arrive(void*);
void Accept(void*);
void Leaving(void*);
void Release(void*);


void Arrive__model2(void*);
void Accept_model2(void*);
void Leaving_model2(void*);
void Release_model2(void*);
void Promotion_model2(void *);

struct task_data
{
	int n;
	int v;
	double duration;
	double arrive_time;
	double leaving_time;
	std::string tag;
};

Scheduler cal;
std::queue<task_data> task_queue;
std::list<task_data> task_queue_m2;

int task_id = 0;
int MAXN = 100;
int MAXV = 25;
int Q = 0;
int N = MAXN;
int V = MAXV;
double T;
double d;

std::ifstream filein;
std::ofstream resource_m1, resource_m2;
std::ofstream time_m1, time_m2;

void Clear_Simulation()
{
	filein >> MAXN;
	filein >> MAXV;
	filein >> d;
	filein >> T;
	task_id = 0;
	Q = 0;
	N = MAXN;
	V = MAXV;
};

task_data new_task(double arrive_time, double dur, int n, int v)
{
	task_data task;
	task.n = n;
	task.v = v;
	task.duration = dur;
	task.arrive_time = arrive_time;
	task.leaving_time = 0.0;
	task.tag = std::to_string(task_id);
	return task;
}

void print_resources(std::ofstream &strm, double Clock, int n, int v)
{
	strm << Clock << " " << static_cast<double>(n) / MAXN << " "
		 << static_cast<double>(v) / MAXV << "\n";
}

void Arrive(void *empty_ptr)
{
	double Clock = cal.GetTime();
	int n, v;
	double next, duration;

	if (!(filein >> next >> duration >> n >> v)) {
		cal.Stop();
		return;
	}
	task_data task = new_task(Clock, duration, n, v);

	task_id++;

	Q++;
	task_queue.push(task);
	// Проверяется первый, а не последним пришедший
	task = task_queue.front();
	if (Q > 0 && task.n <= N && task.v <= V){
		cal.Schedule(&Accept, nullptr, Clock, task.tag);
	}
	cal.Schedule(&Arrive, nullptr, Clock + next, std::to_string(task_id));
}

void Accept(void *empty_ptr)
{
	task_data task = task_queue.front();
	double Clock = cal.GetTime();
	bool flag_newtask;

	do {
		flag_newtask = false;
		task_queue.pop();
		Q--;
		N -= task.n;
		V -= task.v;
		task_data *task_ptr = new task_data(task);
		cal.Schedule(&Leaving, task_ptr, Clock + task.duration, task.tag);
		if (Q > 0){
			task = task_queue.front();
			if (task.n <= N && task.v <= V){
				flag_newtask = true;
			}
		}
	} while (flag_newtask);
	print_resources(resource_m1, Clock, N, V);
}

void Leaving(void *data_ptr)
{
	task_data *taskptr = static_cast<task_data*>(data_ptr);
	double Clock = cal.GetTime();

	V += taskptr->v;
	print_resources(resource_m1, Clock, N, V);
	if (Q > 0){
		task_data next = task_queue.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept, nullptr, cal.GetTime(), next.tag);
	}
	taskptr->leaving_time = cal.GetTime();
	// Вывод данных о времени обслуживания
	time_m1 << cal.GetTime() - taskptr->arrive_time  << "\n"; 
	cal.Schedule(&Release, data_ptr, Clock + d, taskptr->tag);
}

void Release(void *data_ptr)
{
	task_data *taskptr = static_cast<task_data*>(data_ptr);
	double Clock = cal.GetTime();
	N += taskptr->n;
	print_resources(resource_m1, Clock, N, V);
	if (Q > 0){
		task_data next = task_queue.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept, nullptr, Clock, next.tag);
	}
	delete taskptr;
}

/* Модель короткие вперед, после времени ожидания T задача переходит в начало
   очереди. */

void Arrive_model2(void *empty_ptr)
{
	double Clock = cal.GetTime();
	int n, v;
	double duration;
	double next;

	task_id++;
	if (!(filein >> next >> duration >> n >> v)) {
		cal.Stop();
		return;
	}
	task_data task = new_task(Clock, duration, n, v);

	Q++;
	if (Q > 0 && task.n <= N && task.v <= V){
		task_queue_m2.push_front(task);
		cal.Schedule(&Accept_model2, nullptr, Clock, task.tag);
	}
	else {
		task_queue_m2.push_back(task);
		std::string *tag = new std::string(task.tag);
		cal.Schedule(&Promotion_model2, tag, Clock + T, "p" + task.tag);
		task = task_queue_m2.front();
		if (Q > 0 && task.n <= N && task.v <= V){
			cal.Schedule(&Accept_model2, nullptr, Clock, task.tag);
		}
	}
	cal.Schedule(&Arrive_model2, nullptr, Clock + next, std::to_string(task_id));
}

void Accept_model2(void *empty_ptr)
{
	task_data task = task_queue_m2.front();
	double Clock = cal.GetTime();
	bool flag_newtask;
	
	do {
		flag_newtask = false;
		task_queue_m2.pop_front();
		Q--;
		N -= task.n;
		V -= task.v;
		task_data *task_ptr = new task_data(task);
		cal.Cancel("p" + task.tag, Clock);
		cal.Schedule(&Leaving_model2, task_ptr, Clock + task.duration, task.tag);
		if (Q > 0){
			task = task_queue_m2.front();
			if (task.n <= N && task.v <= V){
				flag_newtask = true;
			}
		}
	} while (flag_newtask);
	print_resources(resource_m2, Clock, N, V);
}

void Leaving_model2(void *data_ptr)
{
	task_data *taskptr = static_cast<task_data*>(data_ptr);
	double Clock = cal.GetTime();

	V += taskptr->v;
	print_resources(resource_m2, Clock, N, V);
	if (Q > 0){
		task_data next = task_queue_m2.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept_model2, nullptr, cal.GetTime(), next.tag);
	}
	taskptr->leaving_time = cal.GetTime();
	// Вывод данных о времени обслуживания
	time_m2 << cal.GetTime() - taskptr->arrive_time << "\n";
	cal.Schedule(&Release_model2, data_ptr, Clock + d, taskptr->tag);
}

void Release_model2(void *data_ptr)
{
	task_data *taskptr = static_cast<task_data*>(data_ptr);
	double Clock = cal.GetTime();

	N += taskptr->n;
	print_resources(resource_m2, Clock, N, V);
	if (Q > 0){
		task_data next = task_queue_m2.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept_model2, nullptr, Clock, next.tag);
	}
	delete taskptr;
}

void Promotion_model2(void *strptr)
{
	std::string *tagptr = static_cast<std::string*>(strptr);
	std::string tag = *tagptr;
	std::list<task_data>::iterator it;
	for (it = task_queue_m2.begin(); it != task_queue_m2.end(); ++it){
		if (it->tag == tag) {
			task_data task = *it;
			task_queue_m2.erase(it);
			task_queue_m2.push_front(task);
			if (task.n <= N && task.v <= V){
				cal.Schedule(&Accept_model2, nullptr, cal.GetTime(), task.tag);
			}
			break;
		}
	}
	delete tagptr;
}

int main(int argc, char *argv[])
{
	const std::string sfx_res_m1 = "_res_m1";
	const std::string sfx_res_m2 = "_res_m2";
	const std::string sfx_time_m1 = "_time_m1";
	const std::string sfx_time_m2 = "_time_m2";
	srand(time(NULL));
	
	if (argc < 2) {
		std::cerr << "Uncorrect command line arguments";
		return -1;
	}

	filein.open(argv[1]);
	resource_m1.open(argv[1] + sfx_res_m1);
	resource_m2.open(argv[1] + sfx_res_m2);
	time_m1.open(argv[1] + sfx_time_m1);
	time_m2.open(argv[1] + sfx_time_m2);

	Clear_Simulation();
	cal.Schedule(&Arrive, nullptr, 0, std::to_string(task_id));
	try{
		cal.Simulate();
	}
	catch(...){
		std::cout << "\nEND:\n";
	}
	
	cal.Clear();
	filein.close();
	filein.open(argv[1]);
	Clear_Simulation();
	cal.Schedule(&Arrive_model2, nullptr, 0, std::to_string(task_id));
	try{
		cal.Simulate();
	}
	catch(...){
		std::cout << "\nEND:\n";
	}
	filein.close();
	
	return 0;
}
