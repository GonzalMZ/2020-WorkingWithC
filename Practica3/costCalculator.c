
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define NUM_CONSUMERS 1

//estructura que hemos creado para pasarsela como parametro a productor
struct parametro {
	int start;
	int end;
};
//declaramos los 3 condicionales
pthread_mutex_t mutex;
pthread_cond_t no_lleno;
pthread_cond_t no_vacio;

//declaramos la cola
queue* buffer;

//declaramos la variable global total que sera el resultado que devolveremos
int total=0;

//declaramos el numero maximo de operaciones que se pueden realizar
int maxOperaciones;

//declaramos el puntero para el array de productos
int** productos;

//CLASE PRODUCTOR
void* Productor(void* parametro){
	//casteamos el valor pasado como parametro a productor ya que debe ser un struct parametro*
	struct parametro * param=(struct parametro*) parametro;
	//guarddamos en un int los valores guardados en parametro para un mejor manejo de las variables
	int start=param->start;
	int end= param->end;
	 //recorremos la parte del array que le corresponde a cada productor
	for(start;start<=end;start++){
		//zona critica
		pthread_mutex_lock(&mutex);
		//si el buffer esta lleno, lo bloqueamos
		while(queue_full(buffer)==1){ 
			pthread_cond_wait(&no_lleno,&mutex);
		}
		//si no, lo insertamos
		struct element* operacion;
		operacion =(struct element*)  malloc(sizeof(struct element));
		operacion->type = productos[start][0];
		operacion->time = productos[start][1];
		queue_put(buffer,operacion);
		//si el hilo del consumidor esta bloqueado porque el buffer esta vacio, lo desbloqueamos porque ahora no lo esta
		pthread_cond_signal(&no_vacio);
		//salimos de la zona critica y liberamos el mutex
		pthread_mutex_unlock(&mutex);
	}
    pthread_exit(0);
}

//CODIGO DEL CONSUMIDOR
void* Consumidor(){
	//repetimos este proceso tantas veces como operaciones haya
	for(int i=0;i < maxOperaciones;i++){
		//acedemos al buffer(zona critica)
		pthread_mutex_lock(&mutex);
		//si el buffer esta vacio se bloquea
		while(queue_empty(buffer)==1){
			pthread_cond_wait(&no_vacio,&mutex);
		}//si no lo esta, obtenemos un elemento del buffer
		struct element* dato = queue_get(buffer);
		//indicamos que el buffer no esta lleno, por si se quiere insertar un nuevo elemento
		pthread_cond_signal(&no_lleno);
		//liberamos el mutex
		pthread_mutex_unlock(&mutex);
		//calculamos el precio de cada operacion diferenciando segun el tipo de maquina que tengan
		if(dato->type ==1)
			total = total + dato->time;
		else if(dato->type ==2)
			total = total + 3*(dato->time);
		else if(dato->type==3)
			total = total + 10*(dato->time);
		
	}
	pthread_exit(0);
}

//MAIN
int main (int argc, char ** argv) {
	//si el numero de argumentos es distinto de 4, devolvemos un mensaje de error
	if(argc != 4){  
		printf("El número de argumentos no es el correcto\n");
		return -1;
}
	int prods = atoi(argv[2]); //guardamos el numero de productores
	int bsize = atoi(argv[3]); //guardamos el tamaño del buffer
	//realizamos comprobaciones para ver que los numeros pasados como parametros son correctos
	if(prods<= 0){
		printf("El número de productores no puede ser menor que 1\n");
		return -1;
	}
	if(bsize<= 0){
		printf("El tamaño del buffer no puede ser menor que 1\n");
		return -1;
	}
	//cerramos la salida estandar para redigirla, y abrimos el archivo pasado como parametro, que se colocara en el lugar de la entrada estandar, para poder hacer de manera correcta el scanf
	close(STDIN_FILENO);
	int o= open(argv[1],O_RDONLY);
	if(o < 0){
		printf("Hay un error a la hora de redireccionar la entrada estándar\n");
		return -1;
}
	//guardamos el numero maximo de operaciones
	scanf("%d", &maxOperaciones);
	//definimos el numero de filas que tendra el array de productos, haciendo malloc con el numero de operaciones que hemos sacado en el scanf de antes 
	productos=(int**)malloc(sizeof(int*)*maxOperaciones);
	//para cada fila del array de productos definimos 2 columnas, en las que se guardara el tipo de maquina y el tiempo, y hacemos scanf en la entrada estandar redirigida para conseguir esos valors y guardarlos en el array. Si nos devuelve EOF significa que hay menos operaciones que el numero que se deberia analizar, asi que devolvemos error
	for(int i=0;i<maxOperaciones;i++){   
		productos[i]=(int*)malloc(sizeof(int)*2);
		int s =scanf("%*d %d %d\n",&productos[i][0],&productos[i][1]);
		if(s==EOF){
			printf("Hay un número menor de operaciones de las que se pretende leer\n");
			return -1;
		}
	}
	//inicializamos el buffer
	buffer = queue_init(bsize); //inicializamos el buffer

	//inicializamos el mutex de acceso al buffer compartido
	pthread_mutex_init(&mutex, NULL);  
	//inicializamos el hilo del control de llenado  
	pthread_cond_init(&no_lleno, NULL);  
	//inicializamos el hilo del control de vaciado
	pthread_cond_init(&no_vacio, NULL);  


	//si el numero de productores es mayor que el numero de operaciones devolvemos error
	if(prods>maxOperaciones){
		printf("El número de productores no puede ser mayor que el número de operaciones\n");
		return -1;
	}

	//declaramos los hilos de los productores
	pthread_t productores[prods]; 
	//declaramos e inicializamos el hilo del consumidor
	pthread_t cons;   
	//creamos el array de parametros que vamos a pasarle a los productores   
	struct parametro p[prods];    
	pthread_create(&cons, NULL,Consumidor,NULL);
	//le damos los valores a los atributos de cada parametro que vamos a pasarle a los productores, y creamos los hilos
	for(int i=0;i<prods;i++){
	//el valor start del parametro sera igual para todos
		p[i].start=i*(maxOperaciones/prods);
	//si es el ultimo, end sera el numero maximo de operaciones -1, ya que empezaremos a contar en 0
		if(i==prods-1){
			p[i].end=maxOperaciones-1;
	//si no es el ultimo, el end sera el numero anterior al start del siguiente
		}else{
			p[i].end=((i+1)*(maxOperaciones/prods))-1;
		}pthread_create(&productores[i],NULL,Productor,&p[i]);	
	}
	
	//hacemos el join de los hilos del productor
	for(int j=0;j<prods;j++){
		pthread_join(productores[j],NULL);
	}
	//hacemos el join del hilos del consumidor
	pthread_join(cons,NULL);
	
	//destruimos los condicionales
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&no_lleno);
	pthread_cond_destroy(&no_vacio);

	//destruimos el buffer para liberar la memoria
	queue_destroy(buffer);
	//imprimimos por pantalla el resultado pedido
	printf("Total: %i €.\n", total);
	return 0;
}






