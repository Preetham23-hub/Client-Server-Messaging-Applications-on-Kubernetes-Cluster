#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <bits/stdc++.h>
#define DELIM "."

using namespace std;
using namespace std::chrono;

bool isStep = true;

// Thread Function to call the stepTime
void NegatingBool(int stepTime)
{
    while (1)
    {
        this_thread::sleep_for(chrono::minutes(stepTime));
        isStep = !isStep;
        cout << "*** CYCLE EXECUTED***" << endl;
    }
}

// Function for validating an IpAddress

// check if a given string is a numeric string or not
bool isNumber(const string &str)
{
    // `std::find_first_not_of` searches the string for the first character
    // that does not match any of the characters specified in its arguments
    return !str.empty() &&
           (str.find_first_not_of("[0123456789]") == std::string::npos);
}

// Function to split string `str` using a given delimiter
vector<string> split(const string &str, char delim)
{
    auto i = 0;
    vector<string> list;

    auto pos = str.find(delim);

    while (pos != string::npos)
    {
        list.push_back(str.substr(i, pos - i));
        i = ++pos;
        pos = str.find(delim, pos);
    }

    list.push_back(str.substr(i, str.length()));

    return list;
}

// Function to validate an IP address
bool validateIP(string ip)
{
    // split the string into tokens
    vector<string> list = split(ip, '.');

    // if the token size is not equal to four
    if (list.size() != 4)
    {
        return false;
    }

    // validate each token
    for (string str : list)
    {
        // verify that the string is a number or not, and the numbers
        // are in the valid range
        if (!isNumber(str) || stoi(str) > 255 || stoi(str) < 0)
        {
            return false;
        }
    }

    return true;
}

// Used commandLine arguments for taking inputs
int main(int argc, char **argv)
{

    if (argc != 5)
    {
        cout << "Input error" << endl;
        return 0;
    }

    // First Input will be server IP Address

    string ipAddress = argv[1];

    // Validating IpAddress
    if (validateIP(ipAddress) == false)
    {
        cout << "IP Address is not valid, Enter the valid IP" << endl;
        return 0;
    }

    // Second Input will be port number

    int port = stoi(argv[2]);

    if (port < 0 && port > 65535)
    {
        cout << "Port number is not valid";
        return 0;
    }

    // Third Input will be sleep value between 1 to 60 Seconds

    int sleepTime = stoi(argv[3]);

    if (sleepTime <= 0)
    {
        cout << "Wrong sleepTime input";
        return 0;
    }

    // Forth input will be the step time in Minutes

    int stepTime = stoi(argv[4]);

    if (stepTime <= 0)
    {
        cout << "Wrong stepTime input";
        return 0;
    }

    // create socket
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        cerr << "Can't create the socket";
        return 0;
    }

    // fill sockaddr in structure
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Connect to server
    int conResult = connect(client, (sockaddr *)&hint, sizeof(hint));
    if (conResult == -1)
    {
        cerr << "Can't connect";
        return 0;
    }

    // Thread created to implement stepTime
    thread t(NegatingBool, stepTime);

    // Buffer is declared to write the value of received message
    char buf[4096];

    // Declaring Message counter
    unsigned long long int count = 1;

    // using while for sending the message continiously by running infinate loop.
    while (1)
    {

        string msgCount = to_string(count);
        string msg = "Message from the client  num = " + msgCount;

        if (msg.length() > 0)
        {
            // send the msg back to server
            memset(buf, 4096, 0);
            int sent = send(client, msg.c_str(), msg.size() + 1, 0);

            // Incrementing the message count
            count++;

            if (sent != -1)
            {

                // wait for the response
                memset(buf, 4096, 0);
                int bytesReceived = recv(client, buf, 4096, 0);
                if (bytesReceived > 0)
                {

                    // print the msg to console
                    cout << buf << endl;

                    // making the thread sleep for x seconds
                    if (isStep == true)
                    {
                        this_thread::sleep_for(chrono::milliseconds(sleepTime * 1000));
                    }
                    else
                    {
                        this_thread::sleep_for(chrono::milliseconds((sleepTime * 1000) / 2));
                    }
                }
            }
        }
    }

    // close the socket
    t.join();
    close(client);

    return 0;
}
