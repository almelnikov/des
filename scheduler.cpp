#include "scheduler.hpp"
#include <thread>
#include <chrono>

Scheduler::Scheduler() : SimTime(0){}
Scheduler::~Scheduler() 
{
	if (!calendar.empty())
		Clear();
}

ScheduleNode* Scheduler::newScheduleNode(void (*event) (void), const double t,
							const std::string &event_id) 
{
	auto *new_event = new ScheduleNode;
	new_event->time = t;
	new_event->event_handler = event;
	new_event->id = event_id;
	new_event->next = nullptr;
	return new_event;
}

void Scheduler::Schedule(void (*event) (void), const double t, 
						const std::string &event_id)
{
	auto *new_event = newScheduleNode(event, t, event_id);
	if (calendar.empty()){
		calendar.push_back(new_event);	
	} else {
		for (auto iter = calendar.begin(); iter != calendar.end(); iter++){
			if (t <= (*iter)->time){
				calendar.insert(iter, new_event);
				return;
			}
		}
		calendar.push_back(new_event);
	}			
}

void Scheduler::Cancel(const std::string &event_id, const double t)
{
	for (auto iter = calendar.begin(); iter != calendar.end();){
		if ((*iter)->time >= t && (*iter)->id == event_id){
			delete *iter;
			iter = calendar.erase(iter);
		} else {
			iter++;
		}
	}
}

double Scheduler::GetTime() const 
{
	return SimTime;
}

void Scheduler::Clear()
{
	for (auto iter = calendar.begin(); iter != calendar.end();){
		delete *iter;
		iter = calendar.erase(iter);
	}
}

void Scheduler::Retrieve()
{
	auto iter = calendar.begin();
	delete *iter;
	calendar.pop_front();
}

void Scheduler::Simulate()
{
	ScheduleNode *head;
	while(true){ 
	//	ShowCalendar();
		head = calendar.front();
		Retrieve();
		SimTime = head->time;
		std::cout << "Simulation time: " << SimTime << "\n";
		head->event_handler();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}	

void Scheduler::ShowCalendar() const
{
	for (auto iter : calendar)
		std::cout << iter->time << " " << iter->id  <<"\n";
}
