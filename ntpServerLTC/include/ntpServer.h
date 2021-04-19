#ifndef NTPSERVER_H
#define NTPSERVER_H
#include <cstdint>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define NTP_TIMESTAMP_DELTA 2208988800ull
typedef struct sNtpPacket
{

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
    // li.   Two bits.   Leap indicator.
    // vn.   Three bits. Version number of the protocol.
    // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds. BYTE 24
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second. do BYTE 31

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

    time_t ntpTimeToLinuxTime(uint32_t aSeconds, uint32_t aFraction)
    {
        uint32_t seconds = ntohl( aSeconds ); // Time-stamp seconds.
        uint32_t fraction = ntohl( aFraction ); // Time-stamp fraction of a second.
        time_t txTm = ( time_t ) ( seconds - NTP_TIMESTAMP_DELTA );
    }

    time_t getRxTime()
    {
        return ntpTimeToLinuxTime(rxTm_s, rxTm_f);
    }
    time_t getTxTime()
    {
        return ntpTimeToLinuxTime(txTm_s, txTm_f);
    }
    time_t getOrigTime()
    {
        return ntpTimeToLinuxTime(origTm_s, origTm_f);
    }
    time_t getRefTime()
    {
        return ntpTimeToLinuxTime(refTm_s, refTm_f);
    }
    void setTxTime(time_t aSeconds)
    {   uint32_t seconds = aSeconds + NTP_TIMESTAMP_DELTA;
        seconds = htonl( seconds ); // Time-stamp seconds.
        uint32_t fraction = htonl( 0 ); // Time-stamp fraction of a second.
        txTm_s = seconds;
        txTm_f = fraction;
    }
    void setRxTime(time_t aSeconds)
    {   uint32_t seconds = aSeconds + NTP_TIMESTAMP_DELTA;
        seconds = htonl( seconds ); // Time-stamp seconds.
        uint32_t fraction = htonl( 0 ); // Time-stamp fraction of a second.
        rxTm_s = seconds;
        rxTm_f = fraction;
    }
};              // Total: 384 bits or 48 bytes.dsfds


#define LOCAL_SERVER_PORT 123

//While the Unix timescale is not shown directly in the table, the correspondence between the NTP and Unix timescales is determined only by the constant 2,208,988,800. This is the number of Gregorian seconds from the NTP prime epoch 0h, 1 January 1900 to the Unix prime epoch 0h, 1 January 1970.

class ntpServer
{
public:
    ntpServer();
    virtual ~ntpServer();
    void CreateSocket();
    int  Receive();
    int  Send(time_t);
    void ShowTxPacket();
    void ShowRxPacket();

protected:

private:
    void clearRx();
    void clearTx();
    int rxLen;
    struct sockaddr_in cliAddr, servAddr;
    int sockfd; //socket
    int rc;
    time_t ntpTimeToLinuxTime(uint32_t, uint32_t);
    sNtpPacket packetRx;
    sNtpPacket packetTx;
};

#endif // NTPSERVER_H
