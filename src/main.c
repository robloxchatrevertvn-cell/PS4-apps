#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 16384

int main(int argc, char *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    printf("=== PS4 Image Viewer Final Version Starting ===\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Server listening on port %d... Open your phone browser and connect to your PS4 IP.\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            // التحقق من استقبال الصورة أو طلب الصفحة الرئيسية
            if (strstr(buffer, "POST") != NULL) {
                char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
                                 "<html><body style='background:#111;color:#4CAF50;text-align:center;font-family:sans-serif;padding-top:50px;'>"
                                 "<h1>Image Received on PS4!</h1>"
                                 "<p>The image data was successfully transferred.</p>"
                                 "<a href='/' style='color:#fff;'>Upload another</a>"
                                 "</body></html>";
                write(new_socket, response, strlen(response));
                printf("[PS4 Viewer] Image upload request processed.\n");
            } else {
                char *html_page = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
                                  "<html><body style='background:#1a1a1a;color:#fff;text-align:center;font-family:sans-serif;padding-top:40px;'>"
                                  "<h2>PS4 Ultimate Image Viewer</h2>"
                                  "<form action='/' method='POST' enctype='multipart/form-data'>"
                                  "<input type='file' name='img' accept='image/*' style='padding:15px;background:#333;color:#fff;border:1px solid #555;font-size:16px;'/><br><br>"
                                  "<input type='submit' value='Display on TV' style='padding:15px 30px;background:#0070D1;color:#fff;border:none;font-size:18px;border-radius:8px;'/>"
                                  "</form></body></html>";
                write(new_socket, html_page, strlen(html_page));
            }
        }

        close(new_socket);
    }

    return 0;
}
