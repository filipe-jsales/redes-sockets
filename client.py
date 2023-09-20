import socket

def udp_dns_query(domain, record_type):
    # Configurar o servidor DNS e o socket UDP
    dns_server = ('8.8.8.8', 53)  # Servidor DNS do Google
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Cabeçalho DNS
    transaction_id = b'\xaa\xaa'
    flags = b'\x01\x00'
    question_count = b'\x00\x01'
    answer_count = b'\x00\x00'
    authority_count = b'\x00\x00'
    additional_count = b'\x00\x00'
    header = transaction_id + flags + question_count + answer_count + authority_count + additional_count

    # Tipo de registro (A para consulta de endereço IPv4 ou AAAA para consulta de endereço IPv6)
    query_type = b'\x00\x01' if record_type == 'A' else b'\x00\x1c'

    # Nome do domínio a ser consultado (formato de lista de labels)
    qname = b''
    for part in domain.split('.'):
        label_length = bytes([len(part)])
        label = part.encode('utf-8')
        qname += label_length + label
    qname += b'\x00'  # Terminador nulo

    # Montar a consulta DNS completa
    query = header + qname + query_type + b'\x00\x01'  # Classe (IN para Internet) e tipo de consulta (A ou AAAA)

    # Enviar a consulta DNS para o servidor DNS
    sock.sendto(query, dns_server)

    # Configurar um timeout para a resposta
    sock.settimeout(1)

    try:
        # Receber a resposta do servidor DNS
        data, addr = sock.recvfrom(1024)
    except socket.timeout:
        return None

    # Retornar apenas a parte de resposta da mensagem DNS (ignorando o cabeçalho)
    return data[12:]

if __name__ == '__main__':
    domain = "www.google.com"
    result = udp_dns_query(domain, 'A')
    if result:
        print(f"Consulta DNS para {domain} (Tipo A):")
        print(f"Resultado: {result.hex()}")
    else:
        print(f"Não foi possível obter uma resposta para {domain}")
