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
		while (true){
			printf("Saliendo\n");
			exit(0);
		}
	} else {
		printf("No salgo\n");
	}
}

// Se decide si es necesario que se envie una alarma
int get_message_code(char* buffer) {
	char alert_needed[][BUFSIZE] = {"empty\n", "Printer Error\n", "Communication Offline\n",
		"Communication error\n", "Low Cash alert\n", "Running Out of notes in cassette\n",
		"Service mode entered\n", "Service mode left\n", "device did not answer as expected\n",
		"The protocol was cancelled\n", "Low Paper warning\n", "Paper-out condition\n"
	};

	for (int i = 0; i < 12; i++) {
		if (strncmp(alert_needed[i], buffer, BUFSIZE) == 0) {
			return (i+1);
		}
	}
	return 0;
}

void *connection_handler(void *socket_desc) {
	int socket = *(int*) socket_desc;
	int read_size;
	int message_code = -1;
	char report_message[BUFSIZE];
	time_t current_time;
	struct tm * time_struct;
	char time_buffer[50];
	pthread_t self;
	self = pthread_self();
	int tid = (-1)*(int) self;

	while((read_size = recv(socket, report_message, BUFSIZE, 0)) > 0){

		if (ending_server) {
			return 0;
		}
		message_code = get_message_code(report_message);
		if (message_code != 0) {
			email_alarm(report_message, message_code);
		}

		write(socket, "Accepted\0", strlen("Accepted\0"));
		fflush(stdout);

		time(&current_time);
		time_struct = localtime( &current_time);

		//Escribir en bitacora
		//Controlar con semaforo

		sem_wait(&semaphore);

		output_file = output_ready(f_name);

		if(strftime(time_buffer, 50, "%d.%m.%Y", time_struct) == 0) {
			perror("Could not prepare string for time");
			return NULL;
		}

		fprintf(output_file, "%d:%s:%d %s",tid, time_buffer,message_code, report_message);
		printf("%d:%s:%d %s",tid, time_buffer,message_code, report_message);

		if (fclose(output_file) != 0){
      perror("Error al cerrar el archivo");
    }

		memset(report_message, '\0', BUFSIZE);

		sem_post(&semaphore);

		//Salida del area de escritura en archivo
	}

	if(read_size == 0){
	    puts("Client disconnected");
	    fflush(stdout);
	} else if(read_size == -1){
	    perror("recv failed");
	}

	return 0;
}

void email_alarm(char * report, int code) {
	printf("ALARMA! Reporte (%d) detectado: ", code);
}
