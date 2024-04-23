#Makefile de compilacion para ambos archivos

CC := g++
CFLAGS := -std=c++11 -Wall -Wextra -pthread

SOURCE_SERVER := servidor.cpp
EXECUTABLE_SERVER := servidor

SOURCE_CLIENT := cliente.cpp
EXECUTABLE_CLIENT := cliente

all: $(EXECUTABLE_SERVER) $(EXECUTABLE_CLIENT)

$(EXECUTABLE_SERVER): $(SOURCE_SERVER)
	$(CC) $(CFLAGS) $(SOURCE_SERVER) -o $(EXECUTABLE_SERVER)

$(EXECUTABLE_CLIENT): $(SOURCE_CLIENT)
	$(CC) $(CFLAGS) $(SOURCE_CLIENT) -o $(EXECUTABLE_CLIENT)

clean:
	rm -f $(EXECUTABLE_SERVER) $(EXECUTABLE_CLIENT)