#include "ThreadPool.h"
#include<iostream>

ThreadPool::ThreadPool(size_t num_threads) : numOfThread_(num_threads), isStopAllThread(false)
{
    workerThreads_.reserve(numOfThread_);
    for (size_t i = 0; i < numOfThread_; ++i)
    {
        workerThreads_.emplace_back([this]() { this->WorkerThread(); });
    }
}

void ThreadPool::WorkerThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        conditionVariable_.wait(lock, [this]() { return !this->jobQueue_.empty() || isStopAllThread; });
        if (isStopAllThread && this->jobQueue_.empty())
        {
            return;
        }

        // 맨 앞의 job 을 뺀다.
        std::function<void()> job = std::move(jobQueue_.front());
        jobQueue_.pop();
        lock.unlock();

        // 해당 job 을 수행한다 :)
        job();
    }
}

ThreadPool::~ThreadPool()
{
    isStopAllThread = true;
    conditionVariable_.notify_all();

    for (auto& t : workerThreads_) 
    {
        t.join();
    }
}

void ThreadPool::EnqueueJob(std::function<void()> job) 
{
    if (isStopAllThread) 
    {
        throw std::runtime_error("ThreadPool 사용 중지됨");
    }
    std::lock_guard<std::mutex> lock(mutex_);
    jobQueue_.push(std::move(job));
    conditionVariable_.notify_one();
}