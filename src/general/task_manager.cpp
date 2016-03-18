#include "task_manager.h"

using namespace thrive;

struct TaskManager::Implementation{
    size_t m_availableThreads = boost::thread::hardware_concurrency();
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

size_t
TaskManager::getAvailableThreads()
{
    return m_availableThreads;
}

void
TaskManager::start()
{
    m_running = true;

    for(size_t i = 0; i < m_availableThreads; ++i)
        m_threads.push_back(std::unique_ptr<boost::thread>(new boost::thread(boost::bind(&TaskManager::worker,this))));

    while (m_running)
        boost::this_thread::yield();
}

void
TaskManager::worker()
{
    while(m_running)
    {

    }
}
