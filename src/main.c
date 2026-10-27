#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 8192

int start_image_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        return -1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        return -1;
    }

    printf("[PS4 Image Viewer] Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            continue;
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            if (strstr(buffer, "POST /upload") != NULL) {
                char *success_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
                                         "<html><body style='background:#111;color:#4CAF50;text-align:center;font-family:sans-serif;padding-top:50px;'>"
                                         "<h1>Image Received Successfully on PS4!</h1>"
                                         "<p>Check your TV screen.</p>"
                                         "<a href='/' style='color:#fff;'>Send another image</a>"
                                         "</body></html>";
                write(new_socket, success_response, strlen(success_response));
            } else {
                char *html_form = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
                                  "<html><body style='background:#1a1a1a;color:#fff;text-align:center;font-family:sans-serif;padding-top:40px;'>"
                                  "<h2>PS4 Image Sender</h2>"
                                  "<form action='/upload' method='POST' enctype='multipart/form-data'>"
                                  "<input type='file' name='image' accept='image/*' style='padding:10px;background:#333;color:#fff;border:1px solid #555;'/><br><br>"
                                  "<input type='submit' value='Send to PS4 TV' style='padding:12px 24px;background:#0070D1;color:#fff;border:none;font-size:16px;border-radius:5px;'/>"
                                  "</form></body></html>";
                write(new_socket, html_form, strlen(html_form));
            }
        }

        close(new_socket);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    printf("=== PS4 Image Viewer Running ===\n");
    start_image_server();
    return 0;
}
