# Tarea 1 Comunicacion de datos y redes

## Integrantes
- **Edison Muñoz [@Eddikhie](https://github.com/Xonazo)**
- **Domingo Vega [@DJVegaG](https://github.com/DJVegaG)**
- **Ingenieria Civil Informatica** - **Universidad del Bio-Bio**

## Descripción
Cuatro en Lınea1 es un juego de mesa para dos jugadores distribuido por Hasbro, en el que se introducen
fichas en un tablero vertical con el objetivo de alinear cuatro fichas consecutivas de un mismo color. Fue
creado en 1974. El objetivo del juego es alinear cuatro fichas sobre un tablero formado por seis filas y siete
columnas. Cada jugador dispone de 21 fichas de un color (por lo general, rojas o amarillas). Por turnos,
los jugadores deben introducir una ficha en la columna que prefieran (siempre que no este completa) y esta
caera a la posicion mas baja. Gana la partida el primero que consiga alinear cuatro fichas consecutivas de
un mismo color en horizontal, vertical o diagonal. Si todas las columnas estan llenas pero nadie ha hecho
una fila valida, hay empate.
Implemente el Juego Cuatro en Lınea mediante el modelo Cliente/Servidor. Para ello, debe crear dos
programas de los cuales uno funcionara como el Servidor de juegos (con peticiones concurrentes) y el otro
sera el Cliente que se conectara al servidor para jugar.

## Consideraciones
* El proyecto es en parejas.
* Se debe utilizar C++ como lenguaje de programación y su implementacion de sockets.
* El uso de Makefile es obligatorio, si no se adjunta o no funciona no se podra compilar y por lo tanto no se podra evaluar.
* Para identificar las fichas de los jugadores puede utilizar las letras S para el servidor y C para el cliente.
* Tamaño del tablero debe ser de 6 filas por 7 columnas.
* El servidor debe crear por cada jugador una ejecucion independiente, la cual "controlara" el juego. El servidor debe permitir multiples conexiones y atenderlas simultaneamente.
* El servidor debe levantar su servicio en un puerto TCP que se le indicara al momento de ejecutar, por ejemplo: 
```
$ ./servidor 7777
```
Donde: 7777 es el puerto donde recibira conexiones.
* El cliente debe permitir indicar al momento de ejecutarlo la direccion IP y el puerto del servidor al cual se conectara, por ejemplo:
```
$ ./cliente 192.168.1.100 7777
```
Donde: 192.168.1.100 es la direccion IP del servidor y 7777 es el puerto al cual conectar en el servidor.
* La manera en que el cliente indique la columna queda a criterio del programador.

***

# Pasos Previos
* Tener instalado el compilador de C++

* Tener instalado Make

## Instalacion de compilador de C++
* Para instalar el compilador de C++, se debe escribir y ejecutar el siguiente comando en la terminal:
``` 
sudo apt-get install g++
```

## Instalacion de Make
* Para instalar el programa make, se debe escribir y ejecutar el siguiente comando en la terminal:
``` 
sudo apt-get install Make
```
***

# Instrucciones de uso

 **1. Clonar el repositorio**

 Para clonar el repositorio ingrese el siguiente comando en su terminal:

```
git clone https://github.com/Xonazo/Cuatro-en-Linea-Cliente-Servidor.git
```
**2. Comando make**

Para compilar los programas ubiquese en la carpeta donde esta el servidor y cliente, y escriba el siguiente comando en la terminal:

```
make
```

**3. Servidor**
Para ejecutar el Servidor en su terminal escriba el siguiente comando:

```
./Servidor <Puerto>
```
En puerto se selecciona el puerto que el usuario desee, por ejemplo podria ser el 7777.

```
./Servidor 7777
```

**4. Cliente**

Para ejecutar el cliente en su terminal escriba el siguiente comando:

```
./Cliente <ip> <Puerto>
```

En donde ip es la direccion ip del servidor, y puerto es el puerto al cual conectar en el servidor. Por ejemplo:

```
./Cliente 192.168.1.100 7777
```
**5. Juego**

Una vez aqui ya comienza el juego, en donde por lado del cliente debe ingresar las columnas para jugar y en el servidor puede observar los diversos juegos.

#### Imagen ejemplo de servidor:
![Servidor ejemplo](https://i.imgur.com/MFeNafg.png)

#### Imagen de ejemplo de Cliente:
![Texto alternativo](https://i.imgur.com/zkGR1o6.png)

***
