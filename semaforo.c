#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define TCP_PORT 8000
void gestorDos(int);
//!-------------------Debemos de ver como hacer que este cliente se comunique con los otros clientes
//!-----------Pienso que tal vez debamos de usar la función Listen de el servidor
int main(int argc, const char * argv[]){
    // pid; 
    struct sockaddr_in direccion;
    char buffer[1000];
    sigset_t lasDos;
    sigemptyset(&lasDos);
    sigaddset(&lasDos, SIGUSR1);
    
    int cliente;
    
    ssize_t leidos, escritos;
    //pid=getpid();
    int number_to_send = getpid(); // Put your value
    int converted_number = htonl(number_to_send);

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


    if (argc != 2) {
        printf("Use: %s IP_Servidor \n", argv[0]);
        exit(-1);
    }
    
    // Crear el socket
    cliente = socket(PF_INET, SOCK_STREAM, 0);
    
    // Establecer conexión
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    escritos = connect(cliente, (struct sockaddr *) &direccion, sizeof(direccion));
    
    if (escritos == 0) {
        printf("%d\n", getpid());
        printf("Conectado a %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));
        
        // Escribir datos en el socket
        while ((leidos = read(fileno(stdin), &buffer, sizeof(buffer)))) {
            //write(cliente, &pid, sizeof(char));
            write(cliente, &converted_number, sizeof(converted_number));

            /* Lee del buffer y escribe en pantalla */
            leidos = read(cliente, &buffer, sizeof(buffer) );
            write(fileno(stdout), &buffer, leidos);
            //write(servidor, &pid, sizeof(pid_t));

        }
    }
    
    // Cerrar sockets
    close(cliente);
    
    return 0;
}

void gestorDos(int s){
	if (s == SIGTSTP){
	}
	else{
	}
}