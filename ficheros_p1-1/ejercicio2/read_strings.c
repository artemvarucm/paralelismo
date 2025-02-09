#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

/** Loads a string from a file.
 *
 * fd: integer file descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(int fd)
{
	/* To be completed */
    int count = 0;
    char c;
    
    // Contar caracteres hasta encontrar '\0'
    while (read(fd, &c, sizeof(char)) == 1 && c != '\0') {
        count++;
    }

    if (count == 0 || c != '\0') {
        return NULL;
    }
    
    // Retroceder el puntero del archivo
    lseek(fd, -(count + 1), SEEK_CUR);
    
    // Reservar memoria para la cadena
    char *str = (char*)malloc(count + 1);
    
	if (str) {
		// Volvemos a leer
        read(fd, str, count + 1);
    	return str;
    }

	return NULL;	
}

int main(int argc, char *argv[])
{
	/* To be completed */
	if (argc < 2) {
		fprintf(stderr,"Usage: %s <file-name>\n",argv[0]);	
        return 1;
    }

	int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char *str;
    while ((str = loadstr(fd)) != NULL) {
        printf("%s\n", str);
        free(str);
    }
    
    close(fd);

	return 0;
}
