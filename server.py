import socket
import threading

HOST = '127.0.0.1'
PORT = 12345

messages = []

def broadcast_message(message, client_socket):
    for client in clients:
        if client != client_socket:
            try:
                client.send(message)
            except:
                clients.remove(client)

def handle_client(client_socket):
    client_address = client_socket.getpeername()
    print(f"Cliente {client_address} conectado.")
    
    while True:
        try:
            message = client_socket.recv(1024)
            if message:
                formatted_message = f"De Cliente {client_address}: {message.decode('utf-8')}"
                print(formatted_message)
                messages.append(formatted_message.encode('utf-8'))
                broadcast_message(formatted_message.encode('utf-8'), client_socket)
            else:
                remove_client(client_socket)
        except:
            continue

def remove_client(client_socket):
    if client_socket in clients:
        client_address = client_socket.getpeername()
        print(f"Cliente {client_address} desconectado.")
        clients.remove(client_socket)

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(5)

print(f"Servidor escutando em {HOST}:{PORT}")

clients = []

while True:
    client_socket, client_address = server_socket.accept()
    clients.append(client_socket)
    print(f"Conexão estabelecida com {client_address}")

    # Iniciar uma thread para lidar com o cliente
    client_handler = threading.Thread(target=handle_client, args=(client_socket,))
    client_handler.start()
