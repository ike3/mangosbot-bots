#ifndef _MemoryMonitor_H
#define _MemoryMonitor_H
//#define MEMORY_MONITOR

#include <string>
#include <unordered_map>
#include <map>
#include <thread>


//#include "Common.h"
//#include "PlayerbotAIBase.h"

//using namespace std;

class MemoryMonitor
{
    public:
        MemoryMonitor() {};
        virtual ~MemoryMonitor() {};
        static MemoryMonitor& instance()
        {
            static MemoryMonitor instance;
            return instance;
        }

	public:        
        void Add(std::string objectType, uint64 object, int level = 0);
        void Rem(std::string objectType, uint64 object, int level = 0);
        void Print();
        void Browse();
	private:
        std::unordered_map<std::thread::id, std::map<std::string, int>> objectnumbers;
        std::list<std::map<std::string, int >> objectnumbersHist;
        std::unordered_map < std::thread::id, std::unordered_map<std::string, std::unordered_map<uint64,std::pair<std::string,time_t>>>> adds, rems;
};


#define sMemoryMonitor MemoryMonitor::instance()

#endif
