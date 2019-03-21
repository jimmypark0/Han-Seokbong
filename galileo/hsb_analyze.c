#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/************************* Constant *************************/ 
#define MORPHEME_PATH	"./outputs/result.txt"
#define SCRIPT_PATH	"./outputs/script.txt"
#define KEYWORD_PATH	"./outputs/keyword.xml"
#define DIC_PATH	"./resource/dic.txt"
#define INFO_PATH	"./inputs/info.txt"
#define USER_PATH	"./outputs/user.txt"
#define KEY_HIT_PATH	"./outputs/keyHit"

#define BUF_SIZE	1024
#define STK_SIZE	10	//morphemes in one token would be less than 10
#define DATA_SIZE	64
#define MAX_PRINT_KEY	12

int max = 0;
int numPrintKey = 0;
char numHitFile = 0;


/************************** Struct **************************/ 
struct TreeNode {
	char data[DATA_SIZE];
	struct TreeNode *left, *right;
	int frequency;	//if "user tree", this means the number of mention of attendee
			//if "morpheme tree", this means the number of occurence of morpheme
	int keyword;	//for "user tree". this means the number of hit with keyword.
};

typedef struct TreeNode node;

struct Stack {
	char stk[DATA_SIZE][STK_SIZE];
	int top;
};
typedef struct Stack stack;

static node* dic = NULL;
static node* printKey = NULL;
static node* copyKey = NULL;

/************************ Function **************************/ 
void analyze();	 
void findKeyword(node* m, char* keyword);
void printKeyword(char* keyword);
void analyzeParticipation(node* u, int total);
void analyzeKeywordHit(node* u, char* keyword);
void printKeywordHit(node* u, char* keyword);
void incKey(node* u, char* id);
void decKey(node* u, char* id);

void makeKeywordFile(node* printKey);
void writeKeyword(FILE* keyFile, node* morp);
void report(node* morp, node* user, char* keyword, int total);

void initDic();
void addInfo();

void printUserList(node* user);
void copyTree(node* from);

//function for tree
void insert(node* r, char* id);
void insert2(node* r, int data, char* d);
void freeNode(node* r);
int find(node* r, char* data);

//function for stack
int isEmpty(stack* s);
void push(stack* s, char* data);
void pop(stack* s, char* data);


/************************** Code ****************************/ 
int main()
{
	analyze();

	return 0;
}

void analyze()
{
	node *user, *morp;
	stack *s;
	int total = 0, res, numKeyFile = 0;
	char keyword[DATA_SIZE], oldTok[DATA_SIZE], newTok[DATA_SIZE], m_tok[DATA_SIZE];
	FILE* file = fopen(MORPHEME_PATH, "r");

	if(file == NULL) {
		printf("morpheme file open is failed!\n");
		exit(1);
	}

	//init
	user = NULL;
	morp = NULL;
	oldTok[0] = '\0';
	s = (stack*) malloc(sizeof(stack));
	s->top = -1;
	initDic();

	//make user tree & morp tree
	res = fscanf(file, "%s", newTok);
	while(res > 0) {
		if(strstr(newTok, "[[") != NULL) { //newTok is token
			if(!strcmp(newTok, "[[:]]")) { //oldTok "[[userid]]" 
				if(user == NULL) {
					user = (node*) malloc(sizeof(node));
					strcpy(user->data, oldTok);
					user->left = NULL;
					user->right = NULL;
					user->frequency = 1;
					user->keyword = 0;
				} else 
					insert(user, oldTok);
				total++;
				strcpy(oldTok, newTok);
				while(!isEmpty(s))
					pop(s, m_tok);
				res = fscanf(file, "%s", newTok);
			} else { //newTok "[[morp+]]"
				//handle oldTok by pop morphemes from stack
				if(oldTok[0] != '\0') {
					while(!isEmpty(s)) {
						pop(s, m_tok);
						if(morp == NULL) {
							morp = (node*) malloc(sizeof(node));
							strcpy(morp->data, m_tok);
							morp->left = NULL;
							morp->right = NULL;
							morp->frequency = 1;
						} else
							insert(morp, m_tok);
						if(printKey == NULL) {
							printKey = (node*) malloc(sizeof(node));
							strcpy(printKey->data, m_tok);
							printKey->left = NULL;
							printKey->right = NULL;
							printKey->frequency = 1;
						} else
							insert(printKey, m_tok);
					}
				}
				//if newTok = "[[Agenda]]", write keyword file & handle new morp tree
				strcpy(oldTok, newTok);
				if(!strcmp(newTok, "[[Agenda]]")) {
					report(morp, user, keyword, total);
					total = 0;
					numKeyFile++;
				} else {
					//push morphemes into stack
					res = fscanf(file, "%s", newTok);
					while(res > 0 && strstr(newTok, "[[") == NULL) {
						push(s, newTok);
						res = fscanf(file, "%s", newTok);
					}
				}
			}
		}
	}
	fclose(file);
	while(!isEmpty(s)) {
		pop(s, m_tok);
		insert(morp, m_tok);
		insert(printKey, m_tok);
	}

	report(morp, user, keyword, total);
	copyTree(printKey);
	makeKeywordFile(copyKey);

	printUserList(user);

	//free tree
	freeNode(user);
	free(s);

	addInfo();
}

void insert(node* u, char* d)
{
	if(u == NULL) return;

	if(strcmp(u->data, d) < 0) {
		if(u->left == NULL) {
			u->left = (node*) malloc(sizeof(node));
			strcpy(u->left->data, d);
			u->left->left = NULL;
			u->left->right = NULL;
			u->left->frequency = 1;
			u->keyword = 0;
		} else
			insert(u->left, d);
	} else if(strcmp(u->data, d) > 0) {
		if(u->right == NULL) {
			u->right = (node*) malloc(sizeof(node));
			strcpy(u->right->data, d);
			u->right->left = NULL;
			u->right->right = NULL;
			u->right->frequency = 1;
			u->right->keyword = 0;
		} else
			insert(u->right, d);
	} else
		u->frequency++;
}
void insert2(node* r, int data, char* d) {
	if(r == NULL) return;

	if(r->frequency > data) {
		if(r->left == NULL) {
			r->left = (node*) malloc(sizeof(node));
			strcpy(r->left->data, d);
			r->left->left = NULL;
			r->left->right = NULL;
			r->left->frequency = data;
			r->keyword = 0;
		} else
			insert2(r->left, data, d);
	} else if(r->frequency <= data) {
		if(r->right == NULL) {
			r->right = (node*) malloc(sizeof(node));
			strcpy(r->right->data, d);
			r->right->left = NULL;
			r->right->right = NULL;
			r->right->frequency = data;
			r->right->keyword = 0;
		} else
			insert2(r->right, data, d);
	}
}

void findKeyword(node* m, char* keyword)
{
	if(m == NULL) return;

	if(max < m->frequency) {
		if(!find(dic, m->data)) {
			max = m->frequency;
			strcpy(keyword, m->data);
		}
	}

	findKeyword(m->left, keyword);
	findKeyword(m->right, keyword);
}

void printKeyword(char* keyword)
{
	FILE* file = fopen(SCRIPT_PATH, "a");

	if(file == NULL) {
		printf("script file open is failed!\n");
		exit(1);
	}

	fprintf(file, "\n\n[Keyword]\n본 회의록의 키워드는 \'%s\'(으)로 추정됩니다.\n\n", keyword);
	fclose(file);
}

void analyzeParticipation(node* u, int total)
{
	FILE* file;
	if(u == NULL) return;

	file = fopen(SCRIPT_PATH, "a");
	fprintf(file, "%s 의 회의 참여율은 %2.0lf %% 입니다.\n", u->data, ((double)u->frequency / (double)total) * 100);
	fclose(file);

	analyzeParticipation(u->left, total);
	analyzeParticipation(u->right, total);
}  

void analyzeKeywordHit(node* u, char* keyword)
{
	char oldTok[DATA_SIZE], newTok[DATA_SIZE], id[DATA_SIZE];
	FILE* script = fopen(SCRIPT_PATH, "r");

	if(script == NULL) {
		printf("script file open is failed!\n");
		exit(1);
	}

	oldTok[0] = newTok[0] = id[0] = '\0';
	while(fscanf(script, "%s", newTok) > 0) {
		if(!strcmp(newTok, ":")) { //oldTok is id
			if(strstr(oldTok, keyword) != NULL)
				decKey(u, id);		
			strcpy(id, oldTok);	
		}	

		if(strstr(newTok, keyword) != NULL)
			incKey(u, id);
		strcpy(oldTok, newTok);
	}
	fclose(script);
}

void incKey(node* u, char* id)
{
	char tmp[DATA_SIZE];
	if(u == NULL) 	return;
	if(id == NULL) 	return;
	
	strcpy(tmp, "[[");
	strcat(tmp, id);
	strcat(tmp, "]]");

	if(strcmp(u->data, tmp) < 0)		incKey(u->left, id);
	else if(strcmp(u->data, tmp) > 0)	incKey(u->right, id);
	else 
		u->keyword++;
}

void decKey(node* u, char* id)
{
	char tmp[DATA_SIZE];
	if(u == NULL) 	return;
	if(id == NULL) 	return;
	
	strcpy(tmp, "[[");
	strcpy(tmp, id);
	strcpy(tmp, "]]");

	if(strcmp(u->data, tmp) < 0)		incKey(u->left, id);
	else if(strcmp(u->data, tmp) > 0)	incKey(u->right, id);
	else {
		if(!strcmp(u->data, id))
			u->keyword--;
	}
}

void printKeywordHit(node* u, char* keyword)
{
	FILE* script, *keyHitFile;
	char temp[DATA_SIZE*4];
	int n;

	if(u == NULL) return;

	script = fopen(SCRIPT_PATH, "a");
	if(script == NULL) {
		printf("script file open is failed!\n");
		exit(1);
	}

	strcpy(temp, KEY_HIT_PATH);
	n = strlen(temp);
	temp[n] = numHitFile + '0';
	temp[n+1] = '\0'; 
	if((keyHitFile = fopen(temp, "a")) == NULL) {
		printf("key hit file open is failed!\n");
		exit(1);
	}

	fprintf(script, "%s 는 키워드 \'%s\'를 %d 번 언급했습니다.\n", 
			u->data, keyword, u->keyword);
	fclose(script);
	fprintf(keyHitFile, "%s %d\n", u->data, u->keyword);
	fclose(keyHitFile);

	u->keyword = 0; //init for reusing

	printKeywordHit(u->left, keyword);
	printKeywordHit(u->right, keyword);
}

void freeNode(node* r)
{
	if(r == NULL) return;

	freeNode(r->left);
	freeNode(r->right);
	free(r);
}

int isEmpty(stack* s)
{
	return s->top < 0;
}
void push(stack* s, char* data)
{
	strcpy(s->stk[++(s->top)], data);
}
void pop(stack* s, char* data)
{
	strcpy(data, s->stk[s->top--]);
}

void writeKeyword(FILE* keyFile, node* morp)
{
	if(morp == NULL) return;

	writeKeyword(keyFile, morp->right);
	if(morp->frequency > 1 && numPrintKey < MAX_PRINT_KEY) {
		fprintf(keyFile, "\t<WORD>\n");
		fprintf(keyFile, "\t\t<TEXT>%s</TEXT>\n", morp->data);
		fprintf(keyFile, "\t\t<SIZE>%d</SIZE>\n", morp->frequency);
		fprintf(keyFile, "\t</WORD>\n");
		numPrintKey++;
	}
	writeKeyword(keyFile, morp->left);
}

void report(node* morp, node* user, char* keyword, int total)
{
	FILE* file;

	//find keyword by max frequency & analyze keyword hit rate
	findKeyword(morp, keyword);
	analyzeKeywordHit(user, keyword);
	printKeyword(keyword);

	//report rate of attendency of user
	file = fopen(SCRIPT_PATH, "a");
	fprintf(file, "[Rate of Participation]\n");
	fclose(file);

	analyzeParticipation(user, total);

	//report keyword hit rate of each participant
	file = fopen(SCRIPT_PATH, "a");
	fprintf(file, "\n[Keyword Hit]\n");
	fclose(file);
	printKeywordHit(user, keyword);
}

void makeKeywordFile(node* copyKey)
{
	FILE* keyFile;

	//open keyword file to write morp information
	keyFile = fopen(KEYWORD_PATH, "w");
	if(keyFile == NULL) {
		printf("keyword file is failed to be created!\n");
		exit(1);
	}
	fprintf(keyFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(keyFile, "<TABLE>\n");
	
	//write morp information only for keyword that has frequency more than 2
	writeKeyword(keyFile, copyKey);
	fprintf(keyFile, "</TABLE>\n");
}

void initDic() {
	FILE* dicFile = fopen(DIC_PATH, "r");
	char buf[DATA_SIZE];

	if(dicFile == NULL) {
		printf("dictionay file open is failed\n");
		exit(1);
	}

	while(fscanf(dicFile, "%s", buf) != EOF) {
		if(dic == NULL) {
			dic = (node*) malloc(sizeof(node));
			strcpy(dic->data, buf);
			dic->left = NULL;
			dic->right = NULL;
		} else
			insert(dic, buf);
	}
}

int find(node* r, char* data) {
	if(r == NULL) return 0;

//debug
printf("hsb_analyze: find(dic_data:: %s), find(data:: %s)\n", r->data, data);

	if(!strcmp(r->data, data)) return 1;
	else if(strcmp(r->data, data) < 0) 	find(r->left, data);
	else					find(r->right, data);

	return 0;
}


void addInfo() {
	FILE* script, *info;
	char buf[BUF_SIZE];

	if((script = fopen(SCRIPT_PATH, "a"))  == NULL) {
		printf("script file open is failed\n");
		exit(1);
	}
	if((info = fopen(INFO_PATH, "r"))  == NULL) {
		printf("info file open is failed\n");
		exit(1);
	}

	fprintf(script, "\n[Time] ");	
	if(fgets(buf, BUF_SIZE, info) == NULL) {
		printf("fgets is failed : start time\n");
		exit(1);
	}
	fprintf(script, "%s ~ ", buf);
	if(fgets(buf, BUF_SIZE, info) == NULL) {
		printf("fgets is failed : end time\n");
		exit(1);
	}
	fprintf(script, "%s\n", buf);

	fprintf(script, "[Name] ");	
	if(fgets(buf, BUF_SIZE, info) == NULL) {
		printf("fgets is failed : minute name\n");
		exit(1);
	}
	fprintf(script, "%s\n", buf);
	fclose(script);
	fclose(info);
}

void printUserList(node* user) {
	FILE* userList;
	char temp[DATA_SIZE];

	if(user == NULL) return;

	if((userList = fopen(USER_PATH, "a")) == NULL) {
		printf("user.txt open failed\n");
		exit(1);
	}
	strcpy(temp, user->data);
	temp[strlen(temp)-2] = '\0';
	fprintf(userList, "%s\n", temp+2);
	fclose(userList);

	printUserList(user->left);
	printUserList(user->right);
}

void copyTree(node* from) {
	if(from == NULL) return;

	if(copyKey == NULL) {
		copyKey = (node*) malloc(sizeof(node));
		strcpy(copyKey->data, from->data);
		copyKey->left = NULL;
		copyKey->right = NULL;
		copyKey->frequency = from->frequency;
	} else
		insert2(copyKey, from->frequency, from->data);

	copyTree(from->left);
	copyTree(from->right);
}
