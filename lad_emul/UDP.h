#pragma once

#include <winsock2.h>

class Address
{
public:

    Address();
    Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
    void SetAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

    Address(unsigned int address, unsigned short port);
    unsigned int GetAddress() const;
    void SetAddress(unsigned long address);
    void SetPortU(unsigned short port);
    unsigned char GetA() const;
    unsigned char GetB() const;
    unsigned char GetC() const;
    unsigned char GetD() const;
    unsigned short GetPort() const;
    bool operator == (const Address& other) const;
    bool operator != (const Address& other) const;
private:
    unsigned long address;
    unsigned short port;
};

class Socket
{
public:

    Socket();
    // ~Socket();
    bool Open(unsigned short port);
    void Close();
    bool IsOpen() const;
    bool NonBlock();
    bool Send(const Address& destination, const void* data, int size);
    int Receive(Address& sender, void* data, int size);

private:
    SOCKET Usocket = INVALID_SOCKET;

};


