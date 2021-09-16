#include "UDP.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "UDP.h"
#include <winsock2.h>
#include <iostream>
#include <string>

Address::Address()
{
    address = 0;
    port = 0;
}
Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
{
    address = (a << 24) | (b << 16) | (c << 8) | d;
    Address::port = port;
}
void Address::SetAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
    address = (a << 24) | (b << 16) | (c << 8) | d;
}

Address::Address(unsigned int address, unsigned short port)
{
    Address::address = address;
    Address::port = port;
}

unsigned int Address::GetAddress() const
{
    return address;
}
void Address::SetAddress(unsigned long address)
{
    Address::address = address;
}
void Address::SetPortU(unsigned short port)
{
    Address::port = port;
}
unsigned char Address::GetA() const
{
    return (address & (255 << 24)) >> 24;
}

unsigned char Address::GetB() const
{
    return (address & (255 << 16)) >> 16;
}

unsigned char Address::GetC() const
{
    return (address & (255 << 8)) >> 8;
}

unsigned char Address::GetD() const
{
    return (address & 255);
}

unsigned short Address::GetPort() const
{
    return port;
}

//bool operator == (const Address& other) const;
//bool operator != (const Address& other) const;



//  unsigned long address;
 // unsigned short port;



Socket::Socket()
{

}
//~Socket();

bool Socket::Open(unsigned short port)
{
    int handle;
    WSADATA WsaData;
    handle = WSAStartup(MAKEWORD(2, 2), &WsaData);

    if (handle != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %d\n", handle);
        return 1;
    }

    Usocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (Usocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)port);

    if (bind(Usocket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
    {
        std::cout << "bind error" << std::endl;
        return false;
    }

    return true;

}

void Socket::Close()
{

    closesocket(Usocket);
    WSACleanup();
}

bool Socket::IsOpen() const
{
    return socket;
}

bool Socket::NonBlock()
{
    DWORD nonBlocking = 1;
    if (ioctlsocket(Usocket, FIONBIO, &nonBlocking) != 0)
    {
        return false;
    }
}

bool Socket::Send(const Address& destination, const void* data, int size)
{
    sockaddr_in toaddress;
    memset(&toaddress, 0, sizeof(toaddress));
    toaddress.sin_family = AF_INET;
    toaddress.sin_addr.s_addr = htonl(destination.GetAddress());
    toaddress.sin_port = htons(destination.GetPort());

    int sent_bytes = sendto(Usocket, (const char*)data, size,
        0, (const sockaddr*)&toaddress, sizeof(sockaddr_in));



    if (sent_bytes != size)
    {
        std::cout << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

int Socket::Receive(Address& sender, void* data, int size)
{
    int recived_bytes;

    sockaddr_in from;
    memset(&from, 0, sizeof(from));
    int fromLength = sizeof(from);

    recived_bytes = recvfrom(Usocket, (char*)data, size, 0, (sockaddr*)&from, &fromLength);

    sender.SetAddress(ntohl(from.sin_addr.s_addr));
    sender.SetPortU(ntohs(from.sin_port));

    return recived_bytes;
}

////  SOCKET Usocket;



