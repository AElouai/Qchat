#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
//Mysql
//#include <mysql/mysql.h>


#define CLIENT 100
#define BUFF_LENGTH 1000
#define BUFF_HST 10000
#define D_PORT 1213

//Mysql
//MYSQL *con;

int contacts = 0;
pthread_t tid[CLIENT];	
int active_socket[CLIENT];
int thread_retval = 0;
int sd;
int endloop;

//Mysql
/*
void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "MYSQL Error :%s\n", mysql_error(con));
}

void startDB(MYSQL *con){
con = mysql_init(NULL);
con = mysql_real_connect(con, "localhost","root", "", "chat",3306, NULL, 0);
}
*/
typedef struct contact{
	char contactname[256];
	int contactsd;
}contact;

contact onlinecontacts[CLIENT];

void chat(int sd2){
	int n,i;	
	
	char socket_name[BUFF_LENGTH];
	char *var;
	char message[BUFF_LENGTH];
	char in[BUFF_LENGTH];
	char out[BUFF_LENGTH];
	
	while(1){

		for(i = 0; i < BUFF_LENGTH; i++){ in[i] = 0;out[i] = 0;}

		n = read(sd2, in, sizeof(in));
		
		if(!strcmp(in, "@EXIT@")){
				sprintf(out, "Client");
				for(i = 0; i < contacts; i++){
					if(onlinecontacts[i].contactsd != sd2)
					sprintf(out, "%s@%s",out, onlinecontacts[i].contactname);
				}
				for(i = 0; i < contacts; i++){
					if(onlinecontacts[i].contactsd != sd2)
					write(onlinecontacts[i].contactsd, out, sizeof(out));
				}
			return ;
		}else if(!((in != NULL) && (in[0] == '\0'))){
			if(!strncmp(in, "<", 1)){
				var = strstr(in, "<");
				i = 0;		
				var++;	
				while(*var != '>'){socket_name[i] = *var;var++;i++;}
				socket_name[i] = '\0';
				
				var = strstr(in, ">");
				i = 0;		
				var++;	
				while(*var != '\0'){message[i] = *var;var++;i++;}
				message[i] = '\0';

				//printf("\nMessage [%s] is for [%s]\n\n", message, socket_name);

				for(i = 0; onlinecontacts[i].contactsd != sd2; i++);
				sprintf(out, "MSG@%s:%s", onlinecontacts[i].contactname, message);
/**/
				char buf[200];
				memset(&buf,0,sizeof(buf));
//Mysql
/*				if ( con ){
					sprintf(buf, "INSERT INTO message(src,dst,msg) VALUES('%s','%s','%s')",onlinecontacts[i].contactname,socket_name,message);
					
				    if (mysql_query(con, buf)) { finish_with_error(con);}
				}
*/
				i = 0;
				while(strcmp(onlinecontacts[i].contactname, socket_name)){
					i++;
				}

				write(onlinecontacts[i].contactsd, out, sizeof(out));

			}else 	if(!strncmp(in, ";", 1)){
				var = strstr(in, ";");
				i = 0;		
				var++;	
				while(*var != ':'){socket_name[i] = *var;var++;i++;}
				socket_name[i] = '\0';

				for(i = 0; onlinecontacts[i].contactsd != sd2; i++);
				
//Mysql
/*				
				if ( con ){
					char buf[200];
				    sprintf(buf, "SELECT DISTINCT * FROM message where (src='%s' and  dst='%s') or (src='%s' and  dst='%s') ORDER BY ordate",onlinecontacts[i].contactname, socket_name, socket_name,onlinecontacts[i].contactname);
				   	if (mysql_query(con, buf)) { 
				   		finish_with_error(con);
				   	}else{			
				   		printf("sending historique\n");	 
					  	MYSQL_RES *result = mysql_store_result(con);
					 	if (result == NULL){ finish_with_error(con);}

						MYSQL_ROW row;
					  	char Bbuf[BUFF_HST];
					  	sprintf(Bbuf,"HST@");
						while ((row = mysql_fetch_row(result))){ 
						    sprintf(Bbuf,"%s<%s>%s>%s",Bbuf,row[0],row[1],row[3]); 
					 	}
						for(i = 0; onlinecontacts[i].contactsd != sd2; i++);
						write(onlinecontacts[i].contactsd, Bbuf, sizeof(Bbuf));	
						printf("Buf :%s\n",Bbuf );
					  	mysql_free_result(result);	
					}
				}
*/			}else{
				for(i = 0; onlinecontacts[i].contactsd != sd2; i++);
				sprintf(message, "BRD@%s : %s ", onlinecontacts[i].contactname, in);
				strcpy(out, message);	
				for(i = 0; i < contacts; i++){
					if(onlinecontacts[i].contactsd != sd2)
						write(onlinecontacts[i].contactsd, out, sizeof(out));
				}
			}
			
		}
	}	
}

void interrupt_handler(int sig){
	endloop = 1;
	close(sd);
//mysql
//	mysql_close(con);
	printf("DB and serve are down\n");
	return;
}
	
void *connection(void *sdp) {

	char    in[BUFF_LENGTH];       
	char	out[BUFF_LENGTH];	
	
	int i;			
	int sd2 = *((int *)sdp);
//---->!!! pro here
	int j = ((int *)sdp)-active_socket;/* use pointer arithmetic to get this thread's index in array */

	for(i = 0; i < BUFF_LENGTH; i++){ in[i] = 0;out[i] = 0;}
	
	/*Send the list of Client*/
	sprintf(out, "Client");
	for(i = 0; i < contacts; i++){ sprintf(out, "%s@%s",out, onlinecontacts[i].contactname);}

	for(i = 0; i < contacts; i++){ write(onlinecontacts[i].contactsd, out, sizeof(out)); }
	
	chat(sd2);

	tid[j] = (pthread_t)-1; //è un cast...	/* free thread array entry */
	
	close(sd2);
	contacts--;

	return &thread_retval;

}

int main(int argc, char** argv){
	
	if(argc < 2){
	printf("\nNeed at least IP address PORT is optionelle ");
		return 0;
	}

//Mysql
/*	con = mysql_init(NULL);
	if ( !con ){	printf("mysql_init failed\n"); exit(0);	}
	con = mysql_real_connect(con, "localhost","root", "", "chat",3306, NULL, 0);
*/
	contact newcontact;	

	struct sockaddr_in sad;
	struct sockaddr_in cad;
	socklen_t alen;
	
	memset(&sad,0,sizeof(sad));
	memset(&cad,0,sizeof(cad));
	memset(&alen,0,sizeof(alen));

	int sd2, port, n, i, j = 0;

	char* var;
	char socket_name[256];
	char busymsg[] = "BUSY";
	char buffer[BUFF_LENGTH];

	for(i=0;i<CLIENT;i++) {
		tid[i] = (pthread_t)-1;
		active_socket[i] = -1;
	}

	for(i = 0; i < BUFF_LENGTH; i++){ buffer[i] = 0;}
	
	if(argc == 3){
		port = atoi(argv[2]);
		while(port < 0 || port > 65000){
			printf("Bad port number, buond limits are (0,65000)\n\nEnter a new port number: ");
			scanf("%d", &port);
		}
	}else{
		port = D_PORT;
	}

	//memset((char*)&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	n = inet_aton(argv[1], &sad.sin_addr);
	sad.sin_port = htons((u_short)port);

	printf("Server IP address : %s  port: %d ", argv[1], port);

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd < 0) {
		perror("Socket creation failed\n");
		exit(1);
	}
	printf("Socket id: [%d]\n", sd);

	n = bind(sd, (struct sockaddr *)&sad, sizeof(sad));
	if(n == -1){
		perror("Error in Bind\n");
		exit(1);
	}

	n = listen(sd, 4);
	if(n < 0){
		perror("Listen failed\n");
		exit(1);
	}
	
	signal(SIGINT, interrupt_handler);


	while(!endloop){

		alen = sizeof(cad);
	
		printf("Server is waiting for a Client to serve...\n");
		
		sd2 = accept(sd, (struct sockaddr *)&cad, &alen);
		if (sd2 < 0) {
			if(endloop) break;
			perror("Accept failed\n");
			exit(1);
		}	
	
		if(contacts < CLIENT) {
			
			printf("IP: [%s]\nPORT:[%hu]\nSOCKET:[%d]\n", inet_ntoa(cad.sin_addr), ntohs(cad.sin_port), sd2);
			n = read(sd2, buffer, sizeof(buffer));
			printf("%s\n", buffer);
		
			var = strstr(buffer, "<");
			i = 0;		
			var++;	
			while(*var != '>'){
				socket_name[i] = *var;
				var++;
				i++;
			}
			socket_name[i] = '\0';
				
			printf("\n%s Connected", socket_name);
	
			strcpy(newcontact.contactname, socket_name);
			newcontact.contactsd = sd2;
			onlinecontacts[contacts] = newcontact;

			//look for the first empty slot in thread array 
			for(i=0;tid[i]!=(pthread_t)-1;i++);
			
			active_socket[i] = sd2;
			if(pthread_create(&tid[i], NULL, connection, &active_socket[i])!=0) {
				perror("Thread creation");
				tid[i] = (pthread_t)-1; // to be sure we don't have unknown values... cast
				continue;
			}
			contacts++;
		
		} else {  //too many threads 
			printf("Maximum threads active: closing connection\n");
			write(sd2, busymsg, strlen(busymsg)+1);
			close(sd2);
		}
	}
	printf("Server finished\n");
}
