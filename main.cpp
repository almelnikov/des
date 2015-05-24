#include <iostream>
#include <queue>
#include <map>
#include <limits>
#include <functional>
#include "generators.hpp"
#include "scheduler.hpp"

void Arrive(void*);
void Accept(void*);
void Leaving(void*);
void Release(void*);

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

int task_id = 0;
double lambda = 0.7;
double mu = 0.4;
double d = 0.5;
int Q = 0;
int N = 100;
int V = 25;

task_data new_task(double arrive_time)
{
	task_data task;
	task.n = Uniform(1, N / 2);
	task.v = Uniform(V / 3);
	task.duration = Expon(mu);
	task.arrive_time = arrive_time;
	task.leaving_time = 0.0;
	task.tag = std::to_string(task_id);
	return task;
}

void Arrive(void *empty_ptr)
{
	double Clock = cal.GetTime();
	double next = Expon(lambda);
	task_data task = new_task(Clock);
	std::cout << "Arrive: " << task.tag << " time: " 
				<< cal.GetTime() << "\n";
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
	
	std::cout << "leave: " << task.tag << " t:" 
		<< cal.GetTime() + task.duration + d << "\n";
	
	task_queue.pop();
	Q--;
	N -= task.n;
	V -= task.v;
	task_data *task_ptr = new task_data(task);
	cal.Schedule(&Leaving, task_ptr, Clock + task.duration, task.tag);
}

void Leaving(void *data_ptr)
{
	task_data *taskptr = static_cast<task_data*>(data_ptr);

	V += taskptr->v;
	if (Q > 0){
		task_data next = task_queue.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept, nullptr, cal.GetTime(), next.tag);
	}
	taskptr->leaving_time = cal.GetTime() + d;
	cal.Schedule(&Release, data_ptr, taskptr->leaving_time, taskptr->tag);
}

void Release(void *data_ptr)
{
	task_data *taskptr = static_cast<task_data*>(data_ptr);

	std::cout << "Leave: " << taskptr->tag << " time: " 
			<< cal.GetTime() << "\n";
	N += taskptr->n;
	if (Q > 0){
		task_data next = task_queue.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept, nullptr, cal.GetTime(), next.tag);
	}
}

int main()
{
	srand(time(NULL));
	
	cal.Schedule(&Arrive, nullptr, cal.GetTime() + Expon(lambda), std::to_string(task_id));
	try{
		cal.Simulate();
	}
	catch(...){
		std::cout << "\nEND:\n";
	}
		
	return 0;
}
