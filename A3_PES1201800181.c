#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#define DEBUG 1	
#define MAX_PAT_LENGTH 1000

enum{
CHAR, CHAR_CLASS, DIGIT, ALPHA, NULL_0, BEGIN, END, DOT, STAR, PLUS, QUESTION,
};

typedef struct RE {
	int     type;   /* CHAR, STAR, etc. */
	char    ch;     /* the character itself */
	char    *ccl;   /* for [...] instead */
} RE;


int start;
int end;

int match(char *pat, char *text);
int match_star(char ch, char* pat, char* text);
int match_here(char *pat, char *text);

RE* pattern_compile(char *pattern){
	int n = strlen(pattern);
	RE* pat_compiled = (RE*)malloc(sizeof(RE)*n);
	char* ccl_buf = (char*)malloc(sizeof(char)*n);
	int i=0;
	int j=0;
	int buff_i;
	char c;
	while(pattern[i]!= '\0'){
		char ccl_buf[n];
		buff_i=0;
		c = pattern[i];
		switch(c){
			case '^': {    pat_compiled[j].type = BEGIN;           } break;
			case '$': {    pat_compiled[j].type = END;             } break;
			case '.': {    pat_compiled[j].type = DOT;             } break;
			case '*': {    pat_compiled[j].type = STAR;            } break;
			case '+': {    pat_compiled[j].type = PLUS;            } break;
			case '?': {    pat_compiled[j].type = QUESTION;		   } break;
			case '\\':
			{
				if(pattern[i+1] != '\0'){
					i+=1;
					switch(pattern[i]){
						case 'd': {pat_compiled[j].type = DIGIT; } break;
						case 'w': {pat_compiled[j].type = ALPHA; } break;
						default:{
							pat_compiled[j].type = CHAR;
							pat_compiled[j].ch = pattern[i];
						}
					}
				}
				break;
			}
			case '[':
			{
				pat_compiled[j].type = CHAR_CLASS;
				while(pattern[++i]!=']' && pattern[i] != '\0'){
					if((pattern[i] == '\\') && (pattern[i+1] != '\0')){
						ccl_buf[buff_i++] = pattern[i++];
					}	
					ccl_buf[buff_i++] = pattern[i];
				}
				ccl_buf[buff_i] = '\0';
				pat_compiled[j].ccl = ccl_buf;
				break;
			}
			default:
			{
				pat_compiled[j].type = CHAR;
				pat_compiled[j].ch = c;
			}
		}
		i+=1;
		j+=1;
	}
	pat_compiled[j].type = NULL_0;
	return pat_compiled;
}

int match_star_non(char ch, char* pat, char* text)
{
	do{
		if(match_here(pat, text))
			return 1;
		
	}while(*text != '\0' && (*text++ == ch || ch == '.'));
	return 0;
}

int match_plus_non(char ch, char* pat, char* text)
{

	if(*text == ch && ch == '.'){
		end++;
		return match_star_non(ch, pat, ++text);
	}
	return 0;
}

int match_star(char ch, char* pat, char* text)
{
	char *t;
	for(t = text; *t != '\0' && (*t == ch || ch == '.') ; t++){
		end++;
	}	
	int temp;
	do{
		temp = end;
		if(match_here(pat, t))
			return 1;
		//printf("%s\n",t);
		end=temp-1;
	}while(t-- > text);
	return 0;
}

int match_plus(char ch, char* pat, char* text)
{
	if(*text == ch || ch == '.'){
		end++;
		return match_star(ch, pat, ++text);
	}
	return 0;
}

int match_question(char ch, char* pat, char* text)
{
	if(*text != '\0' && (*text == ch || ch =='.')){
		end++;
		text++;
	}
	if(match_here(pat, text))
		return 1;
	return 0;
}

int match_here(char *pat, char *text)
{
	// empty pattern
	if(*pat == '\0'){
		return 1;
	}
	if(pat[0] == '$' && pat[1] == '\0'){
		return *text == '\0';	
	}
	if(pat[1] == '*'){
		if(pat[2] == '?'){
			return match_star_non(pat[0], pat+3, text);
		}
		return match_star(pat[0], pat+2, text);
	}
	if(pat[1] == '?'){
		return match_question(pat[0], pat+2, text);
	}
	if(pat[1] == '+'){
		if(pat[2] == '?'){
			return match_plus_non(pat[0], pat+3, text);
		}
		return match_plus(pat[0], pat+2, text);
	}
	end++;

	if(*text != '\0' && (*pat == '.' || *pat == *text))
	{
		return match_here(pat + 1, text + 1);
	}	
	return 0;	
}

int match(char *pat, char *text)
{
	RE* pattern = pattern_compile(pat);
	/*while((*pattern).type != NULL_0){
		printf("%d\n",(*pattern).type);
		++pattern;
	}*/
	start = -1;
	end = -1	;
	if(*pat == '^')
	{
		start = 0;
		return match_here(pat + 1, text);
	}
	do
	{
		end = start;
		start++;	

		if(match_here(pat, text)){		
			return 1;
		}
	} while(*text++ != '\0');
	return 0;
}

int main()
{
	char text[4000];
	int i=0;
	char ch;
	ch = getchar();
	while(ch != '\n'){
		text[i++] = ch;
		ch = getchar();
	}
	text[i] = '\0';
	int M;
	scanf("%d",&M);
	char pattern[MAX_PAT_LENGTH];
	int val = 0;
	while(M--){
		scanf("%s", pattern);
		val = match(pattern, text);
		//printf("%d",val);
		if(val){
			if(end == -1){
				printf("%d %d %d\n", val, start, end+1);
			}
			else{
				printf("%d %d %d\n", val, start, end);
			}
		}
		else{
			printf("%d\n", val);
		}
	}
	return 0;
}

