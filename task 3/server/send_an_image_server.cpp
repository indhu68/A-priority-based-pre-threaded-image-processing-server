#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "./include/csapp.h"
int generate_client_number() {
    static int client_count = 0;
    return client_count++;
}
void handle_client(int connfd) {
    int client_number = generate_client_number();
    std::cout << "Server: Processing client " << client_number << "...\n";

    int file_size;
    Rio_readn(connfd, &file_size, sizeof(file_size));
    if (file_size <= 0) {
        std::cerr << "Server: Received empty image or file size error from client " << client_number << ".\n";
        Close(connfd);
        return;
    }

    std::vector<char> image_data(file_size);
    Rio_readn(connfd, image_data.data(), file_size);

    std::string inputPath = "./server/received_images/received_image_" + std::to_string(client_number) + ".jpg";
    std::string outputPath = "./server/processed_images/processed_image_" + std::to_string(client_number) + ".jpg";

    std::ofstream inFile(inputPath, std::ios::binary);
    inFile.write(image_data.data(), file_size);
    inFile.close();

    char operation[256];
    Rio_readlineb(connfd, operation, sizeof(operation));

    std::string cmd = "./opencv/convert " + inputPath + " " + outputPath + " " + operation;
    system(cmd.c_str());

    std::ifstream outFile(outputPath, std::ios::binary | std::ios::ate);
    int gray_file_size = outFile.tellg();
    outFile.seekg(0, std::ios::beg);

    std::vector<char> gray_image_data(gray_file_size);
    if (outFile.read(gray_image_data.data(), gray_file_size)) {
        Rio_writen(connfd, &gray_file_size, sizeof(gray_file_size));
        Rio_writen(connfd, gray_image_data.data(), gray_file_size);
    }
    outFile.close();

    Close(connfd);
    std::cout << "Server: Client " << client_number << " disconnected.\n";
}
int main() {
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    listenfd = Open_listenfd(DEFAULT_PORT);
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = new int;
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        // Create a new thread for each client
        std::thread t(handle_client, *connfdp);
        t.detach();
    }
}