#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <list>
#include <queue>
#include <iostream>

struct ScheduleNode
{
	double time;
	void (*event_handler) (void*);
	void *attr;
	std::string id;
	ScheduleNode *next;
};

class Scheduler
{
	private:
		std::list<ScheduleNode *> calendar;
		double SimTime;
		bool quit_flag;
	public:
		Scheduler ();
		~Scheduler();
		Scheduler(const Scheduler&) = delete;
		Scheduler& operator =(const Scheduler&) = delete;
		
		ScheduleNode* newScheduleNode(void (*event) (void*), void *attr,
									const double t, const std::string &event_id);
		
		void Schedule(void (*event) (void*), void *attr, const double t, 
						const std::string &event_id = "");
		
		void Cancel(const std::string &event_id, const double t);
		double GetTime() const;
		void Clear();
		void Retrieve();
		void Simulate();
		void ShowCalendar() const;
		void Stop();
};

#endif
				
		
