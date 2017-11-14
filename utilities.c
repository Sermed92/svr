#include "utilities.h"

// Definicion de mensajes base
const char *FILE_OPEN_FAIL = "Error! - No se pudo abrir archivo";
const char *WRONG_ARGUMENT_S = "Error!\n Uso de svr_s:\n./svr_s -l <puerto_svr_s> -b <archivo_bitacora>";
const char *WRONG_ARGUMENT_C = "Error!\n Uso de svr_c:\n./svr_c -d <nombre_modulo_central> -p <puerto_svr_s> [-l <puerto_local>]";


// Se verifica la cantidad de argumentos recibida
void argc_verify_s(int amount) {
	if (amount != 5) {
		printf("%s\n", WRONG_ARGUMENT_S);
		exit(1);
	}
}

void argc_verify_c(int amount) {
	if (amount != 5) {
		if (amount != 7){
			printf("%s\n", WRONG_ARGUMENT_C);
			exit(1);
		}
	}
}

// Se abre el archivo a usar como bitacora en modo append
FILE *output_ready(char* f_name){

	if ((output_file = fopen(f_name, "a")) == NULL) {
		printf("%s %s\n",FILE_OPEN_FAIL, f_name);
		exit(1);
	}
	else {
		//printf("Salida: %s\n", arr[opts]);
		fprintf(output_file, "\n------------Nueva bitacora------------\n");
		return output_file;
	}
}

// Manejador de señales para evitar el cierre del programa
void sigintHandler(int sig_num){
    signal(sig_num, sigintHandler);
	char option;
    printf("\n Deseas finalizar el programa? [s/n]");
	fflush(stdout);
	option = getchar();

    if (option=='s'){
		ending_server = true;
		if (fclose(output_file) != 0) {
			perror("Error al cerrar el archivo");
		}
		while (true){
			printf("Saliendo\n");
			exit(0);
		}
	} else {
		printf("No salgo\n");
	}
}

// Se decide si es necesario que se envie una alarma
bool verify_alarm_need(char* buffer) {
	char alert_needed[][BUFSIZE] = {"empty\n", "Printer Error\n", "Communication Offline\n",
		"Communication error\n", "Low Cash alert\n", "Running Out of notes in cassette\n",
		"Service mode entered\n", "Service mode left\n", "device did not answer as expected\n",
		"The protocol was cancelled\n", "Low Paper warning\n", "Paper-out condition\n"
	};

	for (int i = 0; i < 12; i++) {
		if (strncmp(alert_needed[i], buffer, BUFSIZE) == 0) {
			return true;
		}
	}
	return false;
}

void *connection_handler(void *socket_desc) {
	int socket = *(int*) socket_desc;
	int read_size;
	char report_message[BUFSIZE];
	while((read_size = recv(socket, report_message, BUFSIZE, 0)) > 0){
		if (ending_server) {
			return 0;
		}

		if (verify_alarm_need(report_message)) {
			printf("ALARMA MAMA, SE QUEMA LA COCINA\n");
		}

		write(socket, "Accepted\0", strlen("Accepted\0"));
		fflush(stdout);

		//Escribir en bitacora
		//Controlar con semaforo

		sem_wait(&semaphore);

		fprintf(output_file, "%s", report_message);
		memset(report_message, '\0', BUFSIZE);

		sem_post(&semaphore);

		//Salida del area de escritura en archivo
	}

	return 0;
}

void email_alarm(char * report) {
	printf("Alarmaaaa %s\n", report );
}
