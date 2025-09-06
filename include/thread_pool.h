#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <stdexcept>  // ✅ Add this at the top


/*
 * ThreadPool class for managing a fixed pool of worker threads.
 * Tasks can be enqueued and processed concurrently.
 * Automatically joins all threads on destruction.
 */

class ThreadPool {
public:
    // Constructor: initializes and starts 'threads' worker threads
    ThreadPool(size_t threads);

    // Destructor: shuts down the pool and joins threads
    ~ThreadPool();

    // Enqueue a new task (lambda or function) to be executed
    void enqueue(std::function<void()> task);

    // Wait for all enqueued tasks to complete
    void wait_all();

private:
    std::vector<std::thread> workers;                  // Worker threads
    std::queue<std::function<void()>> tasks;           // Task queue

    std::mutex queue_mutex;                            // Mutex for queue access
    std::condition_variable condition;                 // Condition to notify workers
    std::atomic<bool> stop;                            // Stop flag

    std::atomic<int> active_threads;                   // Tracks active running tasks
};
