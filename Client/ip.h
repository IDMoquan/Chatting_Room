#include<WinSock2.h>
#include<string>
#pragma comment(lib, "ws2_32.lib")

std::string getlocalip() {
    int ret;
    WSADATA wsaDATA;
    WSAStartup(MAKEWORD(2, 2), &wsaDATA);
    char hostname[256];
    ret = gethostname(hostname, sizeof(hostname));
    hostent* host = gethostbyname(hostname);
    return inet_ntoa(*(struct in_addr*)*host->h_addr_list);
}