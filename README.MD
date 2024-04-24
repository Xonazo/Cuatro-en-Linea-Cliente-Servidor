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
