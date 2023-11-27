#include "./include/csapp.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

struct task {
    int connfd;
};

std::queue<task> task_queue;
std::mutex queue_mutex;
std::condition_variable cond_var;
const int NUM_WORKERS = 13;

void worker_function() {
    while (true) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        cond_var.wait(lock, [] { return !task_queue.empty(); });

        task t = task_queue.front();
        task_queue.pop();
        lock.unlock();

        handle_client(t.connfd);
    }
}

int main() {
    std::thread workers[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers[i] = std::thread(worker_function);
    }

    int listenfd = Open_listenfd(DEFAULT_PORT);
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(struct sockaddr_in);

    while (true) {
        int connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        std::lock_guard<std::mutex> lock(queue_mutex);
        task_queue.push({connfd});
        cond_var.notify_one();
    }

    // Join threads (not reached in this example)
    for (auto& worker : workers) {
        worker.join();
    }

    return 0;
}
