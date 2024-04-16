#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring> // Para la función memset

class Cliente
{
private:
    std::string ipServidor;
    int puerto;
    int clienteSocket;

public:
    Cliente(std::string ipServidor, int puerto) : ipServidor(ipServidor), puerto(puerto), clienteSocket(-1) {}

    bool conectar()
    {
        clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clienteSocket == -1)
        {
            std::cerr << "Error al crear el socket del cliente" << std::endl;
            return false;
        }

        struct sockaddr_in direccion;
        direccion.sin_family = AF_INET;
        direccion.sin_port = htons(puerto);
        inet_pton(AF_INET, ipServidor.c_str(), &direccion.sin_addr);

        // Conectar al servidor
        if (connect(clienteSocket, (struct sockaddr *)&direccion, sizeof(direccion)) == -1)
        {
            std::cerr << "Error al conectar con el servidor" << std::endl;
            close(clienteSocket);
            return false;
        }

        return true;
    }

    void cerrarConexion()
    {
        if (clienteSocket != -1)
        {
            close(clienteSocket);
            clienteSocket = -1;
        }
    }

    void iniciarJuego()
    {
        if (clienteSocket == -1)
        {
            std::cerr << "El cliente no está conectado al servidor" << std::endl;
            return;
        }

        while (true)
        {
            char buffer[1024];
            int bytesRecibidos = recv(clienteSocket, buffer, sizeof(buffer), 0);
            if (bytesRecibidos <= 0)
            {
                std::cerr << "El servidor ha cerrado la conexión o ha ocurrido un error" << std::endl;
                break;
            }

            buffer[bytesRecibidos] = '\0';
            std::cout << "Mensaje del servidor: " << buffer << std::endl;
        }

        cerrarConexion();
    }
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Uso: " << argv[0] << " <ip_servidor> <puerto>" << std::endl;
        return 1;
    }

    std::string ipServidor = argv[1];
    int puerto = std::atoi(argv[2]);

    Cliente cliente(ipServidor, puerto);
    if (!cliente.conectar())
    {
        std::cerr << "Error al conectar al servidor" << std::endl;
        return 1;
    }

    cliente.iniciarJuego();

    return 0;
}
