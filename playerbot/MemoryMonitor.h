#ifndef _MemoryMonitor_H
#define _MemoryMonitor_H
//#define MEMORY_MONITOR

#include "Common.h"
#include "PlayerbotAIBase.h"

using namespace std;

class MemoryMonitor
{
    public:
        MemoryMonitor();
        virtual ~MemoryMonitor();
        static MemoryMonitor& instance()
        {
            static MemoryMonitor instance;
            return instance;
        }

	public:        
        void Add(string objectType, uint64 object, uint32 level = 0);
        void Rem(string objectType, uint64 object, uint32 level = 0);
        void Print();
        void Browse();
	private:
        unordered_map<thread::id, map<string, int32>> objectnumbers;
        list<map<string, int32 >> objectnumbersHist;
        unordered_map < thread::id, unordered_map<string, unordered_map<uint64,pair<string,time_t>>>> adds, rems;
		std::mutex lock;
};


#define sMemoryMonitor MemoryMonitor::instance()

#endif
