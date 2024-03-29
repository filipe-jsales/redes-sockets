## TCP task:

Run the localhost server:
```
$ python server.py
```

Tunnel the localhost web server to a public URL using ngrok:
```
$ ./ngrok http http://127.0.0.1:8080
```

After this, ngrok will provide a public URL that will redirect to the localhost server, eg:
```
Forwarding                    http://33c4-2804-4df4-8000-11b0-553f-d5aa-653-c065.ngrok-free.app -> http://localhost:8080
```

You can access the localhost in your browser that will work as a client for the application, additionally you can run a curl or a postman:

```
$ curl -k -X POST -d "teste" https://33c4-2804-4df4-8000-11b0-553f-d5aa-653-c065.ngrok-free.app/add_message
```

As ngrok is a free tool, we need to bypass the SSL verify by using the flag -k in curl

The client will receive a post http request and return a response, if succeeded will write in the book.txt file and display it in the web browser page

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