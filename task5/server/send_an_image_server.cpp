#include "./include/csapp.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <pthread.h>

struct task {
    int connfd;
};

std::queue<task> task_queue;
std::mutex queue_mutex;
std::condition_variable cond_var;
const int NUM_WORKERS = 13;

void handle_client(int connfd) {
    int client_number = static_cast<int>(pthread_self());
    std::cout << "Server: Processing client " << client_number << "...\n";

    // Read file size from client
    int file_size;
    Rio_readn(connfd, &file_size, sizeof(file_size));
    if (file_size <= 0) {
        std::cerr << "Server: Received empty image or file size error from client " << client_number << ".\n";
        Close(connfd);
        return;
    }

    // Read image data
    std::vector<char> image_data(file_size);
    Rio_readn(connfd, image_data.data(), file_size);

    // Save received image
    std::string inputPath = "./server/received_images/received_image_" + std::to_string(client_number) + ".jpg";
    std::ofstream inFile(inputPath, std::ios::binary);
    inFile.write(image_data.data(), file_size);
    inFile.close();

    // Process the image
    std::string outputPath = "./server/processed_images/processed_image_" + std::to_string(client_number) + ".jpg";
    // process_image(inputPath, outputPath); // Implement this function based on your requirements

    // Send processed image back to client
    std::ifstream outFile(outputPath, std::ios::binary | std::ios::ate);
    int processed_file_size = outFile.tellg();
    outFile.seekg(0, std::ios::beg);

    std::vector<char> processed_image_data(processed_file_size);
    if (outFile.read(processed_image_data.data(), processed_file_size)) {
        Rio_writen(connfd, &processed_file_size, sizeof(processed_file_size));
        Rio_writen(connfd, processed_image_data.data(), processed_file_size);
    }
    outFile.close();

    Close(connfd);
    std::cout << "Server: Client " << client_number << " disconnected.\n";
}
void worker_function() {
    // Set worker thread priority (lower than main thread)
    pthread_t this_thread = pthread_self();
    struct sched_param params;
    params.sched_priority = sched_get_priority_min(SCHED_FIFO);
    pthread_setschedparam(this_thread, SCHED_FIFO, &params);

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
    std::vector<std::thread> workers;

    // Set main thread to a higher priority
    pthread_t main_thread = pthread_self();
    struct sched_param sch_params;
    sch_params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(main_thread, SCHED_FIFO, &sch_params);

    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers.emplace_back(worker_function);
    }

    int listenfd = Open_listenfd(DEFAULT_PORT);
    sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(sockaddr_in);

    while (true) {
        int connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        std::lock_guard<std::mutex> lock(queue_mutex);
        task_queue.push({connfd});
        cond_var.notify_one();
    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    return 0;
}
