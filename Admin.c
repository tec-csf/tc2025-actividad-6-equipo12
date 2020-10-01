 /*
* Green
! Red
? blue
TODO: ORANGE xD 


TODO: Nos faltan las señales
! Descarga una extensión que se llama color comments para que veas bonitos los comentarios xD
!Semaforos
*Primero que nada Crear un sevidor y cuatro clientes
? Atributos de el semaforo
* Verde 30 segundos, después le cede al siguiente semaforo al de la derecha.
* El semaforo enviara una señal SIGRUS1 para prender al otro semaforo.
* Si el proceso no recibe la señal se queda en rojo
* Cada que cambia de estado le manda su estado al administrador
? Si el Semaforo recibe un mensaje de el admin el mismo semaforo debe de almacenar su estado
? Los semaforos solo pueden responder a la señal SIGUSR1 que le envía otro semaforo directamente
! Administración
* La administración es el server y los 4 semaforos clientes
* El admin recibe y muestra el estado actual de cada semaforo siempre! hasta cuando ella lo manda
? La consola mandara un mensaje para que los semaforos esten intermitentes de 00:00 a 6:00 am
? Ctrl+Z envía el mensaje de ponerse en rojo a todos los semaforos, no importa que hagan
? Ctrl+C envía el mensaje de intermitente a todos los semaforos, no importa que hagan
? La consola solo responde a las señales Ctrl+C y Ctrl+Z
TODO- Para regresar los semaforos al estado en el que se encontraban o al inicio de los tiempos necesito
TODO- Volver a enviar el mismo mensaje
*/
/*
!Primero hacer el server y luego el cliente!
!Luego hacer la recepción de señales de cada estado, ctrl+c y ctrl+z
*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define TCP_PORT 8000

//* Tipos de funciones

void gestor(int);
void estadoActual(int);

//* Variables Globales
int semaforo;  
int cliente[4];
//!---------------Main----------------
int main(int argc, const char * argv[]){
    sigset_t lasDos;
    
    struct sockaddr_in direccion;
    
    int pidReceived;
    int servidor;
    
    ssize_t leidos;
    socklen_t escritos;
    int continuar = 1;
    pid_t pid;
//?-------------------------------------------------------------------------------
//?-------------------------Control de señales Para el padre----------------------
//?-------------------------------------------------------------------------------
    sigemptyset(&lasDos);
    sigaddset(&lasDos, SIGINT);
    sigaddset(&lasDos, SIGTSTP);
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR){
        printf("ERROR: No se pudo llamar al manejador\n");
    }
    else if (signal(SIGINT, SIG_IGN) == SIG_ERR){
        printf("ERROR: No se pudo llamar al manejador\n");
    } 
//?-------------------------------------------------------------------------------
//?---------------------------FIN CONTROL DE SEÑALES------------------------------
//?-------------------------------------------------------------------------------
//!-------------------------------------------------------------------------------
//!---------------------------Scockets, control de SOCKETS------------------------  
//!-------------------------------------------------------------------------------  
    
    if (argc != 2) {
        printf("Use: %s IP_Servidor \n", argv[0]);
        exit(-1);
    }
    
     
    servidor = socket(PF_INET, SOCK_STREAM, 0);
     
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    bind(servidor, (struct sockaddr *) &direccion, sizeof(direccion));
    
     
    listen(servidor, 4);
    
    escritos = sizeof(direccion);
    


    int * semaforos;
    semaforos=(int*)malloc(4*sizeof(int));
    int * pidSemaforos;
    pidSemaforos=(int*)malloc(4*sizeof(int));
    int * s=semaforos;
    int * i;
    int count=0;

    for (i = s; i<s+4; i++, count++) {

        cliente[count] = accept(servidor, (struct sockaddr *) &direccion, &escritos);
        
        printf("Aceptando conexiones en %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));
        
        pid = fork();
        
        if (pid == 0) {
//?-------------------------------------------------------------------------------
//?-------------------------Control de señales Para los Hijos---------------------
//?-------------------------------------------------------------------------------
            if (signal(SIGTSTP, gestor) == SIG_ERR){
            printf("ERROR: No se pudo llamar al manejador\n");
            }
            else if (signal(SIGINT, gestor) == SIG_ERR){
                printf("ERROR: No se pudo llamar al manejador\n");
            }
//?-------------------------------------------------------------------------------
//?---------------------------FIN CONTROL DE SEÑALES------------------------------
//?-------------------------------------------------------------------------------

            close(servidor);
            if (cliente[count] >= 0) {
                 
                int pidReceivedConv;
                //Leer datos del Socket
                while(leidos = read(cliente[count], &pidReceivedConv, sizeof(pidReceivedConv))) {
                    estadoActual(count);  
                    printf("Soy el %d\n", getpid());
                }
            }
            close(cliente[count]);
        }
        else {// Cumple la función de comunicación entre padre e hijo
            pidSemaforos[count] = read(cliente[count], i, sizeof(i));
        }
    }

    if (pid > 0) {
        for (int j = 0; j < 4; j++) {
            int nextSemaforo = (j + 1) % 4;
            write(cliente[j], &semaforos[nextSemaforo], sizeof(pidSemaforos[nextSemaforo]));
        }
         
        int start = 5; 
        int convert= htonl(start);
        write(cliente[0], &convert, sizeof(convert));

        while (wait(NULL) != -1);
        
        //Cerrar Sockets
        close(servidor);
        
    }
    //! Liberar memoria
    free(pidSemaforos);
    free(semaforos);
//!-------------------------------------------------------------------------------  
//!-------------------------------FIN DE SOCKETS----------------------------------
//!-------------------------------------------------------------------------------  
    return 0;
}
//* Con esta función mandamos el estado actual que tenemoos para imprimir los colores
void estadoActual(int s) {
    for (int i=0; i<4; i++) {
        if (i == s){
            printf("\033[0;92m");
            printf("Semaforo %d: VERDE\n", i);
            printf("\033[0m;");
        }
        else{
            printf("\033[0;91m");
            printf("Semaforo %d: ROJO\n", i);
            printf("\033[0m;");
        }
    }
    printf("\n");
}
//* Aquí cuando mandamos estos mensjaes debemos de mandar la señal o algo que reinicie y bloquee a todos los clientes
void gestor(int s){
	if (s == SIGTSTP){
        
        int ctrlZ=htonl(2);
        printf("Acabo de enviar el mensaje ROJO Ctrl+Z a todos los semaforos\n");
        
        for (int i = 0; i < 4; i++){
            printf("Semaforo %d: ROJO EMERGENCIA\n", i);
            write(cliente[i], &ctrlZ, sizeof(ctrlZ));
        }
        
	}
	else if(s==SIGINT){
        int ctrlC=htonl(3);
		printf("Acabo de enviar el mensaje INTERMITENTE Ctrl+C a todos los semaforos\n");
         for (int i = 0; i < 4; i++){
             printf("Semaforo %d: INTERMITENTE\n", i);
            write(cliente[i], &ctrlC, sizeof(ctrlC));
        }
	}
}
