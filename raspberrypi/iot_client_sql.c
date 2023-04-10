#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <mysql/mysql.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define ARR_CNT 5


void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE] = "[Default]";
char msg[BUF_SIZE];
void finish_with_error(MYSQL *conn)
{
	fprintf(stderr, "%s\n", mysql_error(conn));
	mysql_close(conn);
	exit(1);        
}
int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread, mysql_thread;
	void* thread_return;

	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	sprintf(name, "%s", argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	sprintf(msg, "[%s:PASSWD]", name);
	write(sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);


	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);
	return 0;
}


void* send_msg(void* arg)
{
	int* sock = (int*)arg;
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[NAME_SIZE + BUF_SIZE + 2];

	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n", stdout);
	while (1) {
		memset(msg, 0, sizeof(msg));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if (FD_ISSET(STDIN_FILENO, &newset))
		{
			fgets(msg, BUF_SIZE, stdin);
			if (!strncmp(msg, "quit\n", 5)) {
				*sock = -1;
				return NULL;
			}
			else if (msg[0] != '[')
			{
				strcat(name_msg, "[ALLMSG]");
				strcat(name_msg, msg);
			}
			else
				strcpy(name_msg, msg);
			if (write(*sock, name_msg, strlen(name_msg)) <= 0)
			{
				*sock = -1;
				return NULL;
			}
		}
		if (ret == 0)
		{
			if (*sock == -1)
				return NULL;
		}
	}
}

void* recv_msg(void* arg)
{
	MYSQL* conn;
	MYSQL_RES* res_ptr;
	MYSQL_ROW sqlrow;
	char strfir[200] = { 0 };
	char strsec[200] = { 0 };
	char* host = "localhost";
	char* user = "ubuntu";
	char* pass = "ubuntu";
	char* dbname = "April";
	int* sock = (int*)arg;
	int i;
	char* pToken;
	char* pArray[ARR_CNT] = { 0 };
	int j=0;
	char name_msg[NAME_SIZE + BUF_SIZE + 1];
	int str_len;
	int count1 = 0;
	int count2 = 0;
	int resv;
	int pw;
	int rfid;
	int room;
	int name;
	conn = mysql_init(NULL);
	char roomBuf[100]={0};
	char preRoompw[20]={0};
	char curRoompw[20]={0};
	char sendRoompw[100]={0};
	puts("MYSQL startup");
	if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
	{
		fprintf(stderr, "ERROR : %s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	else
		printf("Connection Successful!\n\n");

	while (1) {
		memset(name_msg, 0x0, sizeof(name_msg));
		str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE);
		if (str_len <= 0)
		{
			*sock = -1;
			return NULL;
		}
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);

		pToken = strtok(name_msg, "[:@]");
		i = 0;
		while (pToken != NULL)
		{
			pArray[i] = pToken;
			if ( ++i >= ARR_CNT)
				break;
			pToken = strtok(NULL, "[:@]");

		}
		if((strcmp(pArray[1],"resv")==0) && (i == 3)){
			resv = atoi(pArray[2]);
			if (mysql_query(conn, "SELECT name, resv FROM information")) 
			{
				finish_with_error(conn);
			}

			res_ptr = mysql_store_result(conn);

			if (res_ptr == NULL) 
			{
				finish_with_error(conn);
			}

			int num_fields = mysql_num_fields(res_ptr);

		
			sprintf(strfir,"%d",resv);
			
			while ((sqlrow = mysql_fetch_row(res_ptr))) //resv
			{	
				count1 = 0; 
				for(i = 0; i < num_fields; i++) 
				{ 
					if(i%2==0){					
						if(strcmp(sqlrow[i+1],strfir)==0){
							sprintf(name_msg,"[%s]correct@%s\r\n",pArray[0],sqlrow[i]);//resv
							count1++;
							write(*sock, name_msg, strlen(name_msg));
						}
					}
				} 
				printf("\n"); 
			}
			if(count1 == 0){
				sprintf(name_msg,"[%s]Decline\r\n",pArray[0]);
				write(*sock, name_msg, strlen(name_msg));
			}
		}
		else if((strcmp(pArray[1],"pass")==0) && (i == 4)){
			pw = atoi(pArray[2]);
			rfid = atoi(pArray[3]);
			if (mysql_query(conn, "SELECT id, pw, rfid FROM information")) 
			{
			finish_with_error(conn);
			}

			res_ptr = mysql_store_result(conn);

			if (res_ptr == NULL) 
			{
				finish_with_error(conn);
			}

			int num_fields = mysql_num_fields(res_ptr);

			sprintf(strfir,"%d",pw);
			sprintf(strsec,"%d",rfid);
		
			while ((sqlrow = mysql_fetch_row(res_ptr))) 
			{ 
				count2=0;
				for(i = 0; i < num_fields; i++) 
				{ 
					if (i%3==0){	
						if((strcmp(sqlrow[i+1],strfir)==0) && (strcmp(sqlrow[i+2],strsec)==0)){
							sprintf(name_msg,"[%s]correct@%s\r\n",pArray[0],sqlrow[i]); //pass
							count2++;
							printf("%d,%s",i,name_msg);
							sleep(1);
							write(*sock, name_msg, strlen(name_msg));
						}
					}	
				} 
				printf("\n"); 
			
			}
			if(count2 == 0){
				sleep(1);
				sprintf(name_msg,"[%s]Decline\r\n",pArray[0]);
				write(*sock, name_msg, strlen(name_msg));
			}
		}else if((strcmp(pArray[1],"room")==0) && (i == 4)){
			room = atoi(pArray[2]);
			name = atoi(pArray[3]);
			sprintf(strsec,"SELECT roomPw FROM information Where id=%d",name);
			if (mysql_query(conn, strsec)) 
			{
				finish_with_error(conn);
			}

			res_ptr = mysql_store_result(conn);

			if (res_ptr == NULL) 
			{
				finish_with_error(conn);
			}

			int num_fields = mysql_num_fields(res_ptr);

			sprintf(strfir,"%d",pw);
		
			while ((sqlrow = mysql_fetch_row(res_ptr))) 
			{ 
				for(i = 0; i < num_fields; i++) 
				{
					sprintf(preRoompw,"%s",sqlrow[i]);
				}	
			}  
			
		
			sprintf(roomBuf,"UPDATE information SET roomPw='%d' WHERE id=%d",room,name);
			if (mysql_query(conn, roomBuf)) 
			{
				finish_with_error(conn);
			}

			sprintf(strsec,"SELECT roomPw FROM information Where id=%d",name);
			if (mysql_query(conn, strsec)) 
			{
				finish_with_error(conn);
			}

			res_ptr = mysql_store_result(conn);

			if (res_ptr == NULL) 
			{
				finish_with_error(conn);
			}

			int num_fields2 = mysql_num_fields(res_ptr);

			sprintf(strfir,"%d",pw);
		
			while ((sqlrow = mysql_fetch_row(res_ptr))) 
			{ 
				for(i = 0; i < num_fields2; i++) 
				{
					sprintf(curRoompw,"%s", sqlrow[i]);
				}	
			}
			if(strcmp(preRoompw,curRoompw)!=0){
				sprintf(sendRoompw,"[LJM_AND]%s\r\n",curRoompw);
				write(*sock,sendRoompw,strlen(sendRoompw));
			}  
		}else 
			continue;
		
	}
	mysql_free_result(res_ptr);
	mysql_close(conn);

}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
