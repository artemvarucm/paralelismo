#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


// Lee la siguiente linea del archivo (se fija en \n)
char *loadline(int fd)
{
    int count = 0;
    char c;
    
    // Contar caracteres hasta encontrar '\n'
    while (read(fd, &c, sizeof(char)) == 1 && c != '\n') {
        count++;
    }

    if (count == 0) {
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


pid_t launch_command(char** argv){
    /* To be completed */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Proceso hijo ejecuta el comando
        execvp(argv[0], argv);
        perror("execvp"); // Solo se ejecuta si execvp falla
        exit(EXIT_FAILURE);
    }

    return pid;
}



char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}


int main(int argc, char *argv[]) {
    int opt, arg_count;
    int fd;

    while((opt = getopt(argc, argv, "x:s:")) != -1) {
        switch(opt) {
        case 'x':
            launch_command(parse_command(optarg, &arg_count));
            break;
        case 's':
            fd = open(optarg, O_RDONLY);
            if (fd < 0) {
                perror("Error al abrir el archivo");
                return 1;
            }

            char *line;
            int i = 0;
            while ((line = loadline(fd)) != NULL) {
                int argc;
                char **argv = parse_command(line, &argc);
                if (argc > 0) {
                    printf("@@ Running command #%d: %s", i, line);
                    pid_t childPID = launch_command(argv);
                    int status;
                    waitpid(childPID, &status, 0);
                    printf("@@ Command #%d terminated (pid: %d, status: %d)\n\n", i, childPID, status);
                    i++;
                }
                for (int i = 0; i < argc; i++) {
                    free(argv[i]);
                }
                free(argv);
            }
            close(fd);
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}