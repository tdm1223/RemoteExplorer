#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    ThreadPool(size_t numOfThread_);
    ~ThreadPool();

    // job 을 추가한다.
    void EnqueueJob(std::function<void()> job);
private:
    // 총 Worker 쓰레드의 개수.
    size_t numOfThread_;
    // Worker 쓰레드를 보관하는 벡터.
    std::vector<std::thread> workerThreads_;
    // 할일들을 보관하는 job 큐.
    std::queue<std::function<void()>> jobQueue_;
    // 위의 job 큐를 위한 cv 와 m.
    std::condition_variable conditionVariable_;
    std::mutex mutex_;

    // 모든 쓰레드 종료
    bool isStopAllThread;

    // Worker 쓰레드
    void WorkerThread();
};