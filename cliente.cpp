#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>

class Cliente
{
private:
    std::string ipServidor;
    int puerto;
    int clienteSocket;

public:
    Cliente(std::string ipServidor, int puerto) : ipServidor(ipServidor), puerto(puerto), clienteSocket(-1) {}

    /*funcion conectar con el servidor*/
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

        /*conectar con el servidor*/
        if (connect(clienteSocket, (struct sockaddr *)&direccion, sizeof(direccion)) == -1)
        {
            std::cerr << "Error al conectar con el servidor" << std::endl;
            close(clienteSocket);
            return false;
        }

        return true;
    }

    /*cerrar la conexion*/
    void cerrarConexion()
    {
        if (clienteSocket != -1)
        {
            close(clienteSocket);
            clienteSocket = -1;
        }
    }

    /*inicia el juego*/
    void iniciarJuego()
    {
        if (clienteSocket == -1)
        {
            std::cerr << "El cliente no está conectado al servidor" << std::endl;
            return;
        }

        while (true)
        {
            /*buffer para los datos del servidor*/
            char buffer[1024];
            int bytesRecibidos = recv(clienteSocket, buffer, sizeof(buffer), 0);
            if (bytesRecibidos <= 0)
            {
                std::cerr << "El servidor ha cerrado la conexión o ha ocurrido un error" << std::endl;
                break;
            }

            buffer[bytesRecibidos] = '\0';
            std::cout << " " << buffer << std::endl;

            if (strstr(buffer, "Es tu turno.") != nullptr)
            {
                /* Si se detecta que es el turno del cliente, solicita al usuario que ingrese la columna donde jugara*/
                std::string input;
                bool entradaValida = false;
                while (!entradaValida)
                {
                    std::cout << "Ingrese el numero de columna (1-7): ";
                    std::getline(std::cin, input);

                    bool isValid = true;
                    for (char c : input)
                    {
                        if (!std::isdigit(c))
                        {
                            isValid = false;
                            break;
                        }
                    }

                    if (isValid)
                    {
                        int columna = std::stoi(input);
                        if (columna < 1 || columna > 7)
                        {
                            std::cout << "Columna invalida. Por favor, ingrese un numero entre 1 y 7." << std::endl;
                        }
                        else
                        {
                            entradaValida = true;
                            send(clienteSocket, input.c_str(), input.size(), 0);
                        }
                    }
                    else
                    {
                        std::cout << "Error: La entrada no es un numero. Intente nuevamente." << std::endl;
                    }
                }
            }
            else if (strstr(buffer, "servidor juega") != nullptr)
            {
                std::cout << buffer << std::endl;
            }
        }

        cerrarConexion();
    }
};

int main(int argc, char *argv[])
{
    /*verificacion de entradas usuario*/
    if (argc != 3)
    {
        std::cerr << "Uso: " << argv[0] << " <ip_servidor> <puerto>" << std::endl;
        return 1;
    }

    std::string ipServidor = argv[1];
    int puerto = std::atoi(argv[2]);

    /*objeto cliente*/
    Cliente cliente(ipServidor, puerto);
    if (!cliente.conectar())
    {
        std::cerr << "Error al conectar al servidor" << std::endl;
        return 1;
    }

    /*inicia el juego*/
    cliente.iniciarJuego();

    return 0;
}
