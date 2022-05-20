//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8


// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Saliendo del MSH **** \n");
	//signal(SIGINT, siginthandler);
        exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
    //reset first
    for(int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for ( i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
    /**** Do not delete this code.****/
    int end = 0; 
    int executed_cmd_lines = -1;
    char *cmd_line = NULL;
    char *cmd_lines[10];

    if (!isatty(STDIN_FILENO)) {
        cmd_line = (char*)malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF){
            if(strlen(cmd_line) <= 0) return 0;
            cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush (stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ***argvv = NULL;
    int num_commands;


	while (1) 
	{
		int status = 0;
	    int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

		// Get command
                //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
                executed_cmd_lines++;
                if( end != 0 && executed_cmd_lines < end) {
                    command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
                }else if( end != 0 && executed_cmd_lines == end)
                    return 0;
                else
                    command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
                //************************************************************************************************


              /************************ STUDENTS CODE ********************************/
	      if (command_counter > 0) {
                if (command_counter > MAX_COMMANDS)
                      printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
                else {
		    if(command_counter==1){       /*solo hay un comando*/
                        getCompleteCommand (argvv,0); /*conseguimos el comando a ejecutar*/
                        if(strcmp(argv_execvp[0], "mycalc") == 0){
                         /*si el comando es mycalc, se ejecutara este if*/
                           if(strcmp(argv_execvp[2], "add") == 0 && strcmp(argv_execvp[3], "0") !=0){
                         /*si el segundo termino de la operacion es add y el 
                           tercero existe, ejecutamos la operacion suma*/
                              setenv("Acc","0",0);
                         /*inicializamos la variable de entorno Acc, si ya esta 
                           creada no se sobreescribira ya que tenemos puesto 
                            que overwrite=0*/
                              char *acumulador= getenv("Acc");
                         /*conseguimos el valor de la variable Acc*/
                              int acc = atoi(acumulador);         
                         /*la pasamos a int*/                      
                              int a = atoi(argv_execvp[1]);
                              int b = atoi(argv_execvp[3]);
                              int c = a + b;
                              acc = a + b + acc;
                         /*realizamos las pertinentes operaciones*/
                           
                              fprintf(stderr,"[OK] %d + %d = %d; Acc %d \n",a,b,c,acc); 
                        /*imprimimos or la salida error la operacion, junto con 
                          el valor Acc*/
                              sprintf(acumulador, "%d",acc);
                              setenv("Acc",acumulador,1);
                        /*pasamos el nuevo valor de Acc a char otra vez y 
                         actualizamos su valor(overwrite=1 en este caso)*/
                              

                           }else if(strcmp(argv_execvp[2], "mod") == 0 && strcmp(argv_execvp[3], "0") !=0){
                       /*si el segundo valor del comando es mod y el tercero no
                         es nulo, ejecutamos la operacion*/
                             int a = atoi(argv_execvp[1]);
                             int b = atoi(argv_execvp[3]);
                             int c = a / b;
                             int d = a % b;
                       /*realizamos las pertinentes operaciones*/
                             fprintf(stderr,"[OK] %d %% %d = %d * %d + %d \n", a,b,b,c,d);
                       /*imprimimos por la salida de error la operacion pedida*/
                          }else{
                             printf("[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2> \n");
                       /*si no se cumple ninguno de lo anterior, significa que  
                         el comando esta mal escirto y por lo tanto enviamos un 
                         mensaje de error por la salida estandar*/
                           }
                       }else if(strcmp(argv_execvp[0], "mycp") == 0){
                       /*si el mandato escrito es mycp, se ejecutara este else 
                         if*/
                           if(argv_execvp[1] != NULL && argv_execvp[2] != NULL){
                       /*si este if se cumple, significa que la estructura del 
                         comando esta bien escrita, y por lo tanto lo 
                         ejecutamos*/
                              int origen = open(argv_execvp[1], O_RDONLY);
                       /*abrimos el fichero que se encuentra en argv_execvp[1], 
                         en el modo solo lectura, ya que no necesitamos mas 
                         permisos*/
                              if(origen < 0){
                                 printf("[ERROR] Error al abrir el archivo origen \n");              /*si al abrir el fichero nos devuelve u numero negativo 
                         significa que ha habido un error al abrirlo, por lo 
                         que devolvemos 0 y un mensaje de error*/
                                             
                              }else{
                      /*si no ha habido error al abrir el fichero origen, 
                        continuamos*/
                                  int destino = open (argv_execvp[2], O_CREAT | O_WRONLY | O_TRUNC,0666);
                      /*abrimos el que seria el destino, con permisos de 
                        escritura en este caso ya que sera donde tengamos que 
                        copiar el archivo, y lo truncamos con O_TRUNC. Si no  
                        existe lo creamos con O_CREAT*/
                                  if(destino < 0){
                      /*si nos da error al abrir el destino,cerramos el archivo 
                        origen, y devolvemos un mensaje de error y 0*/
                                    printf("[ERROR] Error al abrir el archivo destino \n");                          
                                    close(origen); 
                                  }else{
                      /*si hemos llegado hasta este else significa que los dos 
                        archivos se han abierto de manera correcta*/    
                                     int nread;
                                     int nwrite;
                                     char buffer [1024];
                                     while((nread = read(origen, buffer, 1024)) > 0){                                   
                     /*vamosc leyendo el origen y guardandolo en buffer*/ 
                                       nwrite = write (destino, buffer, nread);
                     /*escribimos en el archivo destino lo que se ha guardado 
                       en buffer*/
                                       if(nread!=nwrite)
                                          return 0; 
                     /*si ya hemos terminado de copiar, devolvemos 0 para 
                       terminar el proceso*/
                                     }
                                     printf("[OK] Copiado con exito el fichero %s a %s \n",argv_execvp[1],argv_execvp[2]);
                     /*devolvemos un mensaje que indica que el fichero se ha 
                       copiado correctamente*/
                                     close(origen);
                                     close(destino);  
                    /*cerramos los ficheros abiertos*/
                                }        
                             }
                           }else
                    /*si no se ha ejecutado el if de arriba significa que la 
                      estructura del comando no es correcta, por lo que 
                      imprimimos este mensaje por la salida estandar para 
                      indicar que el formato no es valido*/
                              printf("[ERROR] La estructura del comando es mycp <fichero origen> <fichero destino> \n");
                           
                       }else{ 
                        /*si el mandato no es ni mycp ni mycalc, se ejecutara 
                          este else*/
            	        int pid;
		        pid = fork();     /*hacemos un fork*/   
		        switch (pid){
		            case -1:      /*ERROR*/
                                perror ("Error en el fork");
			        return (-1);
		            case 0:                              /* hijo */
                                 
                                if (strcmp(filev[0],"0") != 0){
                                /*hay redireccion de entrada*/
                                /*cerramos la entrada estandar*/
                                    close(STDIN_FILENO);
                                /*abrimos el fichero almacenado en filev[0] y 
                                  lo utilizamos como entrada estandar*/
                                    open(filev[0], O_RDONLY);
                                }if (strcmp(filev[1],"0") != 0){ 
                                /*hay redireccion de salida*/
                                /*realizamos el mismo proceso ue antes pero 
                                  cerrando la salida estandar en este caso*/
                                    close(STDOUT_FILENO);
                                    open(filev[1], O_CREAT | O_WRONLY, 0666); 
                                }if (strcmp(filev[2], "0") != 0){ 
                                 /*hay redireccion de error*/ 
                                 /*realizamos el mismo proceso ue antes pero 
                                  cerrando la salida de error en este caso*/
                                    close(STDERR_FILENO);
                                    open (filev[2], O_CREAT | O_WRONLY, 0666);
                                }
                                /*ejecutamos el comando*/      
			        execvp (argv_execvp[0],argv_execvp);
                                /*si ha habido algun error en la ejecucion del 
                                  comando, se enviara un mensaje de error*/ 
			        perror("Error en el exec");
			        break;

		            default:  /*estamos en el padre*/
                                if(in_background == 0) 
                                /*no se ejecuta en background*/
			           while(pid != wait(&status));
                                else                  
                               /*si se ejecuta en background imprimimos el 
                                 pipe*/
                                   printf("%d/n",pid);
                            }
                        }
                   }else{                /* para n comandos*/
                        int fd1[2];     /*creamos dos tuberias*/
                        int fd2[2];
                        for( int i=0; i < command_counter; i++){   
                           /*repetiremos cada ciclo como numeros de comandos 
                             haya*/
                           if(pipe(fd2) < 0){
                           /*solo inicializamos 1 pipe, y comprobamos que el 
                             valor que nos da es correcto*/
                              perror("Error en el pipe");
                              return(-1);
                           }
                           int pid = fork();  /*hacemos un fork*/
                           switch (pid){
		  	      case -1:        /*ERROR*/
                                  perror ("Error en el fork");
			          return (-1);
		              case 0:         /* hijo */
                                  if(i==0){   /*el primer comando*/
                                  if (strcmp(filev[0],"0") != 0){
                                /*hay redireccion de entrada*/
                                 /*cerramos la entrada estandar*/
                                    close(STDIN_FILENO);
                                  /*abrimos el fichero almacenado en filev[0] y 
                                  lo utilizamos como entrada estandar*/
                                    open(filev[0], O_RDONLY);
                                  }if (strcmp(filev[2], "0") != 0){ 
                                 /*hay redireccion de error*/ 
                                 /*realizamos el mismo proceso que antes pero 
                                  cerrando la salida de error en este caso*/
                                    close(STDERR_FILENO);
                                    open (filev[2], O_CREAT | O_WRONLY, 0666);
                                  }
                                  close(STDOUT_FILENO);  
                                  /*cerramos salida estandar*/
                                  dup(fd2[1]); 
                                  /*redireccionamos la salida estandar*/
                                  close(fd2[1]); 
                                  close(fd2[1]); 
                                  /*cerramos para que el pipe no este abierto
                                  (2 veces ya que habiamos realizado un dup*/ 
                                  close(fd2[0]); 
                                  /*cerramos la entrada del pipe, ya que no la 
                                  vamos a utilizar mas */
                                 }else if(i!=0 && i!= command_counter-1){
                                  if (strcmp(filev[2], "0") != 0){ 
                                  /*hay redireccion de error*/ 
                                      close(STDERR_FILENO);
                                      open (filev[2], O_CREAT | O_WRONLY, 0666);
                                  }
                                  close(STDIN_FILENO);
                                  /*cerramos la entrada estandar*/
                                  dup(fd1[0]);
                                  /*duplicamos la entrada del pipe, que se 
                                    guardara en el descriptor de la salida 
                                    estandar*/
                                  close(fd1[0]);
                                  close(fd1[0]); 
                                  /*cerramos los descriptores que no vamos a  
                                    utilizar, por partida doble ya que habiamos 
                                    realizado un dup*/ 
                                  close(STDOUT_FILENO);
                                  /*cerramos la salida estandar*/
                                  dup(fd2[1]);
                                  /*duplicamos la salida del pipe, y por lo 
                                    tanto la redireccionamos la salida 
                                    estandar*/
                                  close(fd2[1]);
                                  close(fd2[1]);
                                  /*cerramos la salida del pipe 2 veces*/
                                  close(fd2[0]);
                                  /*cerramos la entrada por si ha habido algun 
                                    error*/
                                  
                                 }else{   /*ultimo mandato*/
                                  if (strcmp(filev[1],"0") != 0){ 
                                    /*hay redireccion de salida*/
                                    close(STDOUT_FILENO);
                                    open(filev[1], O_CREAT | O_WRONLY, 0666);
                                  }
                                  if (strcmp(filev[2], "0") != 0){ 
                                    /*hay redireccion de error*/ 
                                      close(STDERR_FILENO);
                                      open (filev[2], O_CREAT | O_WRONLY, 0666);
                                  }
                                  close(STDIN_FILENO);
                                  /*cerramos la entrada estandar*/
                                  dup(fd1[0]);
                                  /*duplicamos para redirigir la entrada del 
                                  pipe*/
                                  close(fd1[0]);
                                  close(fd1[0]);
                                  /*cerramos la entrada del pipe*/
                                  }
                                  getCompleteCommand (argvv,i); 
                                  /*conseguimos el comando a ejecutar*/
			          execvp (argv_execvp[0],argv_execvp); 
                                  /*lo ejecutamos*/
			          perror("Error en el exec");
			          break;
		              default: /*estamos en el padre*/
                                  if(in_background == 0) 
                                  /*no se ejecuta en background*/
			             while(pid != wait(&status));
                                  else                  
                                 /*si se ejecuta en background imprimimos el 
                                   pipe*/
                                     printf("%d/n",pid);
                              }
                              close(fd2[1]); 
                              /*cerramos siempre la salida del pipe para que no 
                                se quede nada abierto y no pueda haber errores*/
                              fd1[0] = fd2[0];
                              fd1[1] = fd2[1];
                              /*asignamos la entrada y salida de una tuberia a 
                                la otra*/
                           }
                       }
                }
                }
              }
        
	return 0;
}
