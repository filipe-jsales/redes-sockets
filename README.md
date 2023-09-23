## TCP task:

Run the localhost server:
```
$ python server.py
```


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