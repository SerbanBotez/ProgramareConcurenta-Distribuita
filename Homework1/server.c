#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <error.h>
#include <unistd.h>
#include <string.h>

extern int errno;

void tcp_protocol()
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in from;

    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("%s\n", "Eroare la socket");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int enable = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0 )
    {
        printf("%s\n", "Eroare la set socket");
        return;
    }

    if(bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("%s\n", "Eroare la bind");
        return;
    }

    if(listen(serverSocket, 5) == -1)
    {
        printf("%s\n", "Eroare la listen");
        return;
    }

    while(1)
    {
        int clientSocket;
        socklen_t length = sizeof(from);
        int bytes_read = 0;

        if((clientSocket = accept(serverSocket, (struct sockaddr *) &from, &length)) < 0)
        {
            printf("%s", "Eroare la accept");
            continue;
        }

        printf("%s\n", "A fost conectat un client");

        int message_size;
        char message[65335];
        int messages_read_number = 0;
        long bytes_read_number = 0;

        while(1)
        {
            if(read(clientSocket, &message_size, sizeof(int)) < 0)
            {
                perror("Eroare la read1");
                //continue;
            }

            if(message_size == 0)
                break;

            if(message_size < 65335)
            {
                //printf("%d\n", message_size);

                if((bytes_read = read(clientSocket, message, message_size)) < 0)
                {
                    perror("Eroare la read2");
                }

                printf("%d\n", bytes_read);

                messages_read_number = messages_read_number + 1;
                bytes_read_number = bytes_read_number + bytes_read;
            
                break;
            }

            //printf("%d\n", message_size);

            if((bytes_read = read(clientSocket, message, message_size)) < 0)
            {
                perror("Eroare la read2");
                continue;
            }
            
            printf("%d\n", bytes_read);

            messages_read_number = messages_read_number + 1;
            bytes_read_number = bytes_read_number + bytes_read;

            //printf("%s\n", message);
        }
        printf("Messages read : %d\n", messages_read_number);
        printf("Bytes read : %ld\n", bytes_read_number);
    }   
}

void tcpsw_protocol()
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in from;

    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("%s\n", "Eroare la socket");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int enable = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0 )
    {
        printf("%s\n", "Eroare la set socket");
        return;
    }

    if(bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("%s\n", "Eroare la bind");
        return;
    }

    if(listen(serverSocket, 5) == -1)
    {
        printf("%s\n", "Eroare la listen");
        return;
    }

    while(1)
    {
        int clientSocket;
        socklen_t length = sizeof(from);
        int bytes_read = 0;

        if((clientSocket = accept(serverSocket, (struct sockaddr *) &from, &length)) < 0)
        {
            printf("%s", "Eroare la accept");
            continue;
        }

        printf("%s\n", "A fost conectat un client");

        int message_size;
        char message[65335];
        int messages_read_number = 0;
        long bytes_read_number = 0;
        char confirm_message[3];

        while(1)
        {
            if(read(clientSocket, &message_size, sizeof(int)) < 0)
            {
                perror("Eroare la read1");
                //continue;
            }

            if(message_size == 0)
                break;

            if(message_size < 65335)
            {
                if((bytes_read = read(clientSocket, message, message_size)) < 0)
                {
                    perror("Eroare la read2");
                }

                printf("Bytes read : %d\n", bytes_read);

                messages_read_number = messages_read_number + 1;
                bytes_read_number = bytes_read_number + bytes_read;
            
                break;
            }

            if((bytes_read = read(clientSocket, message, message_size)) < 0)
            {
                perror("Eroare la read2");
                continue;
            }
            
            printf("Bytes read : %d\n", bytes_read);

            //printf("%d, %d\n", message_size, bytes_read);

            if(bytes_read != message_size)
            {
                strcpy(confirm_message , "NU\0");
                if(write(clientSocket, confirm_message, 3) < 0)
                {
                    perror("Eroare la scrierea confirm_message");
                }
                
                while(1)
                {
                    //printf("%s\n", "aici");
                    read(clientSocket, &message_size, sizeof(int));
                    bytes_read = read(clientSocket, message, message_size);

                    if(bytes_read != message_size)
                    {
                        strcpy(confirm_message , "NU\0");
                        write(clientSocket, confirm_message, 3);
                        continue;
                    }
                    //messages_read_number = messages_read_number + 1;
                    //bytes_read_number = bytes_read_number + bytes_read;
                    break;
                }
                //continue;
            }

            strcpy(confirm_message , "DA\0");
            write(clientSocket, confirm_message, 3);

            messages_read_number = messages_read_number + 1;
            bytes_read_number = bytes_read_number + bytes_read;

            //printf("%s\n", message);
        }
        printf("Messages read : %d\n", messages_read_number);
        printf("Bytes read : %ld\n", bytes_read_number);
    }   
}

void udp_protocol()
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int serverSocket;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      //return errno;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind (serverSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      //return errno;
    }

    while(1)
    {
        socklen_t length = sizeof(clientAddr);
        char message[65335];
        int message_size = 0;
        int nr_messages_received = 0;
        long nr_bytes_received = 0;
        int nr_messages_diff_sizes = 0;

        while(1)
        {

            if (recvfrom(serverSocket, &message_size, sizeof(int), 0,(struct sockaddr*) &clientAddr, &length) <= 0)
            {
                perror ("[server]Eroare la recvfrom() de la client.\n");
                //return errno;
            }    

            printf("%d\n", message_size);

            if (recvfrom(serverSocket, message, message_size, 0,(struct sockaddr*) &clientAddr, &length) <= 0)
            {
                perror ("[server]Eroare la recvfrom() de la client.\n");
                //return errno;
            }

            nr_messages_received ++;
            nr_bytes_received += message_size;

            if(message_size != 65335)
                nr_messages_diff_sizes++;

            if(message_size < 65335 || message_size == 0)
                break;

        }

        printf("Mesages received : %d\n", nr_messages_received);
        printf("Bytes reveived : %ld\n", nr_bytes_received);
        printf("Messages of different size : %d\n", nr_messages_diff_sizes);
    }

}

void udpsw_protocol()
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int serverSocket;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      //return errno;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind (serverSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      //return errno;
    }

    while(1)
    {
        socklen_t length = sizeof(clientAddr);
        char message[65335];
        int message_size = 0;
        int nr_messages_received = 0;
        long nr_bytes_received = 0;
        int nr_messages_diff_sizes = 0;
        int bytes_received = 0;
        char confirm_message[3];

        while(1)
        {

            if (recvfrom(serverSocket, &message_size, sizeof(int), 0,(struct sockaddr*) &clientAddr, &length) <= 0)
            {
                perror ("[server]Eroare la recvfrom() de la client.\n");
            }    

            if ((bytes_received = recvfrom(serverSocket, message, message_size, 0,(struct sockaddr*) &clientAddr, &length)) <= 0)
            {
                perror ("[server]Eroare la recvfrom() de la client.\n");
            }

            printf("%d\n", bytes_received);

            if(message_size > 65335 || message_size < 0)
                nr_messages_diff_sizes++;

            if(message_size < 65335 || message_size == 0)
            {
                strcpy(confirm_message, "DA\0");
                sendto(serverSocket, confirm_message, 3, 0, (struct sockaddr*)&clientAddr, length);
            
                nr_messages_received ++;
                nr_bytes_received += bytes_received;
                break;
            }

            if(bytes_received != message_size)
            {
                strcpy(confirm_message, "NU\0");
                sendto(serverSocket, confirm_message, 3, 0, (struct sockaddr*)&clientAddr, length);
                while(1)
                {
                    recvfrom(serverSocket, &message_size, sizeof(int), 0,(struct sockaddr*) &clientAddr, &length);
                    bytes_received = recvfrom(serverSocket, message, message_size, 0,(struct sockaddr*) &clientAddr, &length);

                    if(bytes_received == message_size)
                    {
                        strcpy(confirm_message, "DA\0");
                        break;
                    }

                    strcpy(confirm_message, "NU\0");
                    sendto(serverSocket, confirm_message, 3, 0, (struct sockaddr*)&clientAddr, length);

                }

            }

            strcpy(confirm_message, "DA\0");
            sendto(serverSocket, confirm_message, 3, 0, (struct sockaddr*)&clientAddr, length);

            nr_messages_received ++;
            nr_bytes_received += bytes_received;
        }

        printf("Mesages received : %d\n", nr_messages_received);
        printf("Bytes reveived : %ld\n", nr_bytes_received);
        printf("Messages of different size : %d\n", nr_messages_diff_sizes);
    }

}

int main(int argc, char *argv[])
{
char protocol_type[5];

    scanf("%s", protocol_type);
    
    if(strcmp(protocol_type, "tcp") == 0)
    {   
        tcp_protocol();
    }

    if(strcmp(protocol_type, "tcps") == 0)
    {   
        tcpsw_protocol();
    }

    if(strcmp(protocol_type, "udp") == 0)
    {
        udp_protocol();
    }

    if(strcmp(protocol_type, "udps") == 0)
    {
        udpsw_protocol();
    }
}