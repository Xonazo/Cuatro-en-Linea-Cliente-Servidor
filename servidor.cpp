#include <iostream>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <algorithm>
#include <random>

class Juego
{
private:
    int clienteSocket;
    std::string identificador; // identificador para el cliente

    std::vector<std::vector<char>> tablero;

public:
    Juego(int socket, std::string id) : clienteSocket(socket), identificador(id)
    {
        tablero.resize(6, std::vector<char>(7, ' '));
    }

    int getClienteSocket() const { return clienteSocket; }
    std::string getIdentificador() const { return identificador; }
    std::vector<std::vector<char>> &getTablero() { return tablero; }
};

class Servidor
{
private:
    int puerto;
    std::vector<Juego> juegos;
    std::mutex mutexJuegos;
    const char jugador = 'C'; // letra del jugador
    const char maquina = 'S'; // letra de la máquina
    int contadorClientes = 0; // contador de clientes

public:
    Servidor(int puerto) : puerto(puerto) {}

    void enviarMensaje(int socket, const std::string &mensaje)
    {
        send(socket, mensaje.c_str(), mensaje.size(), 0);
    }

    void enviarMensajeBienvenida(int socket)
    {
        enviarMensaje(socket, "¡Bienvenido a cuatro linea!\n");
    }

    /*Verificacion de limites*/
    bool verificarJugada(const std::vector<std::vector<char>> &tablero, int columna)
    {
        if (columna < 0 || columna >= 7)
        {
            return false;
        }
        return tablero[0][columna] == ' ';
    }

    void realizarJugada(std::vector<std::vector<char>> &tablero, int columna, char ficha)
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

    /* Verificacion Empate*/
    bool verificarEmpate(const std::vector<std::vector<char>> &tablero)
    {

        for (int j = 0; j < 7; ++j)
        {
            if (tablero[0][j] == ' ')
            {
                return false;
            }
        }

        return true;
    }

    /*Verificaciones de juego*/
    bool verificarGanador(const std::vector<std::vector<char>> &tablero, char ficha)
    {
        /*verificar horizontalmente*/
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
        /* verificar verticalmente*/
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
        /* verificar en diagonal*/
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
        /*Verificar en diagonal (derecha-izquierda)*/
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

    int obtenerColumnaMaquina(const std::vector<std::vector<char>> &tablero)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 6);
        int columna;
        do
        {
            columna = dis(gen);
        } while (!verificarJugada(tablero, columna));
        return columna;
    }

    void manejarCliente(int clienteSocket, const sockaddr_in &clienteDireccion)
    {
        std::string identificador = "cliente" + std::to_string(++contadorClientes); // incrementar el contador de clientes
        Juego nuevoJuego(clienteSocket, identificador);

        {
            std::lock_guard<std::mutex> lock(mutexJuegos);
            juegos.push_back(nuevoJuego);
        }

        std::cout << "Cliente conectado [" << inet_ntoa(clienteDireccion.sin_addr) << ":" << ntohs(clienteDireccion.sin_port) << "] - Identificador: " << identificador << std::endl;

        enviarMensajeBienvenida(clienteSocket);

        std::vector<std::vector<char>> &tablero = nuevoJuego.getTablero(); // tablero especifico de juego

        /*Ver si comienza el cliente o el servidor*/
        bool clienteComienza = std::rand() % 2 == 0;

        /*mensaje de quien comienza*/
        if (clienteComienza)
        {
            enviarMensaje(clienteSocket, "Empiezas el cliente (Tú)\n");
        }
        else
        {
            enviarMensaje(clienteSocket, "Empieza el Servidor\n");

            int columnaInicial = obtenerColumnaMaquina(tablero);
            realizarJugada(tablero, columnaInicial, maquina);
            std::cout << "Servidor jugando con  " << identificador << " juega columna " << columnaInicial + 1 << std::endl;
            enviarMensaje(clienteSocket, "Servidor juega columna " + std::to_string(columnaInicial + 1) + "\n");
        }

        bool finJuego = false;
        while (!finJuego)
        {
            /*Turno del jugador*/
            mostrarTablero(clienteSocket, tablero);
            enviarMensaje(clienteSocket, "Es tu turno.");
            char columna_buffer[1024];
            int bytesRecibidos = recv(clienteSocket, columna_buffer, sizeof(columna_buffer), 0);
            if (bytesRecibidos <= 0)
            {
                std::cerr << "El cliente " << identificador << " se desconecto o ocurrio un error." << std::endl;
                break;
            }
            columna_buffer[bytesRecibidos] = '\0';
            int columna = std::stoi(columna_buffer);
            columna--;
            if (columna < 0 || columna >= 7 || !verificarJugada(tablero, columna))
            {
                enviarMensaje(clienteSocket, "Columna invalida. Intentelo de nuevo.\n");
                continue;
            }
            realizarJugada(tablero, columna, jugador);

            std::cout << identificador << ": Cliente juega columna " << columna + 1 << std::endl;

            /*verificar si gano el jugador*/
            if (verificarGanador(tablero, jugador))
            {
                mostrarTablero(clienteSocket, tablero);
                enviarMensaje(clienteSocket, "\n¡Felicidades! ¡Has ganado!\nFin del juego.\n");

                std::cout << "El cliente " << identificador << " ha ganado la partida y se desconectó." << std::endl;

                finJuego = true;
                break;
            }

            /* verificar empate */
            if (verificarEmpate(tablero))
            {
                mostrarTablero(clienteSocket, tablero);
                enviarMensaje(clienteSocket, "\n¡El juego ha terminado en empate!\nFin del juego.\n");

                std::cout << "El cliente " << identificador << " ha empatado con el servidor y se desconectó." << std::endl;

                finJuego = true;
                break;
            }

            // Turno de la máquina
            int columnaMaquina = obtenerColumnaMaquina(tablero);
            realizarJugada(tablero, columnaMaquina, maquina);
            std::cout << "Servidor jugando con " << identificador << " juega columna " << columnaMaquina + 1 << std::endl;

            enviarMensaje(clienteSocket, "\nServidor juega columna: " + std::to_string(columnaMaquina + 1) + "\n");

            /*verificar si gano el servidor*/
            if (verificarGanador(tablero, maquina))
            {
                mostrarTablero(clienteSocket, tablero);
                enviarMensaje(clienteSocket, "\n¡La máquina ha ganado!\nFin del juego.\n");

                std::cout << "El cliente " << identificador << " ha perdido la partida y se desconecto." << std::endl;

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

        limpiarTablero(tablero);
    }

    void limpiarTablero(std::vector<std::vector<char>> &tablero)
    {
        for (int i = 0; i < 6; ++i)
        {
            for (int j = 0; j < 7; ++j)
            {
                tablero[i][j] = ' ';
            }
        }
    }

    /*funcion encargada de mostrar el tablero*/
    void mostrarTablero(int clienteSocket, const std::vector<std::vector<char>> &tablero)
    {
        std::string mensaje;

        mensaje += "-----------------\n";
        for (size_t i = 0; i < tablero.size(); ++i)
        {
            mensaje += std::to_string(i + 1);
            mensaje += '|';
            for (size_t j = 0; j < tablero[i].size(); ++j)
            {
                mensaje += tablero[i][j];
                mensaje += '|';
            }
            mensaje += '\n';
            mensaje += "----------------\n";
        }
        mensaje += "  1 2 3 4 5 6 7\n";
        enviarMensaje(clienteSocket, mensaje);
    }

    void iniciar()
    {
        /*crear socket*/
        int servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (servidorSocket == -1)
        {
            std::cerr << "Error al crear el socket del servidor" << std::endl;
            return;
        }

        /*configuracion direccion*/
        struct sockaddr_in direccion;
        direccion.sin_family = AF_INET;
        direccion.sin_addr.s_addr = INADDR_ANY;
        direccion.sin_port = htons(puerto);

        if (bind(servidorSocket, (struct sockaddr *)&direccion, sizeof(direccion)) == -1)
        {
            std::cerr << "Error al vincular el socket a la direccion y puerto" << std::endl;
            close(servidorSocket);
            return;
        }

        if (listen(servidorSocket, 10) == -1)
        {
            std::cerr << "Error al escuchar por conexiones entrantes" << std::endl;
            close(servidorSocket);
            return;
        }

        std::cout << "Esperando conexiones ... \n"
                  << std::endl;

        /*Bucle para aceptar conexiones y mantenerlas*/
        while (true)
        {
            struct sockaddr_in clienteDireccion;
            socklen_t clienteTamano = sizeof(clienteDireccion);
            int clienteSocket = accept(servidorSocket, (struct sockaddr *)&clienteDireccion, &clienteTamano);
            if (clienteSocket == -1)
            {
                std::cerr << "Error al aceptar la conexion " << std::endl;
                continue;
            }
            // creacion de hilo para manejar los clientes
            std::thread clienteThread(&Servidor::manejarCliente, this, clienteSocket, clienteDireccion);
            clienteThread.detach();
        }
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
