#include "task_manager.h"

#include <boost/thread.hpp>

using namespace thrive;

struct TaskManager::Implementation{
    unsigned short m_availableThreads = boost::thread::hardware_concurrency()-1;
};

TaskManager&
TaskManager::instance() {
    static TaskManager instance;
    return instance;
}

TaskManager::TaskManager()
    : m_impl(new Implementation())
{
}

TaskManager::~TaskManager() { }

unsigned short
TaskManager::getAvailableThreads()
{
    return m_impl->m_availableThreads;
}
