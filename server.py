import socket
import time

def handle_request(client_socket):
    request_data = client_socket.recv(1024).decode('utf-8')
    if not request_data:
        return

    request_lines = request_data.split('\n')
    first_line = request_lines[0].strip()
    method, path, _ = first_line.split()

    if method == 'GET':
        if path == '/':
            current_time = time.strftime('%Y-%m-%d %H:%M:%S')
            response = 'HTTP/1.1 200 OK\nContent-Type: text/html\n\n'
            response += '<html><body>'
            response += '<div style="text-align: center">'
            response += '<h1 style="color: green">Hora Atual:</h1>'
            response += f'<p>{current_time}</p>'
            response += '</div>'
            response += '<br/>'
            response += '<br/>'
            response += '<br/>'
            response += '<br/>'
            response += '<div style="text-align: center">'
            response += 'HTTP/1.1 200 OK\nContent-Type: text/html\n\n'
            response += '</div>'
            response += '</body></html>'
        else:
            response = 'HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n'
            response += '<html><body><h1>404 Not Found</h1></body></html>'
    else:
        response = 'HTTP/1.1 405 Method Not Allowed\nContent-Type: text/html\n\n'
        response += '<html><body><h1>405 Method Not Allowed</h1></body></html>'

    print(response)
    client_socket.sendall(response.encode('utf-8'))
    client_socket.close()

host = '127.0.0.1'
port = 8080

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(5)

print(f"Servidor rodando em http://{host}:{port}/")

while True:
    client_socket, addr = server_socket.accept()
    print(f"Conexão de {addr[0]}:{addr[1]}")
    handle_request(client_socket)
