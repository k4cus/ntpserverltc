#include "ntpServer.h"
#include <stdlib.h>
#include <iomanip>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <iostream>
#include <cmath>

using namespace std;

ntpServer::ntpServer()
{
    clearRx();
    clearTx();
}

ntpServer::~ntpServer()
{
    //dtor
}

void ntpServer::clearRx()
{
    memset( &packetRx, 0, sizeof( sNtpPacket ) );
}
void ntpServer::clearTx()
{
    memset( &packetTx, 0, sizeof( sNtpPacket ) );
}

time_t ntpServer::ntpTimeToLinuxTime(uint32_t aSeconds, uint32_t aFraction)
{
    // These two fields contain the time-stamp seconds as the packet left the NTP server.
    // The number of seconds correspond to the seconds passed since 1900.
    // ntohl() converts the bit/byte order from the network's to host's "endianness".

    uint32_t seconds = ntohl( aSeconds ); // Time-stamp seconds.
    uint32_t fraction = ntohl( aFraction ); // Time-stamp fraction of a second.

    // Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
    // Subtract 70 years worth of seconds from the seconds since 1900.
    // This leaves the seconds since the UNIX epoch of 1970.
    // (1900)------------------(1970)**************************************(Time Packet Left the Server)

    time_t txTm = ( time_t ) ( seconds - NTP_TIMESTAMP_DELTA );
}

void ntpServer::CreateSocket()
{
    sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
    if(sockfd<0)
    {
        printf(" cannot open socket \n");
        exit(1);
    }
    /* bind local server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(LOCAL_SERVER_PORT);
    rc = bind (sockfd, (struct sockaddr *) &servAddr,sizeof(servAddr));
    if(rc<0)
    {
        printf("cannot bind port number %d \n", LOCAL_SERVER_PORT);
        exit(1);
    }

    printf("waiting for data on port UDP %u\n", LOCAL_SERVER_PORT);


}
int ntpServer::Receive()
{
    clearRx(); // prepare buffer
    int cliLen = sizeof(cliAddr);
    rxLen = recvfrom(sockfd, ( char* )&packetRx, sizeof(sNtpPacket), 0, (struct sockaddr *) &cliAddr, (socklen_t*)&cliLen);
    time_t t = time(nullptr);
    packetRx.setRxTime(t);
    ShowRxPacket();
    return(rxLen);
}
int ntpServer::Send(time_t aSeconds){
    clearTx();
    packetTx.li_vn_mode = 0x1c;
    packetTx.setTxTime(aSeconds);
    packetTx.setRxTime(aSeconds);
    packetTx.origTm_s = packetRx.txTm_s;
    packetTx.origTm_f = packetRx.txTm_f;
    int cliLen = sizeof(cliAddr);
    sendto(sockfd, ( char* )&packetTx, sizeof(sNtpPacket), 0, (struct sockaddr*) &cliAddr, cliLen);
    ShowTxPacket();
}

void ntpServer::ShowRxPacket()
{
    time_t rxTm = packetRx.getRxTime();
    time_t txTm = packetRx.getTxTime();
    time_t origTm = packetRx.getOrigTime();
    time_t refTm = packetRx.getRefTime();
    printf( "\nRx Packet");

    printf( "Time tx: %s", asctime(gmtime(&txTm )) );
    printf( "Time rx: %s", asctime(gmtime(&rxTm )) );
    printf( "Time orig: %s", asctime(gmtime(&origTm )) );
    printf( "Time ref: %s", asctime(gmtime(&refTm )) );
}

void ntpServer::ShowTxPacket()
{
    time_t rxTm = packetTx.getRxTime();
    time_t txTm = packetTx.getTxTime();
    time_t origTm = packetTx.getOrigTime();
    time_t refTm = packetTx.getRefTime();
    printf( "\nTx Packet");
    printf( "Time tx: %s", asctime(gmtime(&txTm )) );
    printf( "Time rx: %s", asctime(gmtime(&rxTm )) );
    printf( "Time orig: %s", asctime(gmtime(&origTm )) );
    printf( "Time ref: %s", asctime(gmtime(&refTm )) );

    //printf( "Time tx: %s", ctime( ( const time_t* ) &txTm ) );
    //printf( "Time rx: %s", ctime( ( const time_t* ) &rxTm ) );
    //printf( "Time orig: %s", ctime( ( const time_t* ) &origTm ) );
    //printf( "Time ref: %s", ctime( ( const time_t* ) &refTm ) );
}



/*


See Appendix A starting on page 50 of https://www.ietf.org/rfc/rfc1305.pdf for a description of the NTP packet format. Those last 8 bytes are supposed to carry the server's response timestamp -- which is exactly why you put a timestamp into those bytes when you build your minimal response packet.

However, based on the fact that this client put a value into those last 8 bytes it looks like it wants to use the mechanism described in Section 5 of the SNTP (Simple NTP) protocol, per https://www.ietf.org/rfc/rfc2030.txt SNTP uses the same packet format as NTP but uses the fields in a slightly different way. In SNTP the value the client places into bytes 40 through 47 is the client's current idea of the time. (In this case it's off by a fair bit, AFAICT that timestamp is sometime around 1951.)

If that's what this client is trying to do then it wants the server to copy those 8 bytes into bytes 24-31 (the Originate Timestamp field), and then write the server's current time into bytes 32-39 (the Receive Timestamp) and into bytes 40-47 (the Transmit Timestamp), and send that as the response. Of course, also continue to change the first byte to 0x1C in the response to indicate that this packet comes from a server. You should also set the Stratum value in byte 1 to a plausible non-zero value, something like 3 or 4.

Given that the client's clock is off by a long way, it might take several rounds of request/response for it to come into sync. So don't expect its clock to immediately jump to match the server's clock. (It might do that, but I wouldn't count on it.)

I don't think you need to complicate your logic by treating this client specially. You can probably apply exactly this same logic to packets from clients that put zeroes into the last 8 bytes. It just means you'll be copying zeroes into the Originate Timestamp field when you build responses to those clients.

*/
