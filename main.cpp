#include <iostream>
#include <queue>
#include "generators.hpp"
#include "scheduler.hpp"

void Arrive();
void Accept();
void Leaving();

struct task_data
{
	int n;
	int v;
	double time;
	std::string tag;
};

std::queue<task_data> task_queue;
std::list<task_data> release_queue; //?!
Scheduler cal;
int task_id = 0;
double lambda = 0.7;
double mu = 0.4;
int Q = 0;
int N = 100;
int V = 25;

void Arrive()
{
	std::cout << "Arrive: \n";
	//new task
	task_data task;
	task.n = Uniform(1, N / 2);
	task.v = Uniform(V / 3);
	task.time = Expon(mu);
	task.tag = std::to_string(task_id);
	task_id++; 
	std::cout << " task: " << task.tag << " n: " 
				<< task.n << " v: " << task.v
				<< " t: " << task.time << "\n";
	
	Q++;
	task_queue.push(task);
	if (Q > 0 && task.n <= N && task.v <= V){
		cal.Schedule(&Accept, cal.GetTime(), task.tag);
	}
	cal.Schedule(&Arrive, cal.GetTime() + Expon(lambda), std::to_string(task_id));
}

void Accept()
{
	task_data task = task_queue.front();
	std::cout << "Accept: " << task.tag << "\n";
	task_queue.pop();
	Q--;
	N = N - task.n;
	V = V - task.v;
	cal.Schedule(&Leaving, cal.GetTime() + task.time, task.tag);
}

void Leaving()
{
	//....
}


int main()
{
	srand(time(NULL));
	
	cal.Schedule(&Arrive, cal.GetTime() + Expon(lambda), std::to_string(task_id));
	try{
		cal.Simulate();
	}
	catch(...){
		std::cout << "\nEND:\n";
	}
	
	return 0;
}
