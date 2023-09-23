#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

char dns_servers[3][100]; /* primary, secondary, and user-specified DNS */

#define T_A 1        /* Ipv4 address */
#define T_NS 2       /* Nameserver */
#define T_CNAME 5    /* canonical name */
#define T_SOA 6      /* start of authority zone */
#define T_PTR 12     /* domain name pointer */
#define T_MX 15      /* Mail server */

static void reverseIP(char*, char*);
static void ngethost(unsigned char*, int);
static void removeDotsFromName(unsigned char*, unsigned char*);
static unsigned char* ReadName(unsigned char*, unsigned char*, int*);

/* The structure of the DNS packet will be:
   16 bits: ID
   16 bits: header
   16 bits: question
   16 bits: answer
   16 bits: authoritative answer
   16 bits: additional info
*/

/* DNS header */
struct DNS_HEADER
{
    unsigned short id; // identification number

# if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritative answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag

    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // reserved and unused
    unsigned char ra :1; // recursion available
# endif
# if __BYTE_ORDER == __BIG_ENDIAN
    unsigned char qr :1;
    unsigned char opcode :4;
    unsigned char aa :1;
    unsigned char tc :1;
    unsigned char rd :1;

    unsigned char ra :1;
    unsigned char z :1;
    unsigned char ad :1;
    unsigned char cd :1;
    unsigned char rcode :4;
# endif

    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

struct QUESTION /* QUESTION DATA */
{
    unsigned short qtype; /* query type: IN, NS, CNAME, SOA, PTR, MX */
    unsigned short qclass; /* query class: IN or CHAOS */
};

#pragma pack(push, 1)
struct R_DATA /* RESOURCE RECORD DATA */
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD /* RESOURCE RECORD FIELD: AUTHORITATIVE, ANSWER, or ADDITIONAL */
{
    unsigned char* name;
    struct R_DATA* resource;
    unsigned char* rdata;
};

typedef struct /* QUESTION FIELD */
{
    unsigned char* name;
    struct QUESTION* ques;
} QUERY;

int main(int argc, char* argv[])
{
    unsigned char hostname[100];
    char* reverse;
    int qtype = T_A;
    if (argc < 2)
    {
        std::cout << "USAGE: lookup <HOSTNAME/HOSTIP> <DNS> <QUERYTYPE>\n";
        std::cout << "QUERY TYPES:\n";
        std::cout << "IPV4 : 1\n";
        std::cout << "NAMESERVER : 2\n";
        std::cout << "CANONICAL NAME : 5\n";
        std::cout << "START OF AUTHORITY : 6\n";
        std::cout << "REVERSE QUERY : 12\n";
        std::cout << "MESSAGE EXCHANGE : 15\n";
        return 0;
    }
    if (argc > 2)
    {
        std::strcpy(dns_servers[0], argv[2]); /* USER SPECIFIED DNS */
        std::strcpy(dns_servers[2], "127.0.1.1"); /* DEFAULT SECONDARY DNS */
        std::strcpy(dns_servers[1], "172.31.1.130"); /* DEFAULT PRIMARY DNS */
    }
    else if (argc == 2)
    {
        std::strcpy(dns_servers[1], "127.0.1.1");
        std::strcpy(dns_servers[0], "172.31.1.130");
    }
    if (argc == 4)
    {
        if (std::strcmp(argv[3], "15") == 0)
            qtype = T_MX;
        else if (std::strcmp(argv[3], "12") == 0)
            qtype = T_PTR;
        else
            qtype = argv[3][0] - '0';
    }

    /* Check whether the second argument is for forward lookup or reverse lookup */
    int flag = 1;
    int i;
    for (i = 0; i < std::strlen(argv[1]); i++)
    {
        if (argv[1][i] != '.' && !(argv[1][i] >= '0' && argv[1][i] <= '9'))
        {
            flag = 0;
            break;
        }
    }
    if (flag == 1) /* reverse query */
    {
        reverseIP(argv[1], reverse);
        ngethost(reinterpret_cast<unsigned char*>(reverse), T_PTR);
        // ngethost(reverse, T_PTR);
    }
    else /* forward query */
        ngethost(reinterpret_cast<unsigned char*>(argv[1]), qtype);
        // ngethost(argv[1], qtype);
    return 0;
}

void reverseIP(char* addr, char* tar) /* change a.b.c.d to d.c.b.a.in-addr.arpa */
{
    int i, j, count_dots = 0, pos = 0;
    char buffer[10];
    for (i = std::strlen(addr) - 1; i >= 0; i--)
    {
        if (addr[i] == '.')
        {
            for (j = count_dots - 1; j >= 0; j--)
            {
                *(tar + pos) = buffer[j];
                pos++;
            }
            *(tar + pos) = '.';
            pos++;
            count_dots = 0;
        }
        else
        {
            buffer[count_dots] = addr[i];
            count_dots++;
        }
    }
    for (j = count_dots - 1; j >= 0; j--)
    {
        *(tar + pos) = buffer[j];
        pos++;
    }
    const char* arpa = ".in-addr.arpa";
    // char* arpa = ".in-addr.arpa";
    for (i = 0; i < 14; i++)
    {
        *(tar + pos) = *arpa;
        pos++;
        arpa++;
    }
}

/* perform nslookup */
void ngethost(unsigned char* host, int query_type)
{
    unsigned char buf[65536], *qname, *reader;
    int i, j, stop, s;

    struct sockaddr_in a, dest;
    struct timeval timeout;
    timeout.tv_sec = 10;

    struct RES_RECORD answers[50], auth[50], addinfo[50];

    struct DNS_HEADER* dns = NULL;
    struct QUESTION* qinfo = NULL;

    std::cout << "Resolving " << host;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); /* set timeout on this socket */

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_servers[0]);

    dns = (struct DNS_HEADER*)&buf; /* DNS HEADER */

    dns->id = (unsigned short)htons(getpid());
    dns->qr = 0;
    dns->opcode = 0; /* standard query */
    dns->aa = 0;
    dns->tc = 1;
    dns->rd = 1; /* recursion desired */
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    qname = (unsigned char*)&buf[sizeof(struct DNS_HEADER)]; /* DNS QUESTION NAME. ANY JUNK VALUE WILL DO */

    removeDotsFromName(qname, host);
    qinfo = (struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (std::strlen((const char*)qname) + 1)]; /* DNS QUESTION TYPE AND CLASS */

    qinfo->qtype = htons(query_type);
    qinfo->qclass = htons(1);

    std::cout << "\nSending Packet to " << dns_servers[0] << "\n";
    if (sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (std::strlen((const char*)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sizeof(dest)) < 0)
    {
        std::cout << "sendto failed on DNS " << dns_servers[0] << ". Attempting to send via " << dns_servers[1] << "..\n";
        dest.sin_addr.s_addr = inet_addr(dns_servers[1]);
        if (sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (std::strlen((const char*)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sizeof(dest)) < 0)
        {
            std::cout << "sendto failed on alternate DNS as well.\n";
            if (std::strcmp(dns_servers[2], "127.0.1.1") == 0)
            {
                dest.sin_addr.s_addr = inet_addr(dns_servers[2]);
                std::cout << "Final attempt on secondary DNS..\n";
                if (sendto(s, (char*)buf, sizeof(struct DNS_HEADER) + (std::strlen((const char*)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sizeof(dest)) < 0)
                {
                    std::cout << "Failed yet again..Aborting...\n";
                    return;
                }
            }
            else
                return;
        }
    }

    std::cout << "Querying done\n";

    std::cout << "Receiving answer...\n";
    i = sizeof(dest);
    if (recvfrom(s, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&i) < 0)
    {
        perror("recvfrom failed");
        return;
    }
    std::cout << "Answer received\n";

    dns = (struct DNS_HEADER*)buf;

    if (dns->ra == 0)
    {
        std::cout << "Recursion not supported..quitting\n";
        return;
    }

    if (dns->aa == 0)
        std::cout << "The server used is a non-authoritative server in the domain\n";
    else
        std::cout << "The server used is an authoritative server in the domain\n";

    if (dns->rcode == 0)
    {
        reader = &buf[sizeof(struct DNS_HEADER) + (std::strlen((const char*)qname) + 1) + sizeof(struct QUESTION)]; /* THE RESPONSE */

        std::cout << "\nThe response contains : ";
        std::cout << "\n " << ntohs(dns->q_count) << " Questions.";
        std::cout << "\n " << ntohs(dns->ans_count) << " Answers.";
        std::cout << "\n " << ntohs(dns->auth_count) << " Authoritative Servers.";
        std::cout << "\n " << ntohs(dns->add_count) << " Additional records.\n\n";

        stop = 0;

        for (i = 0; i < ntohs(dns->ans_count); i++)
        {
            answers[i].name = ReadName(reader, buf, &stop);
            reader = reader + stop;

            answers[i].resource = (struct R_DATA*)(reader);
            reader = reader + sizeof(struct R_DATA);

            if (ntohs(answers[i].resource->type) == 1) /* read address */
            {
                answers[i].rdata = (unsigned char*)std::malloc(ntohs(answers[i].resource->data_len));

                for (j = 0; j < ntohs(answers[i].resource->data_len); j++)
                    answers[i].rdata[j] = reader[j];

                answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

                reader = reader + ntohs(answers[i].resource->data_len);
            }
            else /* read name */
            {
                answers[i].rdata = ReadName(reader, buf, &stop);
                reader = reader + stop;
            }
        }

        // read authorities
        for (i = 0; i < ntohs(dns->auth_count); i++)
        {
            auth[i].name = ReadName(reader, buf, &stop);
            reader += stop;

            auth[i].resource = (struct R_DATA*)(reader);
            reader += sizeof(struct R_DATA);

            if (ntohs(auth[i].resource->type) == 1) /* read address */
            {
                auth[i].rdata = (unsigned char*)std::malloc(ntohs(auth[i].resource->data_len));
                for (j = 0; j < ntohs(auth[i].resource->data_len); j++)
                    auth[i].rdata[j] = reader[j];

                auth[i].rdata[ntohs(auth[i].resource->data_len)] = '\0';
                reader += ntohs(auth[i].resource->data_len);
            }
            else /* read name */
            {
                auth[i].rdata = ReadName(reader, buf, &stop);
                reader += stop;
            }
        }

        // read additional
        for (i = 0; i < ntohs(dns->add_count); i++)
        {
            addinfo[i].name = ReadName(reader, buf, &stop);
            reader += stop;

            addinfo[i].resource = (struct R_DATA*)(reader);
            reader += sizeof(struct R_DATA);

            if (ntohs(addinfo[i].resource->type) == 1) /* read address */
            {
                addinfo[i].rdata = (unsigned char*)std::malloc(ntohs(addinfo[i].resource->data_len));
                for (j = 0; j < ntohs(addinfo[i].resource->data_len); j++)
                    addinfo[i].rdata[j] = reader[j];

                addinfo[i].rdata[ntohs(addinfo[i].resource->data_len)] = '\0';
                reader += ntohs(addinfo[i].resource->data_len);
            }
            else /* read name */
            {
                addinfo[i].rdata = ReadName(reader, buf, &stop);
                reader += stop;
            }
        }

        // print answers
        std::cout << "\nAnswer Records : " << ntohs(dns->ans_count) << "\n";
        for (i = 0; i < ntohs(dns->ans_count); i++)
        {
            if (ntohs(answers[i].resource->type) == 12)
                std::cout << "Address : " << answers[i].name;
            else
                std::cout << "Name : " << answers[i].name;

            if (ntohs(answers[i].resource->type) == T_A) // IPv4 address
            {
                long* p;
                p = (long*)answers[i].rdata;
                a.sin_addr.s_addr = (*p);
                std::cout << " has IPv4 address : " << inet_ntoa(a.sin_addr);
            }
            else if (ntohs(answers[i].resource->type) == 5)
                std::cout << " has alias name : " << answers[i].rdata;
            else if (ntohs(answers[i].resource->type) == 12)
                std::cout << " has domain name :" << answers[i].rdata;
            std::cout << "\n";
        }

        // print authorities
        std::cout << "\nAuthoritive Records : " << ntohs(dns->auth_count) << "\n";
        for (i = 0; i < ntohs(dns->auth_count); i++)
        {

            std::cout << " Name : " << auth[i].name;
            if (ntohs(auth[i].resource->type) == 2)
                std::cout << " has nameserver : " << auth[i].rdata;
            else if (ntohs(auth[i].resource->type) == 6)
                std::cout << " has start of authority : " << auth[i].rdata;
            else if (ntohs(auth[i].resource->type) == 12)
                std::cout << " has domain name : " << auth[i].rdata;
            std::cout << "\n";
        }

        // print additional resource records
        std::cout << "\nAdditional Records : " << ntohs(dns->add_count) << "\n";
        for (i = 0; i < ntohs(dns->add_count); i++)
        {
            std::cout << " Name : " << addinfo[i].name;
            if (ntohs(addinfo[i].resource->type) == 1)
            {
                long* p;
                p = (long*)addinfo[i].rdata;
                a.sin_addr.s_addr = (*p);
                std::cout << " has IPv4 address : " << inet_ntoa(a.sin_addr);
            }
            std::cout << "\n";
        }
    }
    else
    {
        if (dns->rcode == 1)
            std::cout << "The name server was unable to interpret the query\n";
        else if (dns->rcode == 2)
            std::cout << "The name server was unable to process this query due to a problem with the name server.\n";
        else if (dns->rcode == 3)
            std::cout << "domain name referenced in the query does not exist\n";
        else if (dns->rcode == 4)
            std::cout << "The name server does not support the requested kind of query.\n";
        else if (dns->rcode == 5)
            std::cout << "The server refused to answer\n";
        else if (dns->rcode == 6)
            std::cout << "A name exists when it should not\n";
        else if (dns->rcode == 7)
            std::cout << "A resource record set exists that should not\n";
        else if (dns->rcode == 8)
            std::cout << "A resource record set that should exist does not\n";
        else if (dns->rcode == 9)
            std::cout << "The name server receiving the query is not authoritative for the zone specified\n";
        else if (dns->rcode == 10)
            std::cout << "A name specified in the message is not within the zone specified in the message\n";
        else
            std::cout << "Unknown error\n";
    }
    return;
}

u_char* ReadName(unsigned char* reader, unsigned char* buffer, int* count)
{
    unsigned char* name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char*)std::malloc(256); /* maximum allowed length is 256 */

    name[0] = '\0';

    while (*reader != 0)
    {
        if (*reader >= 192)
        {
            offset = (*reader) * 256 + *(reader + 1) - 49152;
            reader = buffer + offset - 1;
            jumped = 1;
        }
        else
            name[p++] = *reader;
        reader = reader + 1;
        if (jumped == 0)
            *count = *count + 1;
    }

    name[p] = '\0';
    if (jumped == 1)
        *count = *count + 1;

    for (i = 0; i < (int)std::strlen((const char*)name); i++)
    {
        p = name[i];
        for (j = 0; j < (int)p; j++)
        {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0';
    return name;
}

void removeDotsFromName(unsigned char* dns, unsigned char* host)
{
    int lock = 0, i;
    strcat((char*)host, ".");
    for (i = 0; i < std::strlen((char*)host); i++)
    {
        if (host[i] == '.')
        {
            *dns++ = i - lock; /* replace the dot with the number of characters after it before the next dot */
            for (; lock < i; lock++)
                *dns++ = host[lock];
            lock++;
        }
    }
    *dns++ = '\0';
}
