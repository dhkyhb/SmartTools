#ifndef SMARTTOOLS_SMART_UTILS_THREAD_LOOP_HPP
#define SMARTTOOLS_SMART_UTILS_THREAD_LOOP_HPP

#include "../kernel.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace smart::utils::thread::loop
{

using kernel::ItType;

using ThreadLoopTaskThing = void (*)(Jint id, void *);

typedef struct
{
    Jbool isCreate;
    Jbool isUing;
    Jint taskID;
    void *taskPamrameter;
    ThreadLoopTaskThing taskThing;

    std::mutex taskLock;
    std::thread taskThread;
    std::condition_variable taskCondition;
} ThreadLoopTask;

typedef struct
{
    Jint current;
    Jint count;
    ThreadLoopTask *tasks;

    std::mutex taskInsertLock;
} ThreadLoopTasks;

typedef struct
{
    Jbool isNew;
    void *p;
    Jint id;
    ThreadLoopTaskThing thing;
} ThreadLoopParameter;

constexpr Jint THREAD_LOOP_IF_NOT_SET_MAX_BUF_USE_THIS_MAX_BUF_SIZE = 50;

class ThreadLoop
{
public:
    static ThreadLoop &Instance()
    {
        static ThreadLoop *obj = nullptr;
        if (obj == nullptr)
            obj = new ThreadLoop();
        return (*obj);
    }

    ThreadLoop &Init(Jsize loopSize)
    {
        if (this->mThreadLoopTasks.tasks != nullptr)
            return (*this);

        this->mThreadLoopTasks.count = loopSize;
        this->mThreadLoopTasks.tasks = new ThreadLoopTask[this->mThreadLoopTasks.count];
        return (*this);
    }

    template<typename T>
    ThreadLoop &Insert(void *p, ItType<T> taskThing)
    {
        if (this->mThreadLoopTasks.tasks == nullptr)
        {
            this->mThreadLoopTasks.count = THREAD_LOOP_IF_NOT_SET_MAX_BUF_USE_THIS_MAX_BUF_SIZE;
            this->mThreadLoopTasks.tasks = new ThreadLoopTask[this->mThreadLoopTasks.count];
        }

        this->mThreadLoopTasks.taskInsertLock.lock();
        this->mThreadLoopParameter.isNew = true;

        for (auto i = 0; i < this->mThreadLoopTasks.count; ++i)
        {
            if (!this->mThreadLoopTasks.tasks[i].isCreate)
                continue;
            if (this->mThreadLoopTasks.tasks[i].isUing)
                continue;

            this->mThreadLoopParameter.p = p;
            this->mThreadLoopParameter.id = i;
            this->mThreadLoopParameter.thing = taskThing;
            this->mThreadLoopParameter.isNew = false;
            this->AddTask();
            break;
        }

        if ((this->mThreadLoopParameter.isNew) && (this->mThreadLoopTasks.current < this->mThreadLoopTasks.count))
        {
            this->mThreadLoopParameter.p = p;
            this->mThreadLoopParameter.id = this->mThreadLoopTasks.current;
            this->mThreadLoopParameter.thing = taskThing;
            ++this->mThreadLoopTasks.current;
            this->AddNewTask();
        }
        return (*this);
    }

    void Process()
    {
        auto &i = this->mThreadLoopParameter.id;

        if (this->mThreadLoopParameter.isNew)
            this->mThreadLoopTasks.tasks[i].taskThread.detach();
        else
            this->mThreadLoopTasks.tasks[i].taskCondition.notify_one();

        this->mThreadLoopTasks.taskInsertLock.unlock();
    }

private:
    ThreadLoopTasks mThreadLoopTasks;
    ThreadLoopParameter mThreadLoopParameter;

    ThreadLoop() :
        mThreadLoopTasks{},
        mThreadLoopParameter{}
    {}

    void AddTask()
    {
        auto &i = this->mThreadLoopParameter.id;
        this->mThreadLoopTasks.tasks[i].taskThing = this->mThreadLoopParameter.thing;
        this->mThreadLoopTasks.tasks[i].taskPamrameter = this->mThreadLoopParameter.p;
        this->mThreadLoopTasks.tasks[i].isUing = true;
    }

    void AddNewTask()
    {
        auto &i = this->mThreadLoopParameter.id;
        this->mThreadLoopTasks.tasks[i].taskID = i;
        this->mThreadLoopTasks.tasks[i].taskThing = this->mThreadLoopParameter.thing;
        this->mThreadLoopTasks.tasks[i].taskPamrameter = this->mThreadLoopParameter.p;
        this->mThreadLoopTasks.tasks[i].isCreate = true;
        this->mThreadLoopTasks.tasks[i].isUing = true;

        this->mThreadLoopTasks.tasks[i].taskThread = std::thread(
            [](ThreadLoopTask *p) {
                auto &task = *p;

                while (true)
                {
                    std::unique_lock<std::mutex> uniqueLock(task.taskLock);
                    if (!task.isUing)
                        task.taskCondition.wait(uniqueLock);

                    task.taskThing(task.taskID, task.taskPamrameter);
                    task.isUing = false;
                }
            },
            &this->mThreadLoopTasks.tasks[i]
        );
    }
};

}

#endif //SMARTTOOLS_SMART_UTILS_THREAD_LOOP_HPP
