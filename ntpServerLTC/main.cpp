#include <ntpServer.h>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <stdlib.h> // defines putenv in POSIX
#include <time.h>

using namespace std;
int main(int argc, char *argv[])
{

    ntpServer s;
    time_t t;
    struct tm* timeinfo;
    s.CreateSocket();

    while(1)
    {

        s.Receive();
// generate timestamp in localtime
        t = time(nullptr);
        timeinfo = localtime(&t);
        //cout << t << "\n" << timeinfo->tm_gmtoff <<endl;
        t = t + timeinfo->tm_gmtoff;
        s.Send(t);
    }


    return 0;
}
