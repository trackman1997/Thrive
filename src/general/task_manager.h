#pragma once

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <memory>

namespace thrive
{

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

        unsigned short getAvailableThreads();

    protected:

    private:
        TaskManager();

        struct Implementation;
        std::unique_ptr<Implementation> m_impl;
};
}

#endif // TASKMANAGER_H
