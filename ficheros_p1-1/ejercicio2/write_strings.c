#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	/* To be completed */
	if (argc < 3) {
        fprintf(stderr, "Uso: %s <archivo> <cadena1> <cadena2> ...\n", argv[0]);
        return 1;
    }
	
    // O_WRONLY solo escritura
    // O_CREAT crear si no existe
    // O_TRUNC sobreescribir si existe
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error al abrir el archivo");
        return 1;
    }
    
    for (int i = 2; i < argc; i++) {
        if (write(fd, argv[i], strlen(argv[i]) + 1) < strlen(argv[i]) + 1) {
            perror("Error al escribir en el archivo");
            close(fd);
            return 1;
        }
    }
    
    close(fd);
	return 0;
}