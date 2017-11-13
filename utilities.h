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

	// Declaracion de mensajes base
	#define BUFSIZE 2048

	extern const char *FILE_OPEN_FAIL;
	typedef enum { false, true } bool;
	// Archivo de salida
	FILE *output_file;
	char* f_name;
	bool ending_server;


	// Funciones a utilizar
	void argc_verify_s(int);
	FILE *output_ready(char*);
	void sigintHandler(int);

#else
#endif
