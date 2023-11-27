# A Priority-Based Pre-Threaded Image Processing Server

## Project Overview

This project explores different approaches to building a client-server image processing system using C/C++, TCP sockets, POSIX processes, multithreading, and OpenCV. It demonstrates how image-processing requests can be handled using different server architectures, progressing from sequential processing to concurrent execution and priority-based thread scheduling.

## Tasks

### Task 1 – Basic Image Processing Using OpenCV

Load an input image using OpenCV, convert the original color image into a grayscale image, save the resulting image, and display both the original and grayscale images.

### Task 2 – Process-Based Client-Server Image Processing

Implement a TCP client-server application in which multiple client processes are created using `fork()`. Each client sends an image and an image-processing operation request to the server. The server receives the image, performs the requested operation using OpenCV, and sends the processed image back to the client.

### Task 3 – Multithreaded Image Processing Server

Extend the client-server image-processing application by introducing multithreading on the server. A separate thread is created for each incoming client connection, allowing multiple client requests to be handled concurrently instead of processing each connection sequentially.

### Task 4 – Pre-Threaded Server Using a Worker Thread Pool

Implement a pre-threaded server architecture using a fixed pool of 13 worker threads. Incoming client connections are placed into a shared task queue. Worker threads wait for tasks using condition variables, retrieve connections from the queue, and handle the client requests concurrently.

### Task 5 – Priority-Based Pre-Threaded Server

Extend the worker-thread-pool architecture by incorporating thread scheduling priorities. The main server thread is assigned a higher `SCHED_FIFO` priority, while worker threads are assigned a lower priority. Client connections are placed in a shared queue and processed by the worker threads, demonstrating priority-based scheduling in a multithreaded server.

