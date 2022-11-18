#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <chrono>
#include <fstream>
#include <ctime>
#include <iomanip>

using namespace std;

// Creating mutex
std::mutex m;
pthread_mutex_t count_mutex;

// Declaring a pthread
pthread_t newThread;

// Creating a counter for sent and received message
unsigned long long int sentCount = 0;
unsigned long long int receivedCount = 0;

// A pthread function for resetting the counter
void *makingCountZero(void *arg)
{

    // Taking stepTime as arguement
    int stepTime = (*(int *)arg);

    // Using while to sleep the thread in loop
    while (1)
    {
        sleep(stepTime * 60);

        // Using pThread_mutex to achive threadSafe
        pthread_mutex_lock(&count_mutex);

        // making counter zero
        sentCount = 0;
        receivedCount = 0;

        cout << "***CYCLE FINISHED****" << endl;

        pthread_mutex_unlock(&count_mutex);
    }
}

// function for increasing the sent count
void increaseSentCount()
{
    // Using mutex to achive threadSafe
    m.lock();
    sentCount++;
    m.unlock();
}

// function for increasing the receive count
void increaseReceiveCount()
{
    // Using mutex to achive threadSafe
    m.lock();
    receivedCount++;
    m.unlock();
}

int main(int argc, char **argv)
{

    // Checking the number of argc
    if (argc != 3)
    {
        cout << "Wrong input";
        return 0;
    }

    // Reading port number from command-line arguement

    int port = stoi(argv[1]);

    if (port < 0 && port > 65535)
    {
        cout << "Port number is not valid";
        return 0;
    }

    // Taking stepTime
    int stepTime = stoi(argv[2]);

    if (stepTime <= 0)
    {
        cout << "Wrong stepTime input";
        return 0;
    }

    // creating a socket

    int listening_socket, client_socket;
    listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket == -1)
    {
        cout << "can't create socket" << endl;
        return 0;
    }

    // bind the socket with ip addr and port no.
    sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    memset(server.sin_zero, 8, 0);

    unsigned len = sizeof(sockaddr_in);
    if ((bind(listening_socket, (sockaddr *)&server, len)) == -1)
    {
        cout << "can't bind" << endl;
        return 0;
    }

    // start listening
    if (listen(listening_socket, 5) < 0)
    {
        cout << "Listen Failed" << endl;
        return 0;
    }

    // wait for connection

    if ((client_socket = accept(listening_socket, (sockaddr *)&client, &len)) == -1)
    {
        cout << "can't accept" << endl;
        return 0;
    }

    // Declaring the buffer for writing message from the client
    char buf[4096];

    // Declaring a Message counter
    unsigned long long int count = 1;

    string msg = "message from server";

    // Mutex initialization
    if (pthread_mutex_init(&count_mutex, NULL) != 0)
    {
        cout << "mutex init failed" << endl;
        return 1;
    }

    // Creating a pThread
    pthread_create(&newThread, NULL, makingCountZero, &stepTime);

    while (1)
    {
        // printing the counter
        cout << "Send Count = " << sentCount << " Received count = " << receivedCount << endl;
        string msgCount = to_string(count);
        string msg = "message from the Server num " + msgCount;

        // wait for client to send msg
        memset(buf, 4096, 0);
        int bytesReceived = recv(client_socket, buf, 4096, 0);
        if (bytesReceived > 0)
        {

            // Increasing the receivedCount
            increaseReceiveCount();

            // Printing the message from the client
            cout << buf << endl;

            // print back msg to client
            int sent = send(client_socket, msg.c_str(), msg.size() + 1, 0);
            count++;
            // cout<<"sent"<<sent<<"bytes to client"<<inet_ntoa(client.sin_addr)<<endl;

            // Increasing the sentCount
            increaseSentCount();
        }
    }

    // Using join() for joining child thread with main thread
    pthread_join(newThread, NULL);

    // Destroying the initialized mutex
    pthread_mutex_destroy(&count_mutex);

    pthread_exit(NULL);

    // close socket
    close(client_socket);
}
