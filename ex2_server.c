#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/select.h>

static int port;

int main(int argc, char const *argv[]) // chạy server: ./server 9000 hoặc ./server ...
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &port);
    addr.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    int client[5]; // chi chap nhan 5 thang client
    int validate[5];memset(validate, 0, sizeof(validate)); // kiem tra client da nhập dung chua đúng thì = 1 : validate {0, 0, 0, 0, 0}
    char *id[5]; // mang xau ki tu de luu client_id
    char message[1024];

    int index = 0;
    char buf[256];
    int receive_1, scan_1, accept_1;
    fd_set fdread;
    char check1[100], check2[100], check3[100];

    while (1)
    {
        FD_ZERO(&fdread); // reset fread
        FD_SET(listener, &fdread); // thêm sự kiện xảy ra khi nhận kết nối vào fread
        for (int i=0;i<index;i++) // mỗi thằng client mới thì thêm sự kiện của nó vào fread
        {
            FD_SET(client[i], &fdread);
        }

        int select_1 = select(100, &fdread, NULL, NULL, NULL); // kiểm tra liên tục fread

        if (FD_ISSET(listener , &fdread)) // nếu nhận dc tín hiệu kết nối từ client thì ...
        {
            int accept_1 = accept(listener, NULL, NULL);
            client[index++] = accept_1; // lưu lại để thêm sự kiện vào fread
            send(accept_1, "client_id : client_name\n", 24, 0);
        }

        for (int i=0;i<index;i++)
        {
            if (FD_ISSET(client[i], &fdread)) // với mỗi client - check sự kiện, nếu có thì ...
            {
                receive_1 = recv(client[i], buf, sizeof(buf), 0);
                buf[receive_1] = 0;
                if (validate[i] == 0) // nếu chưa gửi đúng client_id : client_name
                {
                    if (sscanf(buf, "%s : %s %s", check1, check2, check3) != 2) // kiểm tra xem nhập có thừa (=3) hoặc thiếu (<2) không 
                    {
                        send(client[i], "client_id : client_name\n", 24, 0);
                    }
                    else
                    {
                        validate[i] = 1; // gửi đúng thì lưu lại
                        id[i] = malloc(strlen(check1)); // trong c éo có mảng xâu ký tự đành phải như này
                        strcpy(id[i], check1);
                        printf("new id: %s\n", id[i]);
                    }
                }
                else // nếu client nhập đúng rồi thì gửi cho các thằng còn lại cái nó gửi
                {
                    for (int j=0;j<index;j++)
                    {
                        sprintf(message, "%s : %s", id[i], buf);
                        send(client[j], message, strlen(message), 0);
                    }
                }
            }
        }
    }

    close(listener);
    for (int i=0;i<sizeof(client);i++) close(client[i]);

    return 0;
}
