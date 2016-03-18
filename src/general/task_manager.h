#pragma once

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <boost/thread.hpp>
#include <memory>
#include <vector>

namespace thrive
{

class Task
{
    public:
        template<class F>
        Task();
        ~Task();
};

class TaskManager
{
    public:

        /**
        * @brief Singleton instance
        *
        */
        static TaskManager&
        instance();

        /**
        * @brief Destructor
        *
        */
        ~TaskManager();

        size_t getAvailableThreads();

    protected:

    private:
        TaskManager();
        void start();
        void worker();
        bool m_running;

        size_t m_availableThreads;
        std::vector<std::unique_ptr<boost::thread>> m_threads;

        struct Implementation;
        std::unique_ptr<Implementation> m_impl;
};
}

#endif // TASKMANAGER_H
