#include <iostream>
#include <queue>
#include <map>
#include <limits>
#include <functional>
#include "generators.hpp"
#include "scheduler.hpp"

void Arrive();
void Accept();
void Leaving();
void Release();

struct task_data
{
	int n;
	int v;
	double duration;
	double leaving_time;
	std::string tag;
};

class Release_Calendar
{
	std::list<task_data> release_list;
	
	public:
		Release_Calendar(){}
		void insert(const task_data &rhs)
		{
			if (release_list.empty()) {
				release_list.push_back(rhs);
			} else {
				for (auto iter = release_list.begin(); iter != release_list.end(); iter++){
					if (rhs.leaving_time <= iter->leaving_time){
						release_list.insert(iter, rhs);
						return;
					}
				}
				release_list.push_back(rhs);
			}
		}
	
		task_data pop()
		{
			if (!release_list.empty()){
				task_data task = release_list.front();
				release_list.pop_front();
				return task;
			} else {
				throw "end";
			}
		}
	
		void Show()
		{
			for (auto iter : release_list)
			std::cout << iter.leaving_time << " " << iter.tag  <<"\n";
		}
		
};
		
Scheduler cal;
std::queue<task_data> task_queue;
Release_Calendar release_queue;

int task_id = 0;
double lambda = 0.7;
double mu = 0.4;
double d = 0.5;
int Q = 0;
int N = 100;
int V = 25;

task_data new_task()
{
	task_data task;
	task.n = Uniform(1, N / 2);
	task.v = Uniform(V / 3);
	task.duration = Expon(mu);
	task.leaving_time = 0.0;
	task.tag = std::to_string(task_id);
	return task;
}

void Arrive()
{
	task_data task = new_task();
	std::cout << "Arrive: " << task.tag << " time: " 
				<< cal.GetTime() << "\n";
	task_id++;
	double Clock = cal.GetTime();
	double next = Expon(lambda);

	Q++;
	task_queue.push(task);
	if (Q > 0 && task.n <= N && task.v <= V){
		cal.Schedule(&Accept, Clock, task.tag);
	}
	cal.Schedule(&Arrive, Clock + next, std::to_string(task_id));
}	

void Accept()
{
	task_data task = task_queue.front();
	double Clock = cal.GetTime();
	
	std::cout << "leave: " << task.tag << " t:" 
		<< cal.GetTime() + task.duration + d << "\n";
	
	task_queue.pop();
	Q--;
	N -= task.n;
	V -= task.v;
	cal.Schedule(&Leaving, Clock + task.duration, task.tag);
	
	//release
	task_data release = task;
	release.leaving_time = Clock + task.duration;
	release_queue.insert(release);
}

void Leaving()
{
	task_data task = release_queue.pop();
	V += task.v;
	if (Q > 0){
		task_data next = task_queue.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept, cal.GetTime(), next.tag);
	}
	task.leaving_time = cal.GetTime() + d;
	cal.Schedule(&Release, task.leaving_time, task.tag);
	release_queue.insert(task);
}

void Release()
{
	task_data task = release_queue.pop();
	std::cout << "Leave: " << task.tag << " time: " 
			<< cal.GetTime() << "\n";
	N += task.n;
	if (Q > 0){
		task_data next = task_queue.front();
		if (next.n <= N && next.v <= V)
			cal.Schedule(&Accept, cal.GetTime(), next.tag);
	}
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
