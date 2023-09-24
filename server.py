import socket
from datetime import datetime


def handle_request(request_text):
    # Analisar a solicitação HTTP GET para determinar a ação
    if "GET /" in request_text:
        # Página inicial - exibir o livro de visitas
        with open("book.txt", "r") as book_file:
            messages = book_file.read()
            response_text = f"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
            response_text += "<html><head><title>Registro de Mensagens</title></head><body>"
            response_text += "<h1>Registro de Mensages</h1>"
            response_text += "<h2>Mensagens Anteriores:</h2>"
            response_text += "<p>" + messages.replace("\n", "<br>") + "</p>"
            response_text += "</body></html>"
        return response_text
    elif "POST /add_message" in request_text:
        # Adicionar uma nova mensagem via POST
        message = request_text.split("\r\n\r\n")[-1]
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        with open("book.txt", "a") as book_file:
            book_file.write(f"{timestamp}: {message}\n")
        # Redirecionar de volta para a página inicial
        response_text = "HTTP/1.1 302 Found\r\nLocation: /\r\n\r\n"
        return response_text
    else:
        # Página não encontrada
        response_text = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nPágina não encontrada."
        return response_text

def start_server():
    HOST = '127.0.0.1'
    PORT = 8080

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)

    print(f"Servidor web do Livro de Visitas ouvindo em {HOST}:{PORT}")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Conexão de {client_address}")

        request = client_socket.recv(1024).decode('utf-8')

        if request:
            response = handle_request(request)
            client_socket.send(response.encode('utf-8'))

        client_socket.close()

if __name__ == "__main__":
    start_server()
