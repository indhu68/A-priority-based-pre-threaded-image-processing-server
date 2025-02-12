#include "csapp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

const int MAX_BUFFER_SIZE = 4096;
const int DEFAULT_PORT = 8080;
const std::string DEFAULT_SERVER_IP = "10.0.2.15";
const std::string DEFAULT_OPERATION = "greyscale";
const int NUM_OPERATIONS = 4;

void error(const std::string &msg) {
    perror(msg.c_str());
    exit(1);
}

void send_image(int sockfd, const std::string &filename) {
    std::ifstream fp(filename, std::ios::binary | std::ios::ate);
    if (!fp.is_open()) {
        std::cerr << "Error: File '" << filename << "' does not exist.\n";
        return;
    }

    int file_size = fp.tellg();
    if (file_size == 0) {
        std::cerr << "Error: File '" << filename << "' is empty.\n";
        fp.close();
        return;
    }

    fp.seekg(0, std::ios::beg);
    Rio_writen(sockfd, &file_size, sizeof(file_size));

    std::vector<char> buffer(MAX_BUFFER_SIZE);
    while (!fp.eof()) {
        fp.read(buffer.data(), buffer.size());
        int bytes_read = fp.gcount();
        Rio_writen(sockfd, buffer.data(), bytes_read);
    }
}

void receive_and_display_image(int sockfd, const std::string &outputImagePath) {
    int file_size;
    Rio_readn(sockfd, &file_size, sizeof(file_size));

    std::vector<char> image_data(file_size);
    Rio_readn(sockfd, image_data.data(), file_size);

    std::ofstream fp(outputImagePath, std::ios::binary);
    if (!fp.is_open()) {
        error("Error opening file to save image");
    }
    fp.write(image_data.data(), file_size);
}

void client_process(int client_num, const std::string &server_ip, const std::string &image_file) {
    std::string operations[NUM_OPERATIONS] = {"greyscale", "blur", "vflip", "hflip"};
    srand(time(NULL) + client_num);
    std::string operation = operations[rand() % NUM_OPERATIONS];

    std::cout << "Client " << client_num << ": Selected operation: " << operation << "\n";

    int sockfd = Open_clientfd(const_cast<char *>(server_ip.c_str()), DEFAULT_PORT);
    send_image(sockfd, image_file);

    Rio_writen(sockfd, operation.c_str(), operation.length());
    Rio_writen(sockfd, "\n", 1);

    std::string outputImagePath = "./output/output_image_client" + std::to_string(client_num) + ".jpg";
    receive_and_display_image(sockfd, outputImagePath);
    Close(sockfd);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <number of clients> <image file> <server IP>\n";
        exit(1);
    }

    int num_clients = std::atoi(argv[1]);
    std::string image_file = argv[2];
    std::string server_ip = (argc > 3) ? argv[3] : DEFAULT_SERVER_IP;

    for (int i = 0; i < num_clients; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            client_process(i, server_ip, image_file);
            exit(0);
        }
    }

    for (int i = 0; i < num_clients; ++i) {
        wait(NULL);
    }

    return 0;
}
