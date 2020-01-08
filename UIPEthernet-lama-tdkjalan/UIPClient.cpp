/*
 UIPClient.cpp - Arduino implementation of a UIP wrapper class.
 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 Modified (ported to mbed) by Zoltan Hudak <hudakz@inbox.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
extern "C"
{
#include "utility/uip-conf.h"
#include "utility/uip.h"
#include "utility/uip_arp.h"
#include "utility/millis.h"
#include "string.h"
}
#include "UIPEthernet.h"
#include "UIPClient.h"
#include "Dns.h"

#define UIP_TCP_PHYH_LEN    UIP_LLH_LEN + UIP_IPTCPH_LEN

uip_userdata_t UIPClient::all_data[UIP_CONNS];

/**
 * @brief
 * @note
 * @param
 * @retval
 */
UIPClient::UIPClient(void) :
    data(NULL)
{ }

/**
 * @brief
 * @note
 * @param
 * @retval
 */
UIPClient::UIPClient(uip_userdata_t* conn_data) :
    data(conn_data)
{ }

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::connect(IPAddress ip, uint16_t port) {
    stop();

    uip_ipaddr_t    ipaddr;
    uip_ip_addr(ipaddr, ip);

    struct uip_conn*    conn = uip_connect(&ipaddr, htons(port));
    if (conn)
    {
#if UIP_CONNECT_TIMEOUT > 0
        int32_t timeout = millis() + 1000 * UIP_CONNECT_TIMEOUT;
#endif
        while ((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED) {
            uIPEthernet.tick();
            if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED) {
                data = (uip_userdata_t*)conn->appstate;
#ifdef UIPETHERNET_DEBUG_CLIENT
                printf("connected, state: %d, first packet in: %d\r\n", data->state, data->packets_in[0]);
#endif
                return 1;
            }

#if UIP_CONNECT_TIMEOUT > 0
            if (((int32_t) (millis() - timeout)) > 0) {
                conn->tcpstateflags = UIP_CLOSED;
                break;
            }
#endif
        }
    }

    return 0;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::connect(const char* host, uint16_t port) {

    // Look up the host first
    int         ret = 0;
#if UIP_UDP
    DNSClient   dns;
    IPAddress   remote_addr;

    dns.begin(UIPEthernet::_dnsServerAddress);
    ret = dns.getHostByName(host, remote_addr);
    if (ret == 1) {
        return connect(remote_addr, port);
    }
#endif
    return ret;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPClient::stop(void) {
    if (data && data->state)
    {
#ifdef UIPETHERNET_DEBUG_CLIENT
        printf("before stop(), with data\r\n");
        _dumpAllData();
#endif
        _flushBlocks(&data->packets_in[0]);
        if (data->state & UIP_CLIENT_REMOTECLOSED) {
            data->state = 0;
        }
        else {
            data->state |= UIP_CLIENT_CLOSE;
        }

#ifdef UIPETHERNET_DEBUG_CLIENT
        printf("after stop()\r\n");
        _dumpAllData();
#endif
    }

#ifdef UIPETHERNET_DEBUG_CLIENT
    else {
        printf("stop(), data: NULL\r\n");
    }
#endif
    data = NULL;
    uIPEthernet.tick();
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
uint8_t UIPClient::connected(void) {
    return(data && (data->packets_in[0] != NOBLOCK || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
bool UIPClient::operator==(const UIPClient& rhs) {
    return data && rhs.data && (data == rhs.data);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
UIPClient::operator bool(void) {
    uIPEthernet.tick();
    return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in[0] != NOBLOCK);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
size_t UIPClient::write(uint8_t c) {
    return _write(data, &c, 1);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
size_t UIPClient::write(const uint8_t* buf, size_t size) {
    return _write(data, buf, size);
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
size_t UIPClient::_write(uip_userdata_t* u, const uint8_t* buf, size_t size) {
    int         remain = size;
    uint16_t    written;
#if UIP_ATTEMPTS_ON_WRITE > 0
    uint16_t    attempts = UIP_ATTEMPTS_ON_WRITE;
#endif
    repeat : uIPEthernet.tick();
    if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))) {
        uint8_t p = _currentBlock(&u->packets_out[0]);
        if (u->packets_out[p] == NOBLOCK)
        {
newpacket:
            u->packets_out[p] = uIPEthernet.network.allocBlock(UIP_SOCKET_DATALEN);
            if (u->packets_out[p] == NOBLOCK)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
                if ((--attempts) > 0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                    goto repeat;
#endif
                goto ready;
            }

            u->out_pos = 0;
        }

#ifdef UIPETHERNET_DEBUG_CLIENT
        printf("UIPClient.write: writePacket(%d) pos: %d, buf[%d-%d]\r\n", u->packets_out[p], u->out_pos, size - remain, remain);
#endif
        written = uIPEthernet.network.writePacket
            (
                u->packets_out[p],
                u->out_pos,
                (uint8_t*)buf + size - remain,
                remain
            );
        remain -= written;
        u->out_pos += written;
        if (remain > 0) {
            if (p == UIP_SOCKET_NUMPACKETS - 1)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
                if ((--attempts) > 0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                    goto repeat;
#endif
                goto ready;
            }

            p++;
            goto newpacket;
        }

ready:
#if UIP_CLIENT_TIMER >= 0
        u->timer = millis() + UIP_CLIENT_TIMER;
#endif
        return size - remain;
    }

    return -1;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::available(void) {
    if (*this)
        return _available(data);
    return 0;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::_available(uip_userdata_t* u) {
    int len = 0;
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
        len += uIPEthernet.network.blockSize(u->packets_in[i]);
    }

    return len;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::read(uint8_t* buf, size_t size) {
    if (*this) {
        uint16_t    remain = size;
        if (data->packets_in[0] == NOBLOCK)
            return 0;

        uint16_t    read;
        do {
            read = uIPEthernet.network.readPacket(data->packets_in[0], 0, buf + size - remain, remain);
            if (read == uIPEthernet.network.blockSize(data->packets_in[0])) {
                remain -= read;
                _eatBlock(&data->packets_in[0]);
                if
                (
                    uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS])
                &&  !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))
                ) data->state |= UIP_CLIENT_RESTART;
                if (data->packets_in[0] == NOBLOCK) {
                    if (data->state & UIP_CLIENT_REMOTECLOSED) {
                        data->state = 0;
                        data = NULL;
                    }

                    return size - remain;
                }
            }
            else {
                uIPEthernet.network.resizeBlock(data->packets_in[0], read);
                break;
            }
        } while (remain > 0);
        return size;
    }

    return -1;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::read(void) {
    static uint8_t  c;
    if (read(&c, 1) < 0)
        return -1;
    return c;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
int UIPClient::peek(void) {
    static uint8_t  c;
    if (*this) {
        if (data->packets_in[0] != NOBLOCK) {
            uIPEthernet.network.readPacket(data->packets_in[0], 0, &c, 1);
            return c;
        }
    }

    return -1;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPClient::flush(void) {
    if (*this) {
        _flushBlocks(&data->packets_in[0]);
    }
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void uipclient_appcall(void) {
    uint16_t            send_len = 0;
    uip_userdata_t*     u = (uip_userdata_t*)uip_conn->appstate;
    if (!u && uip_connected())
    {
#ifdef UIPETHERNET_DEBUG_CLIENT
        printf("UIPClient uip_connected\r\n");
        UIPClient::_dumpAllData();
#endif
        u = (uip_userdata_t*)UIPClient::_allocateData();
        if (u) {
            uip_conn->appstate = u;
#ifdef UIPETHERNET_DEBUG_CLIENT
            printf("UIPClient allocated state: %d", u->state);
#endif
        }

#ifdef UIPETHERNET_DEBUG_CLIENT
        else
            printf("UIPClient allocation failed\r\n");
#endif
    }

    if (u) {
        if (uip_newdata())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
            printf("UIPClient uip_newdata, uip_len: %d\r\n", uip_len);
#endif
            if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED))) {
                for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
                    if (u->packets_in[i] == NOBLOCK) {
                        u->packets_in[i] = uIPEthernet.network.allocBlock(uip_len);
                        if (u->packets_in[i] != NOBLOCK) {
                            uIPEthernet.network.copyPacket
                                (
                                    u->packets_in[i],
                                    0,
                                    UIPEthernet::in_packet,
                                    ((uint8_t*)uip_appdata) - uip_buf,
                                    uip_len
                                );
                            if (i == UIP_SOCKET_NUMPACKETS - 1)
                                uip_stop();
                            goto finish_newdata;
                        }
                    }
                }

                UIPEthernet::packetstate &= ~UIPETHERNET_FREEPACKET;
                uip_stop();
            }
        }

finish_newdata:
        if (u->state & UIP_CLIENT_RESTART) {
            u->state &= ~UIP_CLIENT_RESTART;
            uip_restart();
        }

        // If the connection has been closed, save received but unread data.
        if (uip_closed() || uip_timedout())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
            printf("UIPClient uip_closed\r\n");
            UIPClient::_dumpAllData();
#endif
            // drop outgoing packets not sent yet:

            UIPClient::_flushBlocks(&u->packets_out[0]);
            if (u->packets_in[0] != NOBLOCK) {
                ((uip_userdata_closed_t*)u)->lport = uip_conn->lport;
                u->state |= UIP_CLIENT_REMOTECLOSED;
            }
            else
                u->state = 0;

            // disassociate appdata.
#ifdef UIPETHERNET_DEBUG_CLIENT
            printf("after UIPClient uip_closed\r\n");
            UIPClient::_dumpAllData();
#endif
            uip_conn->appstate = NULL;
            goto finish;
        }

        if (uip_acked())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
            printf("UIPClient uip_acked\r\n");
#endif
            UIPClient::_eatBlock(&u->packets_out[0]);
        }

        if (uip_poll() || uip_rexmit())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
            //printf("UIPClient uip_poll\r\n");
#endif
            if (u->packets_out[0] != NOBLOCK) {
                if (u->packets_out[1] == NOBLOCK) {
                    send_len = u->out_pos;
                    if (send_len > 0) {
                        uIPEthernet.network.resizeBlock(u->packets_out[0], 0, send_len);
                    }
                }
                else
                    send_len = uIPEthernet.network.blockSize(u->packets_out[0]);
                if (send_len > 0) {
                    UIPEthernet::uip_hdrlen = ((uint8_t*)uip_appdata) - uip_buf;
                    UIPEthernet::uip_packet = uIPEthernet.network.allocBlock(UIPEthernet::uip_hdrlen + send_len);
                    if (UIPEthernet::uip_packet != NOBLOCK) {
                        uIPEthernet.network.copyPacket
                            (
                                UIPEthernet::uip_packet,
                                UIPEthernet::uip_hdrlen,
                                u->packets_out[0],
                                0,
                                send_len
                            );
                        UIPEthernet::packetstate |= UIPETHERNET_SENDPACKET;
                    }
                }

                goto finish;
            }
        }

        // don't close connection unless all outgoing packets are sent
        if (u->state & UIP_CLIENT_CLOSE)
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
            printf("UIPClient state UIP_CLIENT_CLOSE\r\n");
            UIPClient::_dumpAllData();
#endif
            if (u->packets_out[0] == NOBLOCK) {
                u->state = 0;
                uip_conn->appstate = NULL;
                uip_close();
#ifdef UIPETHERNET_DEBUG_CLIENT
                printf("no blocks out -> free userdata\r\n");
                UIPClient::_dumpAllData();
#endif
            }
            else {
                uip_stop();
#ifdef UIPETHERNET_DEBUG_CLIENT
                printf("blocks outstanding transfer -> uip_stop()\r\n");
#endif
            }
        }
    }

finish:
    uip_send(uip_appdata, send_len);
    uip_len = send_len;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
uip_userdata_t* UIPClient::_allocateData(void) {
    for (uint8_t sock = 0; sock < UIP_CONNS; sock++) {
        uip_userdata_t*     data = &UIPClient::all_data[sock];
        if (!data->state) {
            data->state = sock | UIP_CLIENT_CONNECTED;
            memset(&data->packets_in[0], 0, sizeof(uip_userdata_t) - sizeof(data->state));
            return data;
        }
    }

    return NULL;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
uint8_t UIPClient::_currentBlock(memhandle* block) {
    for (uint8_t i = 1; i < UIP_SOCKET_NUMPACKETS; i++) {
        if (block[i] == NOBLOCK)
            return i - 1;
    }

    return UIP_SOCKET_NUMPACKETS - 1;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPClient::_eatBlock(memhandle* block)
{
#ifdef UIPETHERNET_DEBUG_CLIENT
    memhandle*  start = block;
    printf("eatblock(%d): ", *block);
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
        printf("%d ", start[i]);
    }

    printf("-> ");
#endif
    uIPEthernet.network.freeBlock(block[0]);
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS - 1; i++) {
        block[i] = block[i + 1];
    }

    block[UIP_SOCKET_NUMPACKETS - 1] = NOBLOCK;
#ifdef UIPETHERNET_DEBUG_CLIENT
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
        printf("%d ", start[i]);
    }

    printf("\r\n");
#endif
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPClient::_flushBlocks(memhandle* block) {
    for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++) {
        uIPEthernet.network.freeBlock(block[i]);
        block[i] = NOBLOCK;
    }
}

#ifdef UIPETHERNET_DEBUG_CLIENT

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPClient::_dumpAllData(void) {
    for (uint8_t i = 0; i < UIP_CONNS; i++) {
        printf("UIPClient::all_data[%d], state:%d packets_in: ", i, all_data[i].state);
        for (uint8_t j = 0; j < UIP_SOCKET_NUMPACKETS; j++) {
            printf("%d ", all_data[i].packets_in[j]);
        }

        printf("\r\n");
        if (all_data[i].state & UIP_CLIENT_REMOTECLOSED) {
            printf("state remote closed, local port: %d", htons(((uip_userdata_closed_t *) (&all_data[i]))->lport));
            printf("\r\n");
        }
        else {
            printf("packets_out: ");
            for (uint8_t j = 0; j < UIP_SOCKET_NUMPACKETS; j++) {
                printf("%d ", all_data[i].packets_out[j]);
            }

            printf("\r\n");
            printf("out_pos: %d\r\n", all_data[i].out_pos);
        }
    }
}
#endif

