import socket
import threading

HOST = '127.0.0.1'  
PORT = 12345       

# Configurar o socket do cliente
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

def receive_messages():
    while True:
        try:
            message = client_socket.recv(1024)
            if message:
                sender_info = client_socket.getpeername()  # Obter informações do remetente
                sender_ip, sender_port = sender_info
                print(f"Mensagem recebida De {sender_ip}:{sender_port}: '{message.decode('utf-8')}'")
        except:
            continue

# Iniciar uma thread para receber mensagens 
receive_thread = threading.Thread(target=receive_messages)
receive_thread.start()

while True:
    message = input()
    client_socket.send(message.encode('utf-8'))
