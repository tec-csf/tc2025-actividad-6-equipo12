/*
* Green
! Red
? blue
TODO: ORANGE xD 


TODO: Ya enviamos Rojo, nos falta quitarle uno a la primera vez

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

void gestor(int);
void gestorDos(int);

typedef struct{
     int pid;
     int cliente;
}PidSockets;
int cliente;
int * PIDsemaforos;

void imprimirEstructura(int, PidSockets*);
char ctrlZ[10]="ROJO";
char ctrlC[12]="INTERMITENTE";

typedef struct{
    int pid;
}ArregloPids;

int main(int argc, const char * argv[])
{
    struct sockaddr_in direccion;
    char buffer[1000];
    pid_t PIDsocket;
    sigset_t lasDos;
    sigemptyset(&lasDos);
    sigaddset(&lasDos, SIGINT);
    sigaddset(&lasDos, SIGTSTP);

    ArregloPids * pidis=(ArregloPids *) malloc(4*sizeof(ArregloPids));
    
    int received_int = 0;
    
    int servidor;
    int tubo[2];
    pipe(tubo);
    //int cliente;
    
    ssize_t leidos;
    socklen_t escritos;

    int continuar = 1;
    int pid;
    PidSockets* clientes;
    clientes =(PidSockets*)malloc(10*sizeof(PidSockets));
//?-------------------------------------------------------------------------------
//?-------------------------Control de señales Para el padre----------------------
//?-------------------------------------------------------------------------------
        if (signal(SIGTSTP, gestorDos) == SIG_ERR){
            printf("ERROR: No se pudo llamar al manejador\n");
        }
        else if (signal(SIGINT, gestorDos) == SIG_ERR){
            printf("ERROR: No se pudo llamar al manejador\n");
        }
//?-------------------------------------------------------------------------------
//?---------------------------FIN CONTROL DE SEÑALES------------------------------
//?-------------------------------------------------------------------------------
//!-------------------------------------------------------------------------------
//!---------------------------Scockets, control de SOCKETS------------------------  
//!-------------------------------------------------------------------------------  
//! Comente todos los close de el cliente y servidor para ver si eso solucionaba el problema de send to all pero no xD
    if (argc != 2) {
        printf("Use: %s IP_Servidor \n", argv[0]);
        exit(-1);
    }
    
    // Crear el socket
    servidor = socket(PF_INET, SOCK_STREAM, 0);
    
    // Enlace con el socket
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    bind(servidor, (struct sockaddr *) &direccion, sizeof(direccion));
    
    // Escuhar
    listen(servidor, 4);
    
    escritos = sizeof(direccion);
    
    // Aceptar conexiones
    
    while (continuar)
    {
        cliente= accept(servidor, (struct sockaddr *) &direccion, &escritos);

        printf("Aceptando conexiones en %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));
        pid= fork();
        
        if (pid == 0) continuar = 0;
        
    }

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
        int count=0;
        
        if (cliente >= 0) {
            cliente=cliente;
            printf("Cliente %d\n", cliente);
            // Leer datos del socket

            while (leidos= read(cliente, &received_int, sizeof(received_int))) {
                //write(fileno(stdout), &buffer, leidos);
                
                /* Leer de teclado y escribir en el socket */
                //leidos = read(fileno(stdin), &PIDsocket, sizeof(pid_t));
                //write(cliente, &buffer, leidos);
                //return_status = read(client_socket, &received_int, sizeof(received_int));
                if (leidos > 0) {
                    (pidis+count)->pid=ntohl(received_int);
                    fprintf(stdout, "Received int = %d\n", ntohl(received_int));
                    count++;
                }
                else {
                // Handling erros here
                }
            }
        }
        for(int i=0; i<4; i++){
            printf("%d\n", (pidis+i)->pid);
        }
        
        close(cliente);
    
    
    }
        
        else{
            if (pid > 0){

            leer(tubo);



            while (wait(NULL) != -1);
            
            // Cerrar sockets
            close(servidor); 
            
        }
        }
//!-------------------------------------------------------------------------------  
//!-------------------------------FIN DE SOCKETS----------------------------------
//!-------------------------------------------------------------------------------  
    return 0;
}
//* Aquí cuando mandamos estos mensjaes debemos de mandar la señal o algo que reinicie y bloquee a todos los clientes
/*void imprimirEstructura(int s, PidSockets*clientes){
         if (s == SIGTSTP){
		    printf("Acabo de enviar el mensaje ROJO Ctrl+Z a todos los semaforos\n");
            for(int i=0; i<10; i++){
            printf("Numero de Cliente: %d, Numero de PID que atiende: %d\n ",(clientes+i)->cliente, (clientes+i)->pid );
            }
     }

}*/

void gestor(int s){
	if (s == SIGTSTP){
		
        printf("Acabo de enviar el mensaje ROJO Ctrl+Z a todos los semaforos\n");
        write(cliente, &ctrlZ, 10);
	}
	else{
		printf("Acabo de enviar el mensaje INTERMITENTE Ctrl+C a todos los semaforos\n");
        write(cliente, &ctrlC, 12);
	}
}

void gestorDos(int s){
	if (s == SIGTSTP){
	}
	else{
	}
}


void leer(int * fd)
{
    int c;
    
    while( 1 )
    {
        close(fd[1]);
        read(fd[0], &c, sizeof(int));
        printf("---  Recibí f(%d) = %d  \n", c, factorial(c));
    }
    
}
void escribir(int * fd)
{
    int num;
    
    
    while(1)
    {
        printf("Entre un número: ");
        scanf("%d", &num);
        close (fd[0]);
        printf("+++ Envío %d \n", num);
        write(fd[1], &num, sizeof(int));
    }
    
    
}