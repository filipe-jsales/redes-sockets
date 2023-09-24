## TCP task:

Run the localhost server:
```
$ python server.py
```

Run 2 clients in different cmd's:
```
$ python client.py
```

Then you can send messages in the prompt line, the message will be sent through sockets to the other client, the server also will be able to see the message records.

## UDP task:

Install g++ compiler:
```
$ sudo apt-get update
$ sudo apt-get install g++
```

Run nslookup eg:
```
$ g++ nslookup.cpp -o nslookup                            
$ ./nslookup google.com 8.8.8.8
```

The dns_client.cpp is a low-level UDP socket implementation while nslookup.cpp is a high-level implementation using the getaddrinfo() function.



To make this code perform DNS queries for specific record types (A, NS, CNAME, SOA, PTR, and MX), you need to pass the desired record type as a command-line argument when running the code. The record types are represented by numeric values in the code:

- A (IPv4 address): Represented by the value 1.
- NS (Nameserver): Represented by the value 2.
- CNAME (Canonical Name): Represented by the value 5.
- SOA (Start of Authority): Represented by the value 6.
- PTR (Domain Name Pointer): Represented by the value 12.
- MX (Mail Server): Represented by the value 15.

Here's how you can run the code to perform DNS queries for specific record types:

```
    ./dns_lookup <HOSTNAME/IP> <DNS_SERVER> <RECORD_TYPE>
    <HOSTNAME/IP>: Replace this with the hostname or IP address for which you want to perform the DNS query.
    <DNS_SERVER> (optional): Replace this with the DNS server you want to use for the query. If not specified, it will use default DNS servers.
    <RECORD_TYPE>: Replace this with the numeric value corresponding to the record type you want to query (e.g., 1 for A, 2 for NS, etc.).
```


For example, to perform an A record query for the domain example.com using a specific DNS server (8.8.8.8), you would run:

```
./dns_lookup example.com 8.8.8.8   1
```

```
./dns_lookup example.com 1 5
```

In case there's none, will be displayed the default option (A record).