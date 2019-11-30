/*
  IPAddress.h - Base class that provides IPAddress
  Copyright (c) 2011 Adrian McEwen.  All right reserved.

  Modified (ported to mbed) by Zoltan Hudak <hudakz@inbox.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <stdint.h>
#include <stdio.h>

// A class to make it easier to handle and pass around IP addresses

class   IPAddress
{
private:
    uint8_t     _address[4];    // IPv4 address

    // Access the raw byte array containing the address.  Because this returns a pointer
    // to the internal structure rather than a copy of the address this function should only
    // be used when you know that the usage of the returned uint8_t* will be transient and not

    // stored.
    uint8_t*    raw_address(void)   { return _address; };
public:
    // Constructors
    IPAddress(void);
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddress(uint32_t address);
    IPAddress(const uint8_t* address);

    // Overloaded cast operator to allow IPAddress objects to be used where a pointer
    // to a four-byte uint8_t array is expected
    operator uint32_t(void) const    { return *((uint32_t*)_address); };
    bool operator==(const IPAddress& addr) const { return(*((uint32_t*)_address)) == (*((uint32_t*)addr._address)); };
    bool operator==(const uint8_t* addr) const;

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](int index) const { return _address[index]; };
    uint8_t &operator[](int index)      { return _address[index]; };

    // Overloaded copy operators to allow initialisation of IPAddress objects from other types
    IPAddress &operator =(const uint8_t* address);
    IPAddress &operator =(uint32_t address);

    // Returns IP Address as string of char
    char* toString(void) {
        static char buff[16];
        uint8_t     i = 0;
        uint8_t     j = 0;

        for (i = 0; i < 3; i++) {
            j += sprintf(&buff[j], "%d", _address[i]);
            buff[j++] = '.';
        }

        j += sprintf(&buff[j], "%d", _address[i]);
        buff[j] = '\0';
        return buff;
    }

    //    virtual size_t printTo(Print& p) const;
    friend class        EthernetClass;
    friend class        UDP;
    friend class        Client;
    friend class        Server;
    friend class        DhcpClass;
    friend class        DNSClient;
};

const IPAddress INADDR_NONE(0, 0, 0, 0);
#endif
