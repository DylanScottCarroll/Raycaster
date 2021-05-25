#include "header.h"

void Timers::Start_Timer(int id){
	starts[id] =  high_resolution_clock::now();
}

void Timers::Pause_Timer(int id){
	auto duration = duration_cast<microseconds>( high_resolution_clock::now() - starts[id] );
	totalTimes[id] += duration.count();
}

void Timers::Print_Timer(int id){
	cout << id << ": " << totalTimes[id] << endl;
}

void Timers::Print_As_Percent(int id1, int id2){
	cout << id1 << ": " << 100*totalTimes[id1]/totalTimes[id2] << "%" << endl;
}