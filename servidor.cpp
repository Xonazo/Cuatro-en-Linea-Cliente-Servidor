#include <iostream>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring> // Para la función memset
#include <thread>  // Para manejar múltiples conexiones con hilos
#include <mutex>   // Para la sincronización de datos compartidos
#include <algorithm>
#include <random> // Para generar números aleatorios

class Juego
{
private:
    int clienteSocket;
    std::string identificador; // Identificador único para el cliente

public:
    Juego(int socket, std::string id) : clienteSocket(socket), identificador(id) {}

    int getClienteSocket() const { return clienteSocket; }
    std::string getIdentificador() const { return identificador; }
};

class Servidor
{
private:
    int puerto;
    std::vector<Juego> juegos;
    std::mutex mutexJuegos;                 // Para sincronizar el acceso a la lista de juegos
    std::vector<std::vector<char>> tablero; // Tablero del juego
    const char jugador = 'C';               // Ficha del jugador
    const char maquina = 'S';               // Ficha de la máquina
    int contadorClientes = 0;               // Contador de clientes

public:
    Servidor(int puerto) : puerto(puerto) {}

    void enviarMensaje(int socket, const std::string &mensaje)
    {
        send(socket, mensaje.c_str(), mensaje.size(), 0);
    }

    void enviarMensajeBienvenida(int socket)
    {
        enviarMensaje(socket, "¡Bienvenido al servidor de juegos!\n");
    }

    void inicializarTablero()
    {
        tablero.resize(6, std::vector<char>(7, ' ')); // Inicializar el tablero con espacios en blanco
    }

    void mostrarTablero(int socket)
    {
        std::string mensaje = "\nTABLERO\n";
        for (int i = 0; i < 6; ++i)
        {
            mensaje += std::to_string(i + 1) + " ";
            for (int j = 0; j < 7; ++j)
            {
                mensaje += tablero[i][j];
                mensaje += " ";
            }
            mensaje += "\n";
        }
        mensaje += "-------------\n";
        mensaje += " 1 2 3 4 5 6 7\n";

        enviarMensaje(socket, mensaje);
    }

    bool verificarJugada(int columna)
    {
        return tablero[0][columna] == ' '; // Verificar si la casilla está vacía
    }

    void realizarJugada(int columna, char ficha)
    {
        for (int i = 5; i >= 0; --i)
        {
            if (tablero[i][columna] == ' ')
            {
                tablero[i][columna] = ficha;
                break;
            }
        }
    }

    bool verificarGanador(char ficha)
    {
        // Verificar horizontalmente
        for (int i = 0; i < 6; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (tablero[i][j] == ficha && tablero[i][j + 1] == ficha && tablero[i][j + 2] == ficha && tablero[i][j + 3] == ficha)
                {
                    return true;
                }
            }
        }
        // Verificar verticalmente
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 7; ++j)
            {
                if (tablero[i][j] == ficha && tablero[i + 1][j] == ficha && tablero[i + 2][j] == ficha && tablero[i + 3][j] == ficha)
                {
                    return true;
                }
            }
        }
        // Verificar en diagonal (de izquierda a derecha)
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (tablero[i][j] == ficha && tablero[i + 1][j + 1] == ficha && tablero[i + 2][j + 2] == ficha && tablero[i + 3][j + 3] == ficha)
                {
                    return true;
                }
            }
        }
        // Verificar en diagonal (de derecha a izquierda)
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 3; j < 7; ++j)
            {
                if (tablero[i][j] == ficha && tablero[i + 1][j - 1] == ficha && tablero[i + 2][j - 2] == ficha && tablero[i + 3][j - 3] == ficha)
                {
                    return true;
                }
            }
        }
        return false;
    }

    int obtenerColumnaMaquina()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 6);
        return dis(gen);
    }

    void manejarCliente(int clienteSocket, const sockaddr_in &clienteDireccion)
    {
        std::string identificador = "cliente" + std::to_string(++contadorClientes); // Incrementar el contador de clientes
        Juego nuevoJuego(clienteSocket, identificador);

        {
            std::lock_guard<std::mutex> lock(mutexJuegos);
            juegos.push_back(nuevoJuego);
        }

        std::cout << "Nuevo cliente conectado [" << inet_ntoa(clienteDireccion.sin_addr) << ":" << ntohs(clienteDireccion.sin_port) << "] - Identificador: " << identificador << std::endl;

        enviarMensajeBienvenida(clienteSocket);

        // Inicializar el tablero
        inicializarTablero();

        bool finJuego = false;
        while (!finJuego)
        {
            mostrarTablero(clienteSocket);
            // Turno del jugador
            enviarMensaje(clienteSocket, "Es tu turno. Ingrese el número de columna (1-7): ");
            char columna_buffer[1024];
            int bytesRecibidos = recv(clienteSocket, columna_buffer, sizeof(columna_buffer), 0);
            if (bytesRecibidos <= 0)
            {
                std::cerr << "El cliente " << identificador << " se desconectó o ocurrió un error durante la recepción de datos." << std::endl;
                break; // Salir del bucle si la conexión se ha cerrado o ha ocurrido un error
            }
            columna_buffer[bytesRecibidos] = '\0';
            int columna = std::stoi(columna_buffer);
            columna--; // Ajustar columna a índice base 0
            if (columna < 0 || columna >= 7 || !verificarJugada(columna))
            {
                enviarMensaje(clienteSocket, "Columna inválida. Inténtelo de nuevo.\n");
                continue;
            }
            realizarJugada(columna, jugador);
            // Dentro de la función manejarCliente()

            std::cout << identificador << ": Cliente juega columna " << columna + 1 << std::endl;
            // Enviar el mensaje al cliente indicando el movimiento de la máquina
            enviarMensaje(clienteSocket, "servidor juega columna " + std::to_string(columna + 1) + "\n");

            if (verificarGanador(jugador))
            {
                mostrarTablero(clienteSocket);
                enviarMensaje(clienteSocket, "\n¡Felicidades! ¡Has ganado!\nFin del juego.\n");
                finJuego = true;
                break;
            }

            // Turno de la máquina
            int columnaMaquina = obtenerColumnaMaquina();
            while (!verificarJugada(columnaMaquina))
            {
                columnaMaquina = obtenerColumnaMaquina();
            }
            realizarJugada(columnaMaquina, maquina);
            std::cout << "Servidor juega columna " << columnaMaquina + 1 << std::endl;

            if (verificarGanador(maquina))
            {
                mostrarTablero(clienteSocket);
                enviarMensaje(clienteSocket, "\n¡La máquina ha ganado!\nFin del juego.\n");
                finJuego = true;
                break;
            }
        }

        close(clienteSocket);

        // Eliminar el juego de la lista
        {
            std::lock_guard<std::mutex> lock(mutexJuegos);
            auto it = std::find_if(juegos.begin(), juegos.end(), [&](const Juego &juego)
                                   { return juego.getClienteSocket() == clienteSocket; });
            if (it != juegos.end())
            {
                juegos.erase(it);
            }
        }
    }
    void iniciar()
    {
        int servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (servidorSocket == -1)
        {
            std::cerr << "Error al crear el socket del servidor" << std::endl;
            return;
        }

        struct sockaddr_in direccion;
        direccion.sin_family = AF_INET;
        direccion.sin_addr.s_addr = INADDR_ANY;
        direccion.sin_port = htons(puerto);

        if (bind(servidorSocket, (struct sockaddr *)&direccion, sizeof(direccion)) == -1)
        {
            std::cerr << "Error al vincular el socket a la dirección y puerto" << std::endl;
            close(servidorSocket);
            return;
        }

        if (listen(servidorSocket, 10) == -1)
        {
            std::cerr << "Error al escuchar por conexiones entrantes" << std::endl;
            close(servidorSocket);
            return;
        }

        std::cout << "Esperando conexiones ..." << std::endl;

        while (true)
        {
            std::cout << "Esperando para aceptar una nueva conexión..." << std::endl;
            struct sockaddr_in clienteDireccion;
            socklen_t clienteTamano = sizeof(clienteDireccion);
            int clienteSocket = accept(servidorSocket, (struct sockaddr *)&clienteDireccion, &clienteTamano);
            if (clienteSocket == -1)
            {
                std::cerr << "Error al aceptar la conexión entrante" << std::endl;
                continue;
            }

            std::cout << "Nueva conexión aceptada. Creando hilo para manejar al cliente..." << std::endl;

            std::thread clienteThread(&Servidor::manejarCliente, this, clienteSocket, clienteDireccion);
            clienteThread.detach(); // Desconectamos el hilo del hilo principal
        }

        // No es necesario cerrar el servidorSocket aquí, ya que este bucle es infinito
    }
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 1;
    }

    int puerto = std::atoi(argv[1]);

    Servidor servidor(puerto);
    servidor.iniciar();

    return 0;
}
