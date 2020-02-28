#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <error.h>
#include <unistd.h>
#include <string.h>

#define MESSAGE_SIZE 65507
#define TCP_MESSAGE_SIZE 65483
//#define TCP_MESSAGE_SIZE 65535
//65483 este dimensinea maxima la tcp, pentru o valoare mai mare fragmenteaza pachetul (-52)

extern int errno;

void tcp_protocol()  //fragmenteaza pachetele in 65483 si 52, se primesc bine bytes dar se primesc in plus 2/3 pachete
{
    int serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in from;

    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket");
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int enable = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0 )
    {
        perror("Eroare la set socket");
        return;
    }

    if(bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("Eroare la bind");
        return;
    }

    if(listen(serverSocket, 5) == -1)
    {
        perror("Eroare la listen");
        return;
    }

    while(1)
    {
        int clientSocket;
        socklen_t length = sizeof(from);

        if((clientSocket = accept(serverSocket, (struct sockaddr *) &from, &length)) < 0)
        {
            perror("Eroare la accept");
            continue;
        }

        printf("%s\n", "A fost conectat un client");

        char message[TCP_MESSAGE_SIZE];
        int messages_read_number = 0;
        long bytes_read_number = 0;
        int bytes_read = 0;

        while(1)
        {

            if((bytes_read = read(clientSocket, message, TCP_MESSAGE_SIZE)) < 0)
            {
                perror("Eroare la read2");
            }
            
            printf("%d\n", bytes_read);

            if(bytes_read == 0)
            {
                break;
            }

            messages_read_number ++;
            bytes_read_number = bytes_read_number + bytes_read;

        }
        printf("Messages read : %d\n", messages_read_number);
        printf("Bytes read : %ld\n", bytes_read_number);
        printf("Protocol used : TCP Streaming\n");
    }   
}

void tcpsw_protocol() //se primeste cu ~3kb mai mult pe server
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

        char message[TCP_MESSAGE_SIZE];
        int messages_read_number = 0;
        long bytes_read_number = 0;
        char confirm_message[3];
        int file_size;
        int nr_bytes_left;
        int ok = 0;

        read(clientSocket, &file_size, sizeof(int));

        nr_bytes_left = file_size;

        while(1)
        {
            if(nr_bytes_left <= 0)
               break;
    
            if((bytes_read = read(clientSocket, message, TCP_MESSAGE_SIZE)) < 0)
            {
                perror("Eroare la read2");
            }
            
            printf("%d\n", bytes_read);

            if(bytes_read != TCP_MESSAGE_SIZE)
            {
                ok++;

                strcpy(confirm_message , "NU\0");
                if(write(clientSocket, confirm_message, 3) < 0)
                {
                    perror("Eroare la scrierea confirm_message");
                }
                
                while(1)
                {  
                    bytes_read = read(clientSocket, message, TCP_MESSAGE_SIZE);

                    if(bytes_read != TCP_MESSAGE_SIZE)
                    {
                        strcpy(confirm_message , "NU\0");
                        write(clientSocket, confirm_message, 3);
                        continue;
                    }
                    break;
                }
            }
            strcpy(confirm_message , "DA\0");
            write(clientSocket, confirm_message, 3);

            messages_read_number = messages_read_number + 1;
            bytes_read_number = bytes_read_number + bytes_read;
            nr_bytes_left = nr_bytes_left - bytes_read;
        }

        printf("Messages read : %d\n", messages_read_number);
        printf("Bytes read : %ld\n", bytes_read_number);
        printf("File size : %d\n", file_size);
        printf("Protocol used : TCP STOP-WAIT\n");
        printf("%d\n", ok);
    }   
}

void udp_protocol()     //se primesc pachetele corect, cateva(~20) se pierd
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int serverSocket;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
      perror ("Eroare la socket.\n");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind (serverSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr)) == -1)
    {
      perror ("Eroare la bind.\n");
    }

    while(1)
    {
        socklen_t length = sizeof(clientAddr);
        char message[MESSAGE_SIZE];
        int message_size = 0;
        int nr_messages_received = 0;
        long nr_bytes_received = 0;
        int nr_messages_diff_sizes = 0;
        int bytes_received = 0;

        while(1)
        {

            if (recvfrom(serverSocket, &message_size, sizeof(int), 0,(struct sockaddr*) &clientAddr, &length) <= 0)
            {
                perror ("Eroare la primirea dimensiunii mesajului.\n");
            }    

            if ((bytes_received = recvfrom(serverSocket, message, message_size, 0,(struct sockaddr*) &clientAddr, &length)) <= 0)
            {
                perror ("Eroare la primirea mesajului.\n");
            }

            //printf("%d\n", bytes_received);
            nr_messages_received ++;
            nr_bytes_received += bytes_received;

            if(bytes_received > MESSAGE_SIZE)
                nr_messages_diff_sizes++;

            if(bytes_received < MESSAGE_SIZE || bytes_received == 0)
                break;

        }

        printf("Mesages received : %d\n", nr_messages_received);
        printf("Bytes reveived : %ld\n", nr_bytes_received);
        printf("Messages of different size : %d\n", nr_messages_diff_sizes);
    }

}

void udpsw_protocol()   //se primesc pachetele corect, 0 pierderi
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int serverSocket;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
      perror ("Eroare la socket.\n");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind (serverSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr)) == -1)
    {
      perror ("Eroare la bind.\n");
    }

    while(1)
    {
        socklen_t length = sizeof(clientAddr);
        char message[MESSAGE_SIZE];
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
                perror ("Eroare la primirea dim mesajului.\n");
            }    

            if ((bytes_received = recvfrom(serverSocket, message, message_size, 0,(struct sockaddr*) &clientAddr, &length)) <= 0)
            {
                perror ("Eroare la primirea mesajului.\n");
            }

            printf("%d\n", bytes_received);

            if(bytes_received > MESSAGE_SIZE || bytes_received < 0)
                nr_messages_diff_sizes++;

            if(bytes_received < MESSAGE_SIZE || bytes_received == 0)
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
        printf("Used protocol : UDP Stop-and-Wait\n");
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