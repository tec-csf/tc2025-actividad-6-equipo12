/*
* Green
! Red
? blue
TODO: ORANGE xD 
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

int main(int argc, const char * argv[])
{
    struct sockaddr_in direccion;
    char buffer[1000];
    
    int servidor;
    int * cliente;
    
    ssize_t leidos;
    socklen_t escritos;

    int continuar = 1;
    pid_t pid;
//?-------------------------------------------------------------------------------
//?-------------------------Control de señales------------------------------------
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
    listen(servidor, 10);
    
    escritos = sizeof(direccion);
    
    // Aceptar conexiones
    while (continuar)
    {
        cliente [0]= accept(servidor, (struct sockaddr *) &direccion, &escritos);
        
        printf("Aceptando conexiones en %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));
        
        pid = fork();
        
        if (pid == 0) continuar = 0;
        
    }
    
    if (pid == 0) {
        
        close(servidor);
        
        if (cliente >= 0) {
            
            // Leer datos del socket
            while (leidos = read(cliente[0], &buffer, sizeof(buffer))) {
                write(fileno(stdout), &buffer, leidos);
                
                /* Leer de teclado y escribir en el socket */
                leidos = read(fileno(stdin), &buffer, sizeof(buffer));
                write(cliente[0], &buffer, leidos);
            }
        }
        
        //close(cliente[0]);
    }
    
    else if (pid > 0)
    {
        while (wait(NULL) != -1);
        
        // Cerrar sockets
        //close(servidor); 
        
    }
//!-------------------------------------------------------------------------------  
//!-------------------------------FIN DE SOCKETS----------------------------------
//!-------------------------------------------------------------------------------  
    return 0;
}
//* Aquí cuando mandamos estos mensjaes debemos de mandar la señal o algo que reinicie y bloquee a todos los clientes
void gestor(int s){
	if (s == SIGTSTP){
		printf("Acabo de enviar el mensaje ROJO Ctrl+Z a todos los semaforos\n");
	}
	else{
		printf("Acabo de enviar el mensaje INTERMITENTE Ctrl+C a todos los semaforos\n");
	}
}