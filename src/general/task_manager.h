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
        typedef std::shared_ptr<Task> TaskPtr;

        Task();
        virtual ~Task();
        virtual void run() = 0;

        bool isReady();

    private:
        friend class TaskManager;
};

class TaskManager
{
    public:
        typedef std::shared_ptr<Task> TaskPtr;

        /**
        * @brief Singleton instance
        *
        */
        static TaskManager&
        instance();

        void start();
        void stop();
        void addTask(TaskPtr);

        /**
        * @brief Destructor
        *
        */
        ~TaskManager();

        size_t getAvailableThreads();

    protected:

    private:
        TaskManager();
        void worker();
        bool m_running;
        bool tryGetTask(TaskPtr&);
        std::vector<TaskPtr> m_taskQueue;

        size_t m_availableThreads;
        std::vector<boost::thread*> m_threads;

        struct Implementation;
        std::unique_ptr<Implementation> m_impl;
};
}

#endif // TASKMANAGER_H
