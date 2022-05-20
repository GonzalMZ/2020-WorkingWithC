 #include <stdio.h>	/*Header file for system call printf*/
#include <sys/types.h>	/*Header file for system call fstat*/
#include <sys/stat.h>
#include <fcntl.h>	/*Header file for system call fcntl*/
#include <dirent.h> 	/*Header file for system call opendir, closedir,readdir...*/  
#include <unistd.h>


int mysize() {

   DIR *indir;
   struct dirent *nread;
   
   char buf[PATH_MAX]; 
   char *directorio= getcwd (buf, PATH_MAX);
   indir = opendir(directorio);          /*abre el directorio actual */
   
   if( indir == NULL){             /*si el descriptor del directorio abierto es igual a NULL, significa que ha habido un error, por lo que devolvemos -1 */
   	printf ("ERROR: El directorio no existe o no se pudo abrir \n");
   	return -1;
   }
  
   while((nread = readdir(indir)) > 0) {  /*vamos leyendo los directorios que hay en el descriptor indir */
       if(nread->d_type == DT_REG){
       int fd= open(nread->d_name,O_RDONLY);        /*abrimos el fichero para conseguir cada fichero*/
       if(fd == -1){
       printf("Error al abrir el fichero\n");
       return -1;
       }
       int size=lseek(fd,0, SEEK_END); /*utilizamos la funcion lseek para ver el tamaño */
       if( size == -1){
          printf("Error de posicionamiento en la función lseek\n");
          return -1;
       }
       printf("%s",nread -> d_name); /*imprimimos sus nombres por pantalla */
       printf("\t%i\n",size);         /*imprimimos un espacio y el tamaño */
       }  

   } 
   int a= closedir(indir);   /*cuando hemos recorrido todo, lo cerramos */
   if (a == -1){
       printf( "Error al cerrar el directorio\n");
       return -1;
   }
   return 0;
}
int main(int argc, char *argv[])
{      if(argc==1){
        int solution;   
        solution = mysize();  /* devolvemos la funcion*/
        return solution;
       }
       else{ 
            printf("Error, hay demasiados argumentos \n");
            return -1;
}
}
