#ifndef _NTPCLIENT_
#define _NTPCLIENT_

#include "mbed.h"
//use nslookup 2.pool.ntp.org in cmd to find ip address

//#define NTP_DEFULT_NIST_SERVER_ADDRESS "2.pool.ntp.org"
#define NTP_DEFULT_NIST_SERVER_ADDRESS "109.74.206.120"
#define NTP_DEFULT_NIST_SERVER_PORT 123

class NTPClient {
    public:
        NTPClient(NetworkInterface *iface);
        void set_server(char* server, int port);
        time_t get_timestamp(int timeout = 10000);

    private:
        NetworkInterface *iface;
        char* nist_server_address;
        int nist_server_port;

        uint32_t ntohl(uint32_t num);
};

#endif
