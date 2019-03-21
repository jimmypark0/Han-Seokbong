#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>


/*********************** Constant ***********************/
#define INPUT_FILE_PATH		"./inputs/input.xml"
#define OUTPUT_FILE_PATH	"./outputs"
#define INFO_FILE_PATH		"./history/info.xml"
#define BUF_SIZE		1024 
#define NUM_MESSAGE		100
#define NUM_USER		20
#define USER_NAME		64
#define LOG			"./log.txt"
#define PERMS			0644



/*********************** Struct *************************/
struct Message {
	char time[9];
	char text[BUF_SIZE];
};

struct User {
	int numMessage;
	int idx;
	char id[USER_NAME];
	struct Message* messages[NUM_MESSAGE];
};


/*********************** Function ***********************/
int compareTime(char* t1, char* t2);
void makeIdFiles();
void mergeIdFiles();
void makeLog();
void init();



/*********************** Code ***************************/
int main()
{
    init();
    makeLog();
    makeIdFiles();
    mergeIdFiles();

    return 0;
}



/******************* compareTime *************************/
//param  : t1, t2 is "hh:mm:ss" formatted string
//return : if t1 is earier time than t2, return 1
//	   if t1 is equal to t2, return 0
//	   else return -1
int compareTime(char* t1, char* t2)
{
	int h1, h2, m1, m2, s1, s2;

	h1 = 10 * (t1[0] - '0') + (t1[1] - '0');
	h2 = 10 * (t2[0] - '0') + (t2[1] - '0');
	if(h1 < h2)		return 1;
	else if(h1 > h2)	return -1;

	m1 = 10 * (t1[3] - '0') + (t1[4] - '0');
	m2 = 10 * (t2[3] - '0') + (t2[4] - '0');
	if(m1 < m2)		return 1;
	else if(m1 > m2)	return -1;

	s1 = 10 * (t1[6] - '0') + (t1[7] - '0');
	s2 = 10 * (t2[6] - '0') + (t2[7] - '0');
	if(s1 < s2)		return 1;
	else if(s1 > s2)	return -1;
	else			return 0;
}



/******************* makeIdFiles *************************/
// from input file, make Id files by each Id
void makeIdFiles()
{
	FILE* input = fopen(INPUT_FILE_PATH, "r"), *output;
	char buf[BUF_SIZE];
	char ID[BUF_SIZE];
	char outPath[BUF_SIZE];
	int i, j;

	if(input == NULL) {
		printf("input.xml open is failed!\n");
		exit(1);
	}

	while(fgets(buf, sizeof buf, input) != NULL) {
		if(!strcmp(buf, "<Message>\n")) {
			// get ID
			fgets(buf, sizeof buf, input);

			for(i=5, j=0; i<strlen(buf); i++, j++) {
				if(!strcmp(buf+i, "</ID>\n")) break;
				ID[j] = buf[i];
			} ID[j] = '\0';

			outPath[0] = '\0';
			strcat(outPath, OUTPUT_FILE_PATH);
			strcat(outPath, "/");
			strcat(outPath, ID);
			strcat(outPath, ".xml");

			//write id
			output = fopen(outPath, "a");

			if(output == NULL) {
				printf("ID.xml file open is failed!\n");
				exit(1);
			}

			fprintf(output, "<Message>\n\t<ID>%s</ID>\n", ID);

			//write time, text, </Message>
			do{
				fgets(buf, sizeof(buf)+4, input);
				fprintf(output, "%s", buf);
			} while(strcmp(buf, "</Message>\n"));
			fclose(output);
		}
	}
	fclose(input);
}


/******************* mergeIdFiles *************************/
// in each Id files, sort by time . merge id files and make script
void mergeIdFiles()
{
	DIR *dir = opendir(OUTPUT_FILE_PATH);
	struct dirent *ent;
	FILE* idfile;
	struct User* user[NUM_USER];
	char idfilename[BUF_SIZE];
	char buf[BUF_SIZE];
	char script[strlen(OUTPUT_FILE_PATH)+strlen("/script.txt")+1];
	int numUser = 0;
	int i, j, k;


	script[0] = '\0';
	strcat(script, OUTPUT_FILE_PATH);
	strcat(script, "/script.txt");

	//sort id files
	if(dir) {
		while((ent = readdir(dir)) != NULL) {
			//check whether the file is '.xml' file or not
			if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;

			idfilename[0] = '\0';
			strcat(idfilename, OUTPUT_FILE_PATH);
			strcat(idfilename, "/");
			strcat(idfilename, ent->d_name);

			i = strlen(idfilename);
			if(i < 5) continue;
			if(!(idfilename[i-1]=='l' && idfilename[i-2] == 'm' && idfilename[i-3] == 'x' && idfilename[i-4] == '.')) continue;


			//for '.xml' file, make User struct
			user[numUser++] = (struct User*) malloc(sizeof(struct User));	
			user[numUser-1]->id[0] = '\0';
			strncpy(user[numUser-1]->id, ent->d_name, strlen(ent->d_name)-4);	//set id

			user[numUser-1]->idx = 0;
			idfile = fopen(idfilename, "r");

			i = 0;
			//get messages for sort
			while(fgets(buf, sizeof buf, idfile)) {
				if(!strcmp(buf, "<Message>\n")) {
					user[numUser-1]->messages[i++] = (struct Message*) malloc(sizeof(struct Message));

					//get time
					fgets(buf, sizeof buf, idfile); //ignore <ID>
					fgets(buf, sizeof buf, idfile); //get <Time>
					for(j=7, k=0; k<8; j++, k++) 
						user[numUser-1]->messages[i-1]->time[k] = buf[j];
					user[numUser-1]->messages[i-1]->time[k] = '\0';

					//get text
					fgets(buf, sizeof(buf) + 4, idfile);
					buf[strlen(buf)-strlen("</Text>")-1] = '\0';
					user[numUser-1]->messages[i-1]->text[0] = '\0';
					strcat(user[numUser-1]->messages[i-1]->text, buf + 7);
				}
			}
			user[numUser-1]->numMessage = i;
			fclose(idfile);

			//sort messages by time
			for(j=0; j<user[numUser-1]->numMessage-1; j++) {
				if(compareTime(user[numUser-1]->messages[j]->time, user[numUser-1]->messages[j+1]->time) < 0) {
					struct Message* temp = user[numUser-1]->messages[j];
					user[numUser-1]->messages[j] = user[numUser-1]->messages[j+1];
					user[numUser-1]->messages[j+1] = temp;
					j = -1;
				}
			}

			//over-write sorted messages to idfile(filename is ID. so delete <ID>
			idfile = fopen(idfilename, "w");
			for(j=0; j<i; j++) {
				fprintf(idfile, "<Message>\n\t<Time>%s</Time>\n\t<Text>%s</Text>\n</Messsage>\n",
							user[numUser-1]->messages[j]->time, user[numUser-1]->messages[j]->text);
			}
			fclose(idfile);
		}
	}
	closedir(dir);


	//merge id files.
	idfile = fopen(script, "w");	//var 'idfile' is re-used for script file
	if(idfile == NULL) {
		printf("script file make is failed\n");
		exit(1);
	}

	while(1) {
		//find user who has earliest time
		i = -1;	//min index for user who has earliest time	
		for(j=0; j<numUser; j++) {
			if(user[j]->idx < user[j]->numMessage) {
				if(i < 0 || compareTime(user[i]->messages[user[i]->idx]->time, user[j]->messages[user[j]->idx]->time) < 0)
					i = j;
			}
		}	

		if(i < 0) break;

		//write text until "***"
		if(!strcmp(user[i]->id, "Agenda"))
			fprintf(idfile, "[Agenda] ");
		else if(!strcmp(user[i]->id, "Conclusion"))
			fprintf(idfile, "[Conclusion] ");
		else
			fprintf(idfile, "%s : ", user[i]->id);
		while(strcmp(user[i]->messages[user[i]->idx]->text, "***")) {
			fprintf(idfile, "%s ", user[i]->messages[user[i]->idx]->text);
			user[i]->idx++;
		}		
		user[i]->idx++;
		fprintf(idfile, "\n\n");
	}
	fclose(idfile);

	//free memory of heap
	for(i=0; i<numUser; i++) {
		for(j=0; j<user[i]->numMessage; j++)
			free(user[i]->messages[j]);
		free(user[i]);
	}
}


/******************* makeLog *************************/
// read message(byte stream of 1024 byte) & make log
void makeLog()
{
	//file* id should be changed by 'bluetooth pairing'
	//file* message should be changed by 'bluetooth read' process
	FILE* idFile = fopen("./inputs/id.txt", "r");
	FILE* message = fopen("./inputs/message.bin", "r");


	FILE* log = fopen(INPUT_FILE_PATH, "w"); 
	char id[USER_NAME], buf[BUF_SIZE];
	short hour, min, sec, flag;
	int nread;

	if(message == NULL) {
		printf("error: message fopen is failed\n");
		exit(1);
	}

	fprintf(log, "<Data>\n");			
	while(fscanf(idFile, "%s", id) > 0) {

		if((nread = fread(buf, 1, BUF_SIZE, message)) < 0) {
			printf("read error in message.bin : nread(%d)\n", nread);
			exit(1);
		}

		flag = (short)buf[0];
		hour = (short)buf[1];
		min = (short)buf[2];
		sec = (short)buf[3];

		if(flag >> 1 == 1) {
			strcpy(id, "\0");
			strcpy(id, "Agenda");
		}
		else if(flag >> 2 == 1) {
			strcpy(id, "\0");
			strcpy(id, "Conclusion");
		}

		fprintf(log, "<Message>\n\t<ID>%s</ID>\n", id);

		fprintf(log, "\t<Time>%c%c:%c%c:%c%c</Time>\n", 
				(hour/10)+'0', (hour%10)+'0', (min/10)+'0', (min%10)+'0', (sec/10)+'0', (sec%10)+'0');

		fprintf(log, "\t<Text>");
		//fwrite(buf+4, 1, nread-4, log);
		fprintf(log, "%s", buf+4);
		fprintf(log, "</Text>\n</Message>\n");

		if(flag & 1 == 1) {
			fprintf(log, "<Message>\n\t<ID>%s</ID>\n", id);
			fprintf(log, "\t<Time>%c%c:%c%c:%c%c</Time>\n", 
					(hour/10)+'0', (hour%10)+'0', (min/10)+'0', (min%10)+'0', (sec/10)+'0', (sec%10)+'0');
			fprintf(log, "\t<Text>***</Text>\n</Message>\n");
		}
	}
	fprintf(log, "</Data>\n");			
	fclose(idFile);
	fclose(message);
	fclose(log);
}

void init() {
	int infoFd;
	char buf[BUF_SIZE];

	if((infoFd = open(INFO_FILE_PATH, O_WRONLY | O_CREAT | O_EXCL, PERMS)) < 0) return;

	strcpy(buf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");	
	strcat(buf, "<TABLE>\n");	
	strcat(buf, "</TABLE>\n");	

	if(write(infoFd, buf, strlen(buf)) < strlen(buf)) {
		printf("hsb: init(write error)\n");
		exit(1);
	}
	close(infoFd);
}
