#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define PORT 65432
#define BUFFER_SIZE 4096
#define FILENAME_SIZE 256

void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        send(client_socket, "FILE_NOT_FOUND", 14, 0);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Send file size
    char size_buffer[32];
    snprintf(size_buffer, sizeof(size_buffer), "%ld", file_size);
    send(client_socket, size_buffer, strlen(size_buffer), 0);

    // Send file content
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    printf("File sent successfully: %s (%ld bytes)\n", filename, file_size);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, 
                                   (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        // Receive filename from client
        char filename[FILENAME_SIZE];
        int bytes_received = recv(client_socket, filename, FILENAME_SIZE - 1, 0);
        if (bytes_received > 0) {
            filename[bytes_received] = '\0';
            printf("Client requested: %s\n", filename);
            send_file(client_socket, filename);
        }

        close(client_socket);
        printf("Client disconnected\n\n");
    }

    close(server_fd);
    return 0;
}