/*PRIMERA PRÁCTICA EVALUABLE
	MIGUEL GONZÁLEZ HERRANZ
	CARLOS LEÓN ARJONA
*/
//Memoria compartida a partir del 300
//Definir la union
//poner el valor de la union en los semaforos
//cambiar n_pist a pist vivos en los necesarios
//señal de semaforo de ronda
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include "pist2.h"

#define TAM 1024
int pidP;
int semaforo;
int memoria_id;
char *zonaMemoria = NULL;
int buzon;

typedef struct tipoMensaje{
	long tipo;
	char muerte[10];
}msg;

union semun{
    int val;
    struct semid_ds *buf;
    u_short *array;
};

void manejadora(int sig){
//Aquí van las funciones necesarias para borrar de memoria los recursos IPC una vez hayan sido creados
	//Esto solo lo realizará el padre
	if(getpid()==pidP){
		semctl(semaforo,0,IPC_RMID);
		msgctl(buzon,IPC_RMID,0);
		shmctl(memoria_id,IPC_RMID,0);
		fflush(stdout);
		printf("\n\n\nEstoy en la manejadora...\n");
		fflush(stdout);
		exit(100);
	}
	//Esto lo harán todos los procesos para cerrarse cuando enviemos la señal
	exit(0);
}

void liberaRecursos();

int main(int argc, char *argv[]){
	//Lo primero que vamos a hacer es comprobar el numero de argumentos para evitar que salga violación de segmento
	if(argc<3||argc>4){
		perror("Error en los argumentos");
		exit(0);
	}
	//DECLARAMOS RECURSOS IPC Y VARIABLES
	char idenProc[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	char yo_char;
	int procVivos[26];
	int n_pist = strtol(argv[1],NULL,10);
	int n_vel = strtol(argv[2],NULL,10);
	int semilla;
	int ret;
	int i,j,k,n;
	char victima;
	int menor=0,victima_int;
	int yo;
	int n_pistVivos=n_pist;
	//Obtenemos la semilla si es NULL
	if(argv[3]==NULL){
		semilla=0;
	}else{
		semilla = strtol(argv[3],NULL,10);
	}
	//Obtenemos el pid del proceso padre
	pidP = getpid();
	
	if(n_pist<2 || n_pist>26){
		perror("Error en los argumentos");
		exit(100);
	}
	
	if(n_vel<0){
		perror("Error en los argumentos");
		exit(100);
	}
	//A continuación comprobamos la creación de los recursos IPCs necesarios
	//SEMAFOROS
	union semun semunion;
	semunion.val=0;
	semaforo = semget(IPC_PRIVATE,9,IPC_CREAT | 0777);
	if(semaforo==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,1,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,2,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,3,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,4,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,5,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,6,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,7,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}

	if(semctl(semaforo,8,SETVAL,semunion)==-1){
		perror("Error en la creacion de semáforo");
		exit(100);
	}


	struct sembuf signalPist;
	struct sembuf waitPist;
	struct sembuf signalMorir;
	struct sembuf waitMorir;
	struct sembuf signalDisparar;
	struct sembuf waitDisparar;
	struct sembuf signalPDisparo;
	struct sembuf waitPDisparo;
	struct sembuf signalPadre;
	struct sembuf waitPadre;
	struct sembuf signalRonda;
	struct sembuf waitRonda;
	struct sembuf signalMuertes;
	struct sembuf waitMuertes;
	struct sembuf signalCoordinador;
	struct sembuf waitCoordinador;

	waitPist.sem_num = 1;
	waitPist.sem_op = -1;
	waitPist.sem_flg = 0;

	signalPist.sem_num = 1;
	signalPist.sem_flg = 0;

	signalMorir.sem_num = 3;
	signalMorir.sem_flg = 0;

	waitMorir.sem_num = 3;
	waitMorir.sem_op = -1;
	waitMorir.sem_flg = 0;

	signalDisparar.sem_num = 2;
	signalDisparar.sem_flg = 0;

	waitDisparar.sem_num = 2;
	waitDisparar.sem_op = -1;
	waitDisparar.sem_flg = 0;

	signalPDisparo.sem_num = 4;
	signalPDisparo.sem_op = 1;
	signalPDisparo.sem_flg = 0;

	waitPDisparo.sem_num = 4;
	waitPDisparo.sem_flg = 0;

	signalPadre.sem_num = 5;
	signalPadre.sem_op = 1;
	signalPadre.sem_flg = 0;

	waitPadre.sem_num = 5;
	waitPadre.sem_op = -1;
	waitPadre.sem_flg = 0;

	signalRonda.sem_num = 6;
	signalRonda.sem_op = 1;
	signalRonda.sem_flg = 0;

	waitRonda.sem_num = 6;
	waitRonda.sem_flg = 0;

	signalMuertes.sem_num = 7;
	signalMuertes.sem_op = 1;
	signalMuertes.sem_flg = 0;

	waitMuertes.sem_num = 7;
	waitMuertes.sem_flg = 0;

	signalCoordinador.sem_num = 8;
	signalCoordinador.sem_flg = 0;

	waitCoordinador.sem_num = 8;
	waitCoordinador.sem_op = -1;
	waitCoordinador.sem_flg = 0;
	//SEGMENTO DE MEMORIA
	memoria_id = shmget(IPC_PRIVATE, TAM, IPC_CREAT | 0664);
	
	if(memoria_id==-1){
		perror("Error en la reserva de memoria");
		semctl(semaforo,0,IPC_RMID);
		exit(100);
	}
	
	zonaMemoria = shmat(memoria_id, 0,0);
	char *punteroMemoria = (zonaMemoria + 300);

	if(zonaMemoria == NULL){
		perror("Error en la reserva de memoria");
		semctl(semaforo,0,IPC_RMID);
		exit(100);
	}
	//ahora debemos crear un vector en memoria compartida para saber si los pistoleros están vivos, si es 1, estará vivo, si no estará muerto
	for(i=0;i<n_pist;i++){
		*(punteroMemoria+i)=1;
	}

	*(punteroMemoria+n_pist)=n_pistVivos;
	*(punteroMemoria+n_pist+1)=menor;
	//BUZON
	msg mensaje;
	buzon = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
	
	if(buzon==-1){
		perror("Error en la creación del buzon");
		liberaRecursos();
		exit(100);
	}
	//Modificamos la señal SIGINT para que se encargue de eliminar los recursos IPC
	signal(SIGINT,manejadora);
	//Iniciamos el programa
	if(PIST_inicio(n_pist,n_vel,semaforo,zonaMemoria,semilla)==-1){
		kill(pidP,SIGINT);
		perror("Error en el inicio del programa");
		liberaRecursos();
		exit(100);
	}
	for(i=0;i<n_pist;i++){
		switch(fork()){
			case -1:
				perror("Error en la creacion de procesos");
				liberaRecursos();
				exit(100);
			case 0:
				if(PIST_nuevoPistolero(idenProc[i])==-1){
					perror("Error en la funcion nuevoPistolero");
					liberaRecursos();
					exit(100);
				}
				yo=i;
				yo_char=idenProc[i];
				while(1){
					//Lo primero que debemos hacer es comprobar que es el menor
						if(yo==*(punteroMemoria+n_pist+1)){
							//Enviamos una señal para que todos los procesos despierten y elijan la víctima
							//Asignamos el numero de pistoleros -1 a la operacion de semaforo
							signalPist.sem_op = (*(punteroMemoria+n_pist)-1);
							if(semop(semaforo,&signalPist,1)==-1){
								perror("Error en signalPist");
								liberaRecursos();
								exit(100);
							}
							
						}else{
							//wait para empezar la ronda
							if(semop(semaforo,&waitPist,1)==-1){
								perror("Error en waitPist");
								liberaRecursos();
								exit(100);
							}
						}
						//Coordinacion de disparos
						if(yo==*(punteroMemoria+n_pist+1)){
							signalDisparar.sem_op = (*(punteroMemoria+n_pist)-1);
							if(semop(semaforo,&signalDisparar,1)==-1){
								perror("Error en signalDisparar");
								liberaRecursos();
								exit(100);
							}
							victima = PIST_vIctima();
							if(victima=='@'){
								perror("Error en la selección de victimas");
								liberaRecursos();
								exit(100);
							}
							//Enviamos el mensaje a la víctima y disparamos
							PIST_disparar(victima);
							sprintf(mensaje.muerte,"%s","MUERTE");
							mensaje.tipo=victima;
							if(msgsnd(buzon,&mensaje,sizeof(msg) - sizeof(long),0)==-1){
								perror("Error en el envio de mensajes");
								liberaRecursos();
								exit(100);
							}
						}else{
							if(semop(semaforo,&waitDisparar,1)==-1){
								perror("Error en waitDisparar");
								liberaRecursos();
								exit(100);
							}							
							victima = PIST_vIctima();
							if(victima=='@'){
								perror("Error en la selección de victimas");
								liberaRecursos();
								exit(100);
							}
							//Enviamos el mensaje a la víctima seleccionada y disparamos
							PIST_disparar(victima);
							sprintf(mensaje.muerte,"%s","MUERTE");
							mensaje.tipo=victima;
							if(msgsnd(buzon,&mensaje,sizeof(msg) - sizeof(long),0)==-1){
								perror("Error en el envio de mensajes");
								liberaRecursos();
								exit(100);
							}
						}
						//Cada vez que dispara un pistolero hacen un signal sobre el semaforo que coordina las muertes
						if(semop(semaforo,&signalPDisparo,1)==-1){
								perror("Error en signalPDisparo");
								liberaRecursos();
								exit(100);
						}
						//Coordinación de muertes
						if(yo==*(punteroMemoria+n_pist+1)){
							//hacemos un wait para que hasta que no disparen todos los pistoleros, no se empiecen a morir
							waitPDisparo.sem_op = -(*(punteroMemoria+n_pist));
							if(semop(semaforo,&waitPDisparo,1)==-1){
								perror("Error en waitPDisparo");
								liberaRecursos();
								exit(100);
							}
							//Asignamos la operación de semaforo el numero de pistoleros vivos -1
							signalMorir.sem_op = (*(punteroMemoria+n_pist)-1);
							//Envia la señal para que todos se puedan empezar a morir
							if(semop(semaforo,&signalMorir,1)==-1){
								perror("Error en signalMorir");
								liberaRecursos();
								exit(100);
							}
							if(msgrcv(buzon,&mensaje,sizeof(msg)-sizeof(long),idenProc[i],IPC_NOWAIT)!=-1){
								//if recibe mensaje
								*(punteroMemoria+i)=0;
								if(PIST_morirme()==-1){
									perror("Error en la muerte de procesos");
									liberaRecursos();
									exit(100);
								}
								//Cada vez que se gestiona la muerte, manda una señal
								if(semop(semaforo,&signalMuertes,1)==-1){
									perror("Error en signalMuertes");
									liberaRecursos();
									exit(100);
								}
								//exit(0);
							}else{
								//Cada vez que se gestiona la muerte, manda una señal
								if(semop(semaforo,&signalMuertes,1)==-1){
									perror("Error en signalMuertes");
									liberaRecursos();
									exit(100);
								}
							}
						}else{
							//Cuando mande la señal el coordinador, se pueden empezar a morir 
							if(semop(semaforo,&waitMorir,1)==-1){
									perror("Error en waitMorir");
									liberaRecursos();
									exit(100);
							}
							if(msgrcv(buzon,&mensaje,sizeof(msg)-sizeof(long),idenProc[i],IPC_NOWAIT)!=-1){
								//if recibe mensaje
								*(punteroMemoria+i)=0;
								if(PIST_morirme()==-1){
									perror("Error en la muerte de procesos");
									liberaRecursos();
									exit(100);
								}
								//Cada vez que se gestiona la muerte, manda una señal
								if(semop(semaforo,&signalMuertes,1)==-1){
									perror("Error en signalMuertes");
									liberaRecursos();
									exit(100);
								}
								exit(0);
							}else{
								//Cada vez que se gestiona la muerte, manda una señal
								if(semop(semaforo,&signalMuertes,1)==-1){
									perror("Error en signalMuertes");
									liberaRecursos();
									exit(100);
								}
							}
						}
						if(yo==*(punteroMemoria+n_pist+1)){
							//Espera a que gestionen las muertes todos los procesos
							waitMuertes.sem_op=-(*(punteroMemoria+n_pist));
							if(semop(semaforo,&waitMuertes,1)==-1){
									perror("Error en waitMuertes");
									liberaRecursos();
									exit(100);
							}
							//Selecciona el menor
							k=0;
							while(*(punteroMemoria+k)==0){
								k=k+1;
							}
							menor=k;
							//Selecciona el número de pistoleros vivos
							n_pistVivos=0;
							for(n=0;n<n_pist;n++){
								if(*(punteroMemoria+n)==1){
									n_pistVivos=n_pistVivos+1;
								}
							}
							//Guarda el número de pistoleros vivos en memoria compartida
							*(punteroMemoria+n_pist)=n_pistVivos;
							//Si no queda ningún proceso vivo, se suicida
							if(n_pistVivos==0){
								if(semop(semaforo,&signalPadre,1)==-1){
									perror("Error en signalPadre");
									liberaRecursos();
									exit(100);
								}
								exit(0);
							}
							if(n_pistVivos<=1){
								*(punteroMemoria+n_pist+1)=menor;
								if(*(punteroMemoria+yo)==0){
									//Si quedan vivos 1 o menos pistoleros y él ha recibido el mensaje de muerte, da la señal de que ha acabado de gestionar la ronda y se suicida
									signalCoordinador.sem_op=*(punteroMemoria+n_pist);
									if(semop(semaforo,&signalCoordinador,1)==-1){
										perror("Error en signalCoordinador");
										liberaRecursos();
										exit(100);
									}
									exit(0);
								}
								if(*(punteroMemoria+yo)==1){
									//Si quedan vivos 1 o menos pistoleros y él es el último pistolero, llama al padre y se suicida
									if(semop(semaforo,&signalPadre,1)==-1){
										perror("Error en signalPadre");
										liberaRecursos();
										exit(100);
									}
									exit(0);
								}
							}else{
								//Se guarda el menor(coordinador de la ronda) en memoria compartida
								*(punteroMemoria+n_pist+1)=menor;
								signalCoordinador.sem_op=*(punteroMemoria+n_pist);
								//Manda la señal de que ya ha terminado de gestionar todo
								if(semop(semaforo,&signalCoordinador,1)==-1){
									perror("Error en signalCoordinador");
									liberaRecursos();
									exit(100);
								}
								//Si el coordinador tiene un 0 en su posición, se suicida
								if(*(punteroMemoria+yo)==0){
									exit(0);
								}
							}
						}else{
							//Esperan todos a que el coordinador termine de gestionar la ronda
							if(semop(semaforo,&waitCoordinador,1)==-1){
									perror("Error en waitCoordinador");
									liberaRecursos();
									exit(100);
							}
							//Si sólo queda él, se suicida y llama al padre
							if(*(punteroMemoria+n_pist)==1){
								if(semop(semaforo,&signalPadre,1)==-1){
									perror("Error en signalPadre");
									liberaRecursos();
									exit(100);
								}
								exit(0);
							}

						}
						if(semop(semaforo,&signalRonda,1)==-1){
									perror("Error en signalRonda");
									liberaRecursos();
									exit(100);
						}


						//Esperamos a que se gestionen las rondas
						waitRonda.sem_op=*(punteroMemoria+n_pist);
						if(semop(semaforo,&waitRonda,1)==-1){
									perror("Error en waitRonda");
									liberaRecursos();
									exit(100);
						}
				}
		}
	}
	//Al finalizar el programa, volvemos a enviar la señal que elimina los recursos IPC
	//wait para que salga el padre cuando hayan acabado los procesos
	if(semop(semaforo,&waitPadre,1)==-1){
		perror("Error en waitPadre");
		kill(pidP,SIGINT);
	}
	menor=*(punteroMemoria+n_pist+1);
	PIST_fin();
	liberaRecursos();
	//El padre devolverá el indice del proceso menor
	return menor;
}

void liberaRecursos(){
	semctl(semaforo,0,IPC_RMID);
	msgctl(buzon,IPC_RMID,0);
	shmctl(memoria_id,IPC_RMID,0);
}
