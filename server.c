#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void send_html
    (int client_socket, const char* filename) {
        FILE *html_file = fopen(filename, "r");
        if (html_file == NULL) {
            perror("Error opening HTML file");
            return; }
        
        char response_header[BUFFER_SIZE];
        snprintf(response_header, BUFFER_SIZE,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n\r\n");

        send(client_socket, response_header, strlen(response_header), 0);

        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, html_file)) > 0)
            send(client_socket, buffer, bytes_read, 0);

        fclose(html_file);
}

int main
    () {
        int server_socket, client_socket;
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_len = sizeof(client_addr);

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE); }
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
            perror("Bind failed");
            exit(EXIT_FAILURE); }

        if (listen(server_socket, 5) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE); }

        printf("Server running on port %d...\n", PORT);

        while (1) {
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
            if (client_socket < 0) {
                perror("Accept failed");
                continue; }
            
            send_html(client_socket, "index.html");
            close(client_socket);
        }

        close(server_socket);
        return 0;
}