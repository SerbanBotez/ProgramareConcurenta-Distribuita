#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <error.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

extern int errno;

void tcp_protocol()
{
        int serverSocket;
        struct sockaddr_in serverAddr;

        if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("%s\n", "Eroare la socket");
            return;
        }

        //int nr = strlen(protocol_type);

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(2030);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if(connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) == -1)
        {
            printf("%s\n", "Eroare la connect");
            return;
        }

        FILE *file = fopen("file.txt", "r");

        if(file == NULL)
        {
            printf("%s\n", "Eroare la deschiderea fisierului");
            perror("Eroare la deschiderea fisierului");
        }

        //read message_size from file

        int i;
        int c;
        char message[65335];
        int messages_sent_number = 0;
        int bytes_sent_number = 0;
        clock_t start,end;

        start = clock();
        do
        {
            //memset(message, 0, sizeof(message));
            for(i = 0; i < 65335; i++)
            {
                c = fgetc(file);
                if(c == EOF)
                    break;
                else
                {
                    //printf("%c", c);
                    message[i] = c;
                }      
            }

            //message[i] = '\0';
            int message_size = i;
            printf("%d\n", message_size);
            //printf("%s\n", message);

            if(message_size == 0)
            {
                if(write(serverSocket, &message_size, sizeof(int)) <= 0)
                {
                    perror("Eroare la write");
                }

                break;
            }

            if(write(serverSocket, &message_size, sizeof(int)) <= 0)
            {
                perror("Eroare la write1");
            }

            if(write(serverSocket, message, message_size) <= 0)
            {
                perror("Eroare la write2");
            }

            messages_sent_number = messages_sent_number + 1;
            bytes_sent_number = bytes_sent_number + i;

        }
        while(c !=EOF);
        
        end = clock();

        double transmission_time;

        transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("Transmission time : %f seconds\n", transmission_time);
        printf("Messages sent : %d\n", messages_sent_number);
        printf("Bytes sent %d\n", bytes_sent_number);
}

void tcpsw_protocol()
{
        int serverSocket;
        struct sockaddr_in serverAddr;

        if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("%s\n", "Eroare la socket");
            return;
        }

        //int nr = strlen(protocol_type);

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(2030);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if(connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) == -1)
        {
            printf("%s\n", "Eroare la connect");
            return;
        }

        FILE *file = fopen("file.txt", "r");

        if(file == NULL)
        {
            printf("%s\n", "Eroare la deschiderea fisierului");
            perror("Eroare la deschiderea fisierului");
        }

        //read message_size from file

        int i;
        int c;
        char message[65335];
        char confirm_message[3];
        int messages_sent_number = 0;
        int bytes_sent_number = 0;
        int bytes_sent = 0;
        clock_t start,end;

        start = clock();
        do
        {
            for(i = 0; i < 65335; i++)
            {
                c = fgetc(file);
                if(c == EOF)
                    break;
                else
                {
                    //printf("%c", c);
                    message[i] = c;
                }      
            }

            int message_size = i;          

            if(message_size == 0)
            {
                if(write(serverSocket, &message_size, sizeof(int)) <= 0)
                {
                    perror("Eroare la write");
                }

                break;
            }

            if(write(serverSocket, &message_size, sizeof(int)) <= 0)
            {
                perror("Eroare la write1");
            }

            if((bytes_sent = write(serverSocket, message, message_size)) <= 0)
            {
                perror("Eroare la write2");
            }

            printf("%d\n", bytes_sent);

            if(read(serverSocket, confirm_message, 3) < 0)
            {
                perror("Eroare la citirea confirm_message");
            }

            //printf("%s\n", confirm_message);
            //printf("%s\n", "aici2");

            if(strcmp(confirm_message, "DA") == 0)
            {
                messages_sent_number = messages_sent_number + 1;
                bytes_sent_number = bytes_sent_number + i;
            }

            if(strcmp(confirm_message, "NU") == 0)
            {
                while(1)
                {
                    //printf("%s\n", "aici2");
                    write(serverSocket, &message_size, sizeof(int));
                    write(serverSocket, message, message_size);

                    read(serverSocket, confirm_message, 3);

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
        
        end = clock();

        double transmission_time;

        transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("Transmission time : %f seconds\n", transmission_time);
        printf("Messages sent : %d\n", messages_sent_number);
        printf("Bytes sent %d\n", bytes_sent_number);
}

void udp_protocol()
{

    struct sockaddr_in serverAddr;
    int serverSocket;
    socklen_t length;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM,  IPPROTO_UDP)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      //return errno;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    length = sizeof(serverAddr);

    char message[65335];
    int message_size;
    int nr_messages_sent = 0;
    long bytes_sent = 0;
    clock_t start, end;

    FILE *file = fopen("file.txt", "r");
    int i;
    char c;

    start = clock();
    do
    {
        for(i = 0; i < 65335; i++)
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

        printf("%d\n", message_size);

        if (sendto (serverSocket, &message_size, sizeof(int), 0, (struct sockaddr*)&serverAddr, length) <= 0)
        {
            perror ("[client]Eroare la sendto() spre server.\n");
            //perror (errno);
        }

        if (sendto (serverSocket, message, message_size,0, (struct sockaddr*)&serverAddr, length) <= 0)
        {
            perror ("[client]Eroare la sendto() spre server.\n");
            //perror (errno);
        }

        nr_messages_sent ++;
        bytes_sent += message_size;
    }
    while(c != EOF);
    end = clock();

    double transmission_time;
    transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Transmission time : %f seconds \n", transmission_time);
    printf("Mesages sent : %d\n", nr_messages_sent);
    printf("Bytes sent : %ld\n", bytes_sent);
}

void udpsw_protocol()
{

    struct sockaddr_in serverAddr;
    int serverSocket;
    socklen_t length;

    if ((serverSocket = socket (AF_INET, SOCK_DGRAM,  IPPROTO_UDP)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      //return errno;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2030);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    length = sizeof(serverAddr);

    char message[65335];
    char compare_message[3];
    int message_size;
    int nr_messages_sent = 0;
    long bytes_sent = 0;
    clock_t start, end;

    FILE *file = fopen("file.txt", "r");
    int i;
    char c;

    start = clock();
    do
    {
        for(i = 0; i < 65335; i++)
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

        printf("%d\n", message_size);

        if (sendto (serverSocket, &message_size, sizeof(int), 0, (struct sockaddr*)&serverAddr, length) <= 0)
        {
            perror ("[client]Eroare la sendto() spre server.\n");
        }

        if (sendto (serverSocket, message, message_size,0, (struct sockaddr*)&serverAddr, length) <= 0)
        {
            perror ("[client]Eroare la sendto() spre server.\n");
        }

        recvfrom(serverSocket, compare_message, 3, 0,(struct sockaddr*) &serverAddr, &length);

        if(strcmp(compare_message, "NU") == 0)
        {
            while(1)
            {
                sendto (serverSocket, &message_size, sizeof(int), 0, (struct sockaddr*)&serverAddr, length);
                sendto (serverSocket, message, message_size,0, (struct sockaddr*)&serverAddr, length);

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
            bytes_sent += message_size;
            continue;
        }
    }
    while(c != EOF);
    end = clock();

    double transmission_time;
    transmission_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Transmission time : %f seconds \n", transmission_time);
    printf("Mesages sent : %d\n", nr_messages_sent);
    printf("Bytes sent : %ld\n", bytes_sent);
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
        //printf("%s", "DA\n");
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