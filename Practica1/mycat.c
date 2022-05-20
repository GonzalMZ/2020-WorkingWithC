#include <stdio.h>		/* Header file for printf */
#include <sys/types.h>		/* Header file for system call  open */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>		/* Header file for system calls read, write y close */

#define BUFFER_SIZE 1024
 
int mycat (char *name1)	
{
	int infile;
	int nread;
        int nwrite;

	char buffer [BUFFER_SIZE];  /*creamos un array de char de tamaño BUFFER_SIZE */

	infile = open(name1, O_RDONLY);  /*abrimos el fichero name1 */
	if (infile < 0 ) {
		printf("No se puede abrir el fichero %s\n", name1);
		return(-1); /* no puede abrirse */
	}

	
	/*lectura de name1 BUFSIZE caracteres de una vez*/
	while( (nread = read(infile, buffer, BUFFER_SIZE) ) > 0 )
	{
		/*escribir el buffer al archivo de salida*/
		do {
			nwrite = write (STDOUT_FILENO, buffer, nread);
			if (nwrite == -1) {  /*si hay un error cerramos el fichero y devolvemos -1 */
				close(infile);
                                printf("Error de escritura \n");
				return (-1);
			}
			nread = nread-nwrite;  /*avanzamos en el fichero para llegar a los siguientes caracteres */
		} while (nread > 0);

	}

	if (nread == -1){
                close(infile);
                printf("Error de lectura \n");
		return (-1); 	/* error al leer */
        } 
        if ((close(infile))==-1){
             printf ("Error al cerrar el fichero \n");
             return -1;
        }      
	close(infile);    /*cerramos el fichero */
        
	return(0);
}
int main(int argc, char *argv[])
{
        int solution;

	/*si el numero de argumentos es menor de 2, significa que no se le ha pasado un argumento a la funcion y por lo tanto es un error, imprimimos -1*/
	if(argc < 2)
	{
		printf("No hay suficientes argumentos\n");
		return -1;
	}else if(argc > 2)
        {
		printf("Hay demasiados argumentos\n");
		return -1;
	}
        solution = mycat(argv[1]); /*si todo ha ido bien, devolvemos la funcion realizada sobre el argumento pasado */
	return solution;
}

