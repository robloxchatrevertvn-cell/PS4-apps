#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

// دالة بسيطة لبدء خادم HTTP على PS4 لاستقبال الصور
int start_image_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // إنشاء الـ Socket
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

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("[PS4 Image Viewer] Server started on port %d...\n", PORT);

    // حلقة استقبال الاتصالات من هاتف Android
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // قراءة الطلب القادم من المتصفح أو تطبيق الهاتف
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            // الرد برصف HTML بسيط يؤكد جاهزية الاستقبال أو استقبال البيانات
            char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
                             "<html><body style='background:#111;color:#fff;text-align:center;font-family:sans-serif;'>"
                             "<h2>PS4 Image Receiver Active</h2>"
                             "<p>Send your image to display on TV.</p>"
                             "</body></html>";
            write(new_socket, response, strlen(response));
        }

        close(new_socket);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    printf("=== PS4 Image Viewer Initializing ===\n");
    
    // تشغيل الخادم المحلي
    start_image_server();

    return 0;
}
