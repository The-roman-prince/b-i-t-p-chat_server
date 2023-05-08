#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m" // đoạn này ko cần quan tâm :))
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8898); 

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect() failed");
        return 1;
    }
    
    char buf[256];
    fd_set fdread;
    FD_ZERO(&fdread);
    int select_1, recv_1;

    // client vừa gõ phím được vừa nhận tin nhắn dc
    while (1)
    {
	FD_ZERO(&fdread);
        FD_SET(STDIN_FILENO, &fdread);
        FD_SET(client, &fdread);

        select_1 = select(client+1, &fdread, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &fdread))
        {
            fgets(buf, sizeof(buf), stdin);
            send(client, buf, strlen(buf), 0);
        }

        if (FD_ISSET(client, &fdread))
        {
            recv_1 = recv(client, buf, sizeof(buf), 0);
            if (recv_1 <= 0)
                break;
            buf[recv_1] = 0;
            printf("%s%s%s\n", MAG, buf, RESET); // tin nhắn nhận được thì in màu xong reset MAG - RESET ở #define
        }
    }

    // Ket thuc, dong socket
    close(client);

    return 0;
}