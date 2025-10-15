#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int port = 8080;
char client_names[MAX_CLIENTS][20];

int main(int argc, char *argv[])
{
    int server_fd, new_socket, client_socket[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int max_sd, sd, activity, i, j;
    socklen_t addrlen;

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        client_socket[i] = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)))
    {
        perror("Setsockopt");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(client_addr);
    printf("TCP Chat Server listening on port %d\n", port);

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_fd, &readfds))
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            char username[21];
            memset(username, 0, sizeof(username));
            int bytes_read = read(new_socket, username, sizeof(username) - 1);
            if (bytes_read > 0)
            {
                username[bytes_read] = '\0';
                username[strcspn(username, "\n")] = 0;
            }

            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    memset(client_names[i], 0, sizeof(client_names[i]));
                    strncpy(client_names[i], username, sizeof(client_names[i]) - 1);
                    client_names[i][sizeof(client_names[i]) - 1] = '\0';
                    printf("New connection: socket fd is %d, ip is : %s, port : %d, username: %s\n",
                           new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_names[i]);
                    break;
                }
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds))
            {
                int valread;
                getpeername(sd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    printf("Host disconnected: ip %s, port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(sd);
                    client_socket[i] = 0;
                    memset(client_names[i], 0, sizeof(client_names[i]));
                }
                else
                {
                    buffer[valread] = '\0';
                    printf("%s, on port %d sent: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

                    char formatted_message[BUFFER_SIZE + 50];
                    snprintf(formatted_message, sizeof(formatted_message), "%s: %s", client_names[i], buffer);
                    send(sd, formatted_message, strlen(formatted_message), 0);

                    for (j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (client_socket[j] != 0 && client_socket[j] != sd)
                        {
                            char formatted_message[BUFFER_SIZE];
                            snprintf(formatted_message, sizeof(formatted_message), "%s: %.1000s", client_names[i], buffer);
                            send(client_socket[j], formatted_message, strlen(formatted_message), 0);
                        }
                    }

                    memset(buffer, 0, BUFFER_SIZE);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}