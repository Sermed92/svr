#ifndef utilities
#define utilities

	// Importacion de librerias externas
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>		//strlen
	#include <sys/socket.h>
	#include <arpa/inet.h>	//inet_addr
	#include <unistd.h>		//write
	#include <errno.h>
	#include <signal.h>
	#include <time.h>
	#include <pthread.h>
	#include <getopt.h>
	#include <semaphore.h>
	#include <ctype.h>

	// Declaracion de mensajes base
	#define BUFSIZE 2048
	#define  SEPARATOR "\n"
	#define MAX_CONNECTIONS 1000
	extern const char *FILE_OPEN_FAIL;

	typedef enum { false, true } bool;
	// Archivo de salida
	FILE *output_file;
	char* f_name;
	bool ending_server;
	sem_t semaphore;
	int connection_counter;

	// Funciones a utilizar
	void argc_verify_s(int);
	void argc_verify_c(int);
	FILE *output_ready(char*);
	void sigintHandler(int);
	int get_message_code(char*);
	void *connection_handler(void*);
	void email_alarm(char *, int);
#else
#endif
