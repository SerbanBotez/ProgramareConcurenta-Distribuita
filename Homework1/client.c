#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <error.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define MESSAGE_SIZE 65507
//65507
//#define TCP_MESSAGE_SIZE 65535
#define TCP_MESSAGE_SIZE 65483
#define LOCALHOST_ADRESS "127.0.0.1"
#define SERVER_ADRESS "35.226.70.186"

extern int errno;

void tcp_protocol()
{
        int clientSocket;
        struct sockaddr_in serverAddr;

        if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("Eroare la socket");
            return;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(2030);
        serverAddr.sin_addr.s_addr = inet_addr(LOCALHOST_ADRESS);
        //

        if(connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) == -1)
        {
            perror("Eroare la connect");
            return;
        }

        FILE *file = fopen("file.txt", "r");

        if(file == NULL)
        {
            printf("%s\n", "Eroare la deschiderea fisierului");
            perror("Eroare la deschiderea fisierului");
        }

        int i, c;
        int message_size;
        char message[TCP_MESSAGE_SIZE];
        int messages_sent_number = 0;
        int bytes_sent_number = 0;
        int bytes_sent = 0;
        clock_t start,end;

        start = clock();
        do
        {
            for(i = 0; i < TCP_MESSAGE_SIZE; i++)
            {
                c = fgetc(file);
                if(c == EOF)
                    break;
                else
                {
                    message[i] = c;
                }      
            }
            message_size = i;
        
            if((bytes_sent = write(clientSocket, message, message_size)) <= 0)
            {
                perror("Eroare la write2");
            }

            printf("%d\n", bytes_sent);
            
            messages_sent_number++;
            bytes_sent_number = bytes_sent_number + bytes_sent;

        }
        while(c !=EOF);
        
        fclose(file);
        close(clientSocket);

        end = clock();

        double transmission_time;

        transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("Transmission time : %f seconds\n", transmission_time);
        printf("Messages sent : %d\n", messages_sent_number);
        printf("Bytes sent %d\n", bytes_sent_number);
}

void tcpsw_protocol()
{
        int clientSocket;
        struct sockaddr_in serverAddr;

        if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("%s\n", "Eroare la socket");
            return;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(2030);
        serverAddr.sin_addr.s_addr = inet_addr(LOCALHOST_ADRESS);

        if(connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) == -1)
        {
            printf("%s\n", "Eroare la connect");
            return;
        }

        FILE *file = fopen("file.txt", "r");

        if(file == NULL)
        {
            perror("Eroare la deschiderea fisierului");
        }

        fseek(file, 0L, SEEK_END);
        int file_size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        write(clientSocket, &file_size, sizeof(int));

        int i, c;
        char message[TCP_MESSAGE_SIZE];
        char confirm_message[3];
        int messages_sent_number = 0;
        int bytes_sent_number = 0;
        int bytes_sent = 0;
        int message_size = 0;
        clock_t start,end;

        start = clock();
        do
        {
            for(i = 0; i < TCP_MESSAGE_SIZE; i++)
            {
                c = fgetc(file);
                if(c == EOF)
                    break;
                message[i] = c;
                  
            }

            message_size = i;          

            if((bytes_sent = write(clientSocket, message, message_size)) <= 0)
            {
                perror("Eroare la write2");
            }

            printf("%d\n", bytes_sent);

            if(read(clientSocket, confirm_message, 3) < 0)
            {
                perror("Eroare la citirea confirm_message");
            }

            //printf("%s\n", confirm_message);
            //printf("%s\n", "aici2");

            if(strcmp(confirm_message, "DA") == 0)
            {
                messages_sent_number ++;
                bytes_sent_number = bytes_sent_number + bytes_sent;
            }

            if(strcmp(confirm_message, "NU") == 0)
            {
                while(1)
                {
                    //printf("%s\n", "aici2");
                    write(clientSocket, message, message_size);

                    read(clientSocket, confirm_message, 3);

                    if(strcmp(confirm_message, "DA") == 0)
                    {
                        messages_sent_number = messages_sent_number + 1;
                        bytes_sent_number = bytes_sent_number + i;
                        break;
                    }
                }
            }
        }
        while(c !=EOF);
        
        //fclose(file);
        //close(clientSocket);

        end = clock();

        double transmission_time;

        transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("Transmission time : %f seconds\n", transmission_time);
        printf("Messages sent : %d\n", messages_sent_number);
        printf("Bytes sent %d\n", bytes_sent_number);
        printf("File size : %d\n", file_size);
}

void udp_protocol()
{

    struct sockaddr_in serverAddr;
    int serverSocket;
    socklen_t length;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM,  IPPROTO_UDP)) == -1)
    {
      perror ("Eroare la socket.\n");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = inet_addr(LOCALHOST_ADRESS); 
    //"35.226.70.186"

    length = sizeof(serverAddr);

    char message[MESSAGE_SIZE];
    int message_size;
    int bytes_sent = 0;
    int nr_messages_sent = 0;
    long nr_bytes_sent = 0;
    clock_t start, end;

    FILE *file = fopen("file.txt", "r");
    int i;
    char c;

    start = clock();
    do
    {
        for(i = 0; i < MESSAGE_SIZE; i++)
        {
            c = fgetc(file);
            if(c == EOF)
                break;
            else
            {
                message[i] = c;
            }      
        }

        message_size = i;

        if (sendto (serverSocket, &message_size, sizeof(int), 0, (struct sockaddr*)&serverAddr, length) <= 0)
        {
            perror ("Eroare la trimiterea dimensiunii mesajului.\n");
        }

        if ((bytes_sent = sendto (serverSocket, message, message_size,0, (struct sockaddr*)&serverAddr, length)) <= 0)
        {
            perror ("Eroare la trimiterea mesajului.\n");
        }

        //printf("%d\n", bytes_sent);

        nr_messages_sent ++;
        nr_bytes_sent += bytes_sent;
    }
    while(c != EOF);
    end = clock();

    double transmission_time;
    transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Transmission time : %f seconds \n", transmission_time);
    printf("Mesages sent : %d\n", nr_messages_sent);
    printf("Bytes sent : %ld\n", nr_bytes_sent);
    printf("Protocol : UDP Streaming\n");
}

void udpsw_protocol()
{

    struct sockaddr_in serverAddr;
    int serverSocket;
    socklen_t length;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM,  IPPROTO_UDP)) == -1)
    {
      perror ("Eroare la socket.\n");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = inet_addr(LOCALHOST_ADRESS); 
    //"35.226.70.186"

    length = sizeof(serverAddr);

    char message[MESSAGE_SIZE];
    char compare_message[3];
    int message_size;
    int nr_messages_sent = 0;
    long nr_bytes_sent = 0;
    int bytes_sent;
    clock_t start, end;

    FILE *file = fopen("file.txt", "r");
    int i;
    char c;

    start = clock();
    do
    {
        for(i = 0; i < MESSAGE_SIZE; i++)
        {
            c = fgetc(file);
            if(c == EOF)
                break;
            else
            {
                message[i] = c;
            }      
        }
        message_size = i;

        if (sendto (serverSocket, &message_size, sizeof(int), 0, (struct sockaddr*)&serverAddr, length) <= 0)
        {
            perror ("Eroare la trimiterea dim mesajului.\n");
        }

        if ((bytes_sent = sendto (serverSocket, message, message_size, 0, (struct sockaddr*)&serverAddr, length)) <= 0)
        {
            perror ("Eroare la trimiterea mesajului.\n");
        }

        printf("%d\n", bytes_sent);

        recvfrom(serverSocket, compare_message, 3, 0,(struct sockaddr*) &serverAddr, &length);

        if(strcmp(compare_message, "NU") == 0)
        {
            while(1)
            {
                sendto (serverSocket, &message_size, sizeof(int), 0, (struct sockaddr*)&serverAddr, length);
                bytes_sent = sendto (serverSocket, message, message_size,0, (struct sockaddr*)&serverAddr, length);

                recvfrom(serverSocket, compare_message, 3, 0,(struct sockaddr*) &serverAddr, &length);

                if(strcmp(compare_message, "NU"))
                    continue;
                if(strcmp(compare_message, "DA"))
                    break;
            }
        }

        if(strcmp(compare_message, "DA") == 0)
        {
            nr_messages_sent ++;
            nr_bytes_sent += bytes_sent;
            continue;
        }
    }
    while(c != EOF);
    end = clock();

    double transmission_time;
    transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Transmission time : %f seconds \n", transmission_time);
    printf("Mesages sent : %d\n", nr_messages_sent);
    printf("Bytes sent : %ld\n", nr_bytes_sent);
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