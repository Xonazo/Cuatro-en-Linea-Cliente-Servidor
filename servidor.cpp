#include <iostream>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring> // Para la función memset
#include <thread> // Para manejar múltiples conexiones con hilos
#include <mutex> // Para la sincronización de datos compartidos
#include <algorithm>

class Juego {
private:
    int clienteSocket;
    std::string identificador; // Identificador único para el cliente

public:
    Juego(int socket, std::string id) : clienteSocket(socket), identificador(id) {}

    int getClienteSocket() const { return clienteSocket; }
    std::string getIdentificador() const { return identificador; }
};

class Servidor {
private:
    int puerto;
    std::vector<Juego> juegos;
    std::mutex mutexJuegos; // Para sincronizar el acceso a la lista de juegos

public:
    Servidor(int puerto) : puerto(puerto) {}

    void enviarMensaje(int socket, const std::string& mensaje) {
        send(socket, mensaje.c_str(), mensaje.size(), 0);
    }

    void enviarMensajeBienvenida(int socket) {
        enviarMensaje(socket, "¡Bienvenido al servidor de juegos!\n");
    }

    void manejarCliente(int clienteSocket, const sockaddr_in& clienteDireccion) {
        std::string identificador = "cliente" + std::to_string(juegos.size() + 1); // Generar identificador único
        Juego nuevoJuego(clienteSocket, identificador);

        {
            std::lock_guard<std::mutex> lock(mutexJuegos);
            juegos.push_back(nuevoJuego);
        }

        std::cout << "Nuevo cliente conectado [" << inet_ntoa(clienteDireccion.sin_addr) << ":" << ntohs(clienteDireccion.sin_port) << "] - Identificador: " << identificador << std::endl;

        enviarMensajeBienvenida(clienteSocket);

       //Mantener la conexión y verificar si el cliente se desconecta
        while (true) {
            char buffer[1024];
            int bytesRecibidos = recv(clienteSocket, buffer, sizeof(buffer), 0);
            if (bytesRecibidos <= 0) {
                std::cerr << "El cliente " << identificador << " ha cerrado la conexión o ha ocurrido un error" << std::endl;
            
                {
                    std::lock_guard<std::mutex> lock(mutexJuegos);
         
                    auto it = std::find_if(juegos.begin(), juegos.end(), [&](const Juego& juego) {
                        return juego.getClienteSocket() == clienteSocket;
                    });
                    if (it != juegos.end()) {
                        juegos.erase(it);
                    }
                }
                break;
            }
        }

        close(clienteSocket);
    }

    void iniciar() {
        int servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (servidorSocket == -1) {
            std::cerr << "Error al crear el socket del servidor" << std::endl;
            return;
        }

        struct sockaddr_in direccion;
        direccion.sin_family = AF_INET;
        direccion.sin_addr.s_addr = INADDR_ANY;
        direccion.sin_port = htons(puerto);

        if (bind(servidorSocket, (struct sockaddr *)&direccion, sizeof(direccion)) == -1) {
            std::cerr << "Error al vincular el socket a la dirección y puerto" << std::endl;
            close(servidorSocket);
            return;
        }

        if (listen(servidorSocket, 10) == -1) {
            std::cerr << "Error al escuchar por conexiones entrantes" << std::endl;
            close(servidorSocket);
            return;
        }

        std::cout << "Esperando conexiones ..." << std::endl;

        while (true) {
            struct sockaddr_in clienteDireccion;
            socklen_t clienteTamano = sizeof(clienteDireccion);
            int clienteSocket = accept(servidorSocket, (struct sockaddr *)&clienteDireccion, &clienteTamano);
            if (clienteSocket == -1) {
                std::cerr << "Error al aceptar la conexión entrante" << std::endl;
                continue;
            }

            std::thread clienteThread(&Servidor::manejarCliente, this, clienteSocket, clienteDireccion);
            clienteThread.detach(); // Desconectamos el hilo del hilo principal
        }

        close(servidorSocket);
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 1;
    }

    int puerto = std::atoi(argv[1]);

    Servidor servidor(puerto);
    servidor.iniciar();

    return 0;
}