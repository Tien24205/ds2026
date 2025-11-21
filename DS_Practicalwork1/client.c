#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 4096
#define FILENAME_SIZE 256

void receive_file(int server_socket, const char *filename, const char *output_filename) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Receive initial response (file size or error)
    bytes_received = recv(server_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0) {
        printf("Server disconnected\n");
        return;
    }
    buffer[bytes_received] = '\0';

    // Check if file was found
    if (strcmp(buffer, "FILE_NOT_FOUND") == 0) {
        printf("Server: File not found!\n");
        return;
    }

    // Parse file size and receive file content
    long file_size = atol(buffer);
    printf("Receiving %s (%ld bytes)\n", filename, file_size);

    FILE *file = fopen(output_filename, "wb");
    if (file == NULL) {
        printf("Error creating output file\n");
        return;
    }

    long total_received = 0;
    while (total_received < file_size) {
        bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
        total_received += bytes_received;

        // Display progress
        float progress = (float)total_received / file_size * 100;
        printf("Progress: %.1f%%\r", progress);
        fflush(stdout);
    }

    fclose(file);
    printf("\nFile saved as: %s\n", output_filename);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filename> <output_filename>\n", argv[0]);
        return 1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to server\n");

    // Send filename to server
    send(sock, argv[1], strlen(argv[1]), 0);

    // Receive file from server
    receive_file(sock, argv[1], argv[2]);

    close(sock);
    return 0;
}