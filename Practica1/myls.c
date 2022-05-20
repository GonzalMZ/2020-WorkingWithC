#include <stdio.h>		/* Header file for system call printf */
#include <unistd.h>		/* Header file for system call getcwd */
#include <sys/types.h>		/* Header file for system calls opendir, readdir y closedir */
#include <dirent.h>
#include <string.h>



int myls (char *name1){

   DIR *indir;
   int nwrite;
   struct dirent *nread;
   

   indir = opendir(name1);          /*abre el directorio que recibe la funcion como argumento*/
   
   if( indir == NULL){             /*si el descriptor del directorio abierto es igual a NULL, significa que ha habido un error, por lo que devolvemos -1 */
   	printf ("ERROR: El directorio no existe o no se pudo abrir \n");
   	return -1;
   }
   while((nread = readdir(indir)) > 0) {  /*vamos leyendo los directorios que hay en el descriptor indir */

   printf("%s\n",nread -> d_name); /*imprimimos sus nombres por pantalla */

} 
  int a = closedir(indir); /*cuando hemos recorrido todo, lo cerramos */
  if (a == -1){ /*si nos devuelve -1 es que ha habido un error */
      printf ("Error al cerrar el directorio \n");
      return -1;
  }
   
  return 0;
  
}
int main(int argc, char *argv[])
{
        int solution;
        if (argc>2){  /*si hay mas de 2 argumentos el programa dará error y devolverá -1*/
		printf("Hay demasiados argumentos\n");
		return -1;
	}
       /* Si el numero de argumentos es 1, significa que no se le ha pasado el argumento a la función myls. Sin embargo, si el numero de argumentos es 2, si que se la ha pasado un argumento */
       
	else if (argc < 2){   /*como no se le ha pasado un argumento, utlizamos la funcion getcwd para encontrar el directorio actual y realizamos la funcion pasando el directorio actual como argumento */
        char buf[PATH_MAX]; 
        char *directorio= getcwd (buf, PATH_MAX);
        solution = myls(directorio);    
}       else{                         /*si se le ha pasado un argumento, realizamos la llamada a la funcion con ese argumento */
        solution = myls (argv[1]);
}
  return solution;

}

