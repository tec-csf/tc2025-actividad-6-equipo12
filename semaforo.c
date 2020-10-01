#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define TCP_PORT 8000
#define time 3
//!-------------------Debemos de ver como hacer que este cliente se comunique con los otros clientes
//!-----------Pienso que tal vez debamos de usar la función Listen de el servidor
//* Tipos de funciones

void gestorEstado(int signal);
void gestorNextEstado(int signal);

//* Variables Globales
int cliente;
int nextPid; 
int estado; 

int main(int argc, const char * argv[]){
 
    struct sockaddr_in direccion;
    int pidToSend=getpid();
    int pidConvert=htonl(pidToSend);
    int estadoAnterior;
    int pidGet;
    ssize_t leidos;
    socklen_t escritos;
    sigset_t lasDos;
//?-------------------------------------------------------------------------------
//?-------------------------Control de señales Para el padre----------------------
//?-------------------------------------------------------------------------------
    sigemptyset(&lasDos);
    sigaddset(&lasDos, SIGUSR1);
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
    
    if (argc != 2){
        printf("Use: %s IP_Servidor \n", argv[0]);
        exit(-1);
    }
    
    int received_int;//Para las señales que llegan
    // Crear el socket
    cliente = socket(PF_INET, SOCK_STREAM, 0);
    // Establecer conexión
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    escritos = connect(cliente, (struct sockaddr *) &direccion, sizeof(direccion));
    
    if (escritos == 0) {
        printf("Conectado a %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));

        write(cliente, &pidConvert, sizeof(pidConvert));
        leidos = read(cliente, &pidGet, sizeof(pidGet));
        nextPid=ntohl(pidGet);

        signal(SIGUSR1, gestorEstado);
        signal(SIGALRM, gestorNextEstado);
                   
        while ((leidos = read(cliente, &received_int, sizeof(received_int)))){
            if (ntohl(received_int) == 5) {
                raise(SIGUSR1);
            } 
            else if (ntohl(received_int) == 2 && estado != 2) {
                estadoAnterior=estado;
                sigprocmask(SIG_BLOCK, &lasDos, NULL);
                estado = 2;
            } 
            else if (ntohl(received_int) == 3 && estado != 3) {
                estadoAnterior=estado;
                sigprocmask(SIG_UNBLOCK, &lasDos, NULL);
                estado = 3;
            }
            else if (ntohl(received_int) == 2 && estado == 2) {//? Para la segunda Vez
                sigprocmask(SIG_UNBLOCK, &lasDos, NULL);
                estado = estadoAnterior;
            } 
            else if (ntohl(received_int) == 3 && estado == 3) { //? Para la segunda vez
                sigprocmask(SIG_UNBLOCK, &lasDos, NULL);
                estado = estadoAnterior;
            }     
        }
    }
    // Cerrar sockets
    close(cliente);
    
    return 0;
}

void gestorEstado(int signal) {
    estado = 1;
    int state=htonl(estado);
    write(cliente, &state, sizeof(state));
    printf("Me cambie a verde soy el %d\n", getpid());
    alarm(time);
}

void gestorNextEstado(int signal) {
    estado = 0;
    kill(nextPid, SIGUSR1);
}