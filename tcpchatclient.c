#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int port = 8080;
char name[20];

int main(int argc, char *argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    printf("\033[2J\033[H");
    printf("Enter the port to connect to: ");
    char port_input[10];
    scanf("%9s", port_input);

    printf("Enter your name: ");
    scanf("%19s", name);

    if (strlen(port_input) > 0)
    {
        port = atoi(port_input);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock, name, strlen(name), 0);
    printf("\033[2J\033[H");

    printf("> ");
    fflush(stdout);
    int connected = 1;

    while (connected)
    {
        memset(buffer, 0, BUFFER_SIZE);
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        int max_fd = sock > STDIN_FILENO ? sock : STDIN_FILENO;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("select error");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            fgets(message, BUFFER_SIZE, stdin);
            message[strcspn(message, "\n")] = 0;

            if (strcmp(message, "exit") == 0)
            {
                connected = 0;
                printf("\033[2J\033[H");
            }

            send(sock, message, strlen(message), 0);
            printf("> ");
            fflush(stdout);
        }

        if (FD_ISSET(sock, &readfds))
        {
            int valread = read(sock, buffer, BUFFER_SIZE - 1);
            if (valread > 0)
            {
                buffer[valread] = '\0';
                printf("\r\033[K");

                char buffer_copy[BUFFER_SIZE];
                memset(buffer_copy, 0, BUFFER_SIZE);
                strncpy(buffer_copy, buffer, BUFFER_SIZE - 1);
                buffer_copy[BUFFER_SIZE - 1] = '\0';

                char *user = strtok(buffer_copy, ":");
                char *msg = strtok(NULL, "");

                if (user && msg)
                {
                    if (strcmp(user, name) != 0)
                    {
                        printf("%s: %s\n", user, msg);
                    }
                }
                else
                {
                    printf("Invalid message format received: %s\n", buffer);
                }

                memset(buffer, 0, BUFFER_SIZE);

                printf("> ");
                fflush(stdout);
            }
            else if (valread == 0)
            {
                printf("\nServer disconnected\n");
                break;
            }
        }
    }

    close(sock);
    return 0;
}