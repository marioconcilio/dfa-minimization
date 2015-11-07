
/************************************
	ACH2043 - Introducao a Teoria da Computacao
	Prof. Marcelo de Souza Lauretto

	Estrutura de dados e funcoes sobre AFDs
	Adaptacao de:
	RAMOS, M. V. M.; NETO, J. J.; VEGA, I. S. Linguagens Formais. Ed. Bookman

  Chamada:
     converte_afd <inp_afd> <out_afd>
  Ver detalhes dos tipos de arquivos no comentario da funcao main

  compilacao:
    gcc -Wall -O3 -o converte_afd.exe afd_util.c -D _STANDALONE_

*************************************/

// #define _STANDALONE_

#define M_PI 3.14159265358979323846
#define MAX_AFD_SZ 200
#define swap(type, i, j) {type vaux = i; i = j; j = vaux;}

#include "afd_util.h"



/* ***********************************
   As duas funcoes auxiliares abaixo implementam a funcao getline, que nao eh padrao em C.
   Foram obtidas atraves do forum:
     http://stackoverflow.com/questions/27369580/codeblocks-and-c-undefined-reference-to-getline
     author: BLUEPIXY
*/
ssize_t getdelim(char **linep, size_t *n, int delim, FILE *fp){
    int ch;
    size_t i = 0;
    if(!linep || !n || !fp){
        errno = EINVAL;
        return -1;
    }
    if(*linep == NULL){
        if(NULL==(*linep = malloc(*n=128))){
            *n = 0;
            errno = ENOMEM;
            return -1;
        }
    }
    while((ch = fgetc(fp)) != EOF){
        if(i + 1 >= *n){
            char *temp = realloc(*linep, *n + 128);
            if(!temp){
                errno = ENOMEM;
                return -1;
            }
            *n += 128;
            *linep = temp;
        }
        (*linep)[i++] = ch;
        if(ch == delim)
            break;
    }
    (*linep)[i] = '\0';
    return !i && ch == EOF ? -1 : i;
}
ssize_t getline(char **linep, size_t *n, FILE *fp){
    return getdelim(linep, n, '\n', fp);
}
/* **************************************************** */


/*
  InicializaAFD(AFD, n, s):
  Inicializa a estrutura para um AFD com n estados e s simbolos
  Aloca espaco para o vetor binario de estados finais e para a matriz de transicao
*/
void InicializaAFD(tAFD *aut, int n, int s) {
  int i, j;
  aut->n = n;
  aut->s = s;

  aut->F = (int*) calloc(n, sizeof(int));
  for (i=0; i<n; i++) aut->F[i] = 0;

  aut->Delta = (int**) calloc(n, sizeof(int*));
  for (i=0;i<n; i++)
    aut->Delta[i] = (int*) calloc(s, sizeof(int));

  for (i=0; i<n; i++)
    for (j=0; j<s; j++)
      aut->Delta[i][j] = -1;
}


/*
  LiberaAFD(AFD): Liberar o espaco ocupado por um AFD.
*/
void LiberaAFD (tAFD *aut) {
  int i;
  int *v;

  free(aut->F);
  for (i=0;i<aut->n; i++) {
    v = aut->Delta[i];
    free(v);
  }
  free(aut->Delta);
  aut->n = aut->s = 0;
}



/*
  LeAFDTXT(nomearq, aut)
  Le o arquivo nomearq e armazena na estrutura aut
  Lay-out:
    A 1a linha deve conter o número de estados, numero de simbolos do alfabeto e indice do estado inicial.
    A 2a linha contem um vetor binario F:
         F[i] = 1 se estado i eh de aceitacao; F[i] = 0 caso contrario
    As proximas linhas contem os elementos da matriz de transicao:
    Delta[i][j] = estado ao qual o automato deve ir se estiver no estado i e ler o simbolo j
    Se nao houver transicao do estado i com o simbolo j, Delta[i][j] = -1
    Observações:
      - Estados e simbolos sao indexados de 0 a n-1 e 0 a s-1
      - Em cada linha, os simbolos sao separados por espacos

  Codigo de saida:
    1: leitura bem sucedida
    0: erro na leitura do arquivo
*/
int LeAFDTXT(char* nomearq, tAFD *aut) {
  FILE *fp;
  int i, j, n, s, q0;

  fp = fopen(nomearq, "rt");
  if (fp==NULL)
     return(0);

  if (fscanf(fp, "%u %u %u", &n, &s, &q0)!=3)
    return(0);

  InicializaAFD(aut, n, s);

  aut->n = n;
  aut->s = s;
  aut->q0 = q0;

  for (i=0;i<n;i++) {
    if (fscanf(fp, "%d", &(aut->F[i])) !=1 ) {
      LiberaAFD(aut);
      return(0);
    }
  }

  // for (i=0;i<n;i++) printf("elemento %d: %d\n", i, aut->F[i]);

  for (i=0;i<n; i++)
    for (j=0; j<s; j++) {
      if (fscanf(fp, "%d", &(aut->Delta[i][j])) != 1) {
        LiberaAFD(aut);
        return(0);
      }
    }

  fclose(fp);
  return(1);
}


/*
  LeAFDJFF(nomearq, aut)
  Le o automato no arquivo nomearq em formato JFlap
  Os rotulos originais dos estados sao ignorados, passando a ser identificados
  de 0 a n-1, na ordem em que aparecem.
  Idem para os simbolos; cada simbolo no AFD original deve ser identificado
  por apenas um caracter

  Codigo de saida:
    1: escrita bem sucedida
    0: erro na escrita do arquivo
*/
int LeAFDJFF(char* nomearq, tAFD *aut) {

  // funcao auxiliar para ordenacao dos simbolos do alfabeto
  int cmpfunc (const void *a, const void *b) {
    int valor =  **(char**)a - **(char**)b;
    return (valor);
  }

  FILE *fp;
  int i, j, estorig, estdest;
  int n=0;
  int s=0;
  int q0=-1;
  int F[MAX_AFD_SZ];   // vetor de estados finais (alocacao estatica)
  char S[MAX_AFD_SZ];  // vetor de simbolos do alfabeto
  char **PTR;          // vetor de ponteiros para os simbolos do alfabeto
  int Delta[MAX_AFD_SZ][MAX_AFD_SZ]; // matriz de transicao
  char *buffer;        // strig que conterah as linhas lidas
  char *ret1, *ret2;   // ponteiros para localizar as tagas especificas
  char aux[10];       // string auxiliar
  size_t characters;
  size_t bufsize=300;
  char para = 0;

  //const char strestado[] = "<state id=";
  //const char strfimestado[] = "</state>"
  //const char strtrans[] = "<transition>";
  //const char strfim[] = "</automaton>"

  // Alocacao do buffer
  buffer = (char *)malloc(bufsize * sizeof(char));

  // Inicializacao dos vetores de estados finais e de simbolos
  for (i=0; i<MAX_AFD_SZ; i++) F[i]=0;
  for (j=0; j<MAX_AFD_SZ; j++) S[j]=' ';
  for (i=0;i<MAX_AFD_SZ; i++) for (j=0; j<MAX_AFD_SZ; j++) Delta[i][j] = -1;

  fp = fopen(nomearq, "rt");
  if (fp==NULL)
     return(0);

  while (!feof(fp) && !para) {
    if ((characters = getline(&buffer, &bufsize, fp)) == -1) return(0);
    // printf("%s\n", buffer);

    // leitura de um estado
    if ( (ret1 = strstr(buffer, "<state id=")) != NULL) {
      n = n+1;
      do {
        if ( (characters = getline(&buffer,&bufsize,fp)) == -1) return(0);
        if ( (ret1 = strstr(buffer, "<initial/>")) != NULL) q0 = n-1;
        if ( (ret1 = strstr(buffer, "<final/>")) != NULL) F[n-1] = 1;
      } while ( (ret1 = strstr(buffer, "</state>")) == NULL);
    }

    // leitura de uma transicao
    if ( (ret1 = strstr(buffer, "<transition>")) != NULL) {
      if ( (characters = getline(&buffer,&bufsize,fp)) == -1) return(0);
      if (( (ret1 = strstr(buffer, "<from>")) != NULL) &&
          ( (ret2 = strstr(buffer, "</from>")) != NULL)) {
        ret1 = ret1+6;
        // printf("%s\n%s\n%d",ret1, ret2, (int)(ret2-ret1));
        strncpy(aux, ret1, (int)(ret2-ret1));
        // printf("%s\n", aux);
        estorig = (int) atoi(aux);
      } else return(0);
      if ( (characters = getline(&buffer,&bufsize,fp)) == -1) return(0);
      if (( (ret1 = strstr(buffer, "<to>")) != NULL) &&
          ( (ret2 = strstr(buffer, "</to>")) != NULL)) {
        ret1 = ret1+4;
        // printf("%s\n%s\n%d",ret1, ret2, (int)(ret2-ret1));
        strncpy(aux, ret1, (int) (ret2-ret1));
        // printf("%s\n", aux);
        estdest = (int) atoi(aux);
      } else return(0);
      if ( (characters = getline(&buffer,&bufsize,fp)) == -1) return(0);
      if ( (ret1 = strstr(buffer, "<read>")) != NULL) {
        ret1 = ret1+6;
        // printf("%s\n",ret1);
        for (j=0; j<s && S[j]!=*ret1; j++);
        if (j==s) {
          s++;
          S[j] = *ret1;
          // printf("%s\n", S);
        }
        Delta[estorig][j] = estdest;
      } else return(0);
    }

    // final da especificacao do automato
    if ( (ret1 = strstr(buffer, "</automaton>")) != NULL) para = 1;
  }

  // ordena os simbolos do alfabeto
  PTR = (char **)malloc(s * sizeof(char*));
  for (j=0; j<s; j++) PTR[j] = (char*)(S+j);
  // for (j=0; j<s; j++) printf("%p   ", S[j]); printf("\n");
  // for (j=0; j<s; j++) printf("%p   ", PTR[j]-S); printf("\n");

  qsort(PTR, s, sizeof(char*), cmpfunc);
  // for (j=0; j<s; j++) printf("%p   ", *PTR[j]); printf("\n");
  // for (j=0; j<s; j++) printf("%p   ", PTR[j]-S); printf("\n");

  InicializaAFD(aut, n, s);

  aut->n = n;
  aut->s = s;
  aut->q0 = q0;

  for (i=0; i<n; i++) aut->F[i] = F[i];
  for (i=0; i<n; i++) for (j=0; j<s; j++) aut->Delta[i][j] = Delta[i][PTR[j]-S];

  free(PTR);
  free(buffer);
  fclose(fp);
  return(1);
}




/*
  EscreveFD(nomearq, aut)
  Escreve o automato no arquivo nomearq
  Lay-out: identico aa funcao LeAFDTXT

  Codigo de saida:
    1: escrita bem sucedida
    0: erro na escrita do arquivo
*/
int EscreveAFDTXT(char* nomearq, tAFD *aut) {
  FILE *fp;
  int i, j;

  fp = fopen(nomearq, "wt");
  if (fp==NULL)
     return(0);

  fprintf(fp, "%d %d %d\n", aut->n,  aut->s, aut->q0);

  for (i=0;i<aut->n;i++)
    fprintf(fp, "%d ", aut->F[i]);
  fprintf(fp, "\n");

  for (i=0;i<aut->n; i++) {
    for (j=0; j<aut->s; j++)
      fprintf(fp, "%d ", aut->Delta[i][j]);
    fprintf(fp, "\n");
  }

  fclose(fp);
  return(1);
}


/*
  EscreveAFDJFF(nomearq, aut)
  Escreve o automato no arquivo nomearq em formato JFlap

  Codigo de saida:
    1: escrita bem sucedida
    0: erro na escrita do arquivo
*/
int EscreveAFDJFF(char* nomearq, tAFD *aut) {
  FILE *fp;
  int i, j;

  double raio, delta, alfa;

  delta = 2.0 * M_PI / (aut->n);
  raio = (double) aut->n * 120.0 / (2.0 * M_PI);

  fp = fopen(nomearq, "wt");
  if (fp==NULL)
     return(0);


  fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><!--Created with JFLAP 6.3.--><structure> \n");
  fprintf(fp, "	<type>fa</type> \n");
  fprintf(fp, "	<automaton> \n");
  fprintf(fp, "		<!--The list of states.--> \n");

  alfa = M_PI;
  for (i=0; i<aut->n; i++) {
    fprintf(fp, "		<state id=\"%d\" name=\"q%d\"> \n", i, i);
    fprintf(fp, "			<x>%f</x> \n", (double) (cos(alfa)*raio + 1.7*raio));
    fprintf(fp, "			<y>%f</y> \n", (double) (sin(alfa)*raio + 1.7*raio));
    if (aut->q0 == i)
      fprintf(fp, "			<initial/> \n");
    if (aut->F[i] == 1)
      fprintf(fp, "			<final/> \n");
    fprintf(fp, "		</state> \n");
    alfa = alfa + delta;
  }

  fprintf(fp, "		<!--The list of transitions.--> \n");

  for (i=0; i<aut->n; i++)
    for (j=0; j<aut->s; j++) {
      if (aut->Delta[i][j]>=0) {
        fprintf(fp, "		<transition> \n");
        fprintf(fp, "			<from>%d</from> \n", i);
        fprintf(fp, "			<to>%d</to> \n", aut->Delta[i][j]);
        fprintf(fp, "			<read>%d</read> \n", j);
        fprintf(fp, "		</transition> \n");
      }
    }

  fprintf(fp, "	</automaton> \n");
  fprintf(fp, "</structure> \n");

  fclose(fp);
  return(1);
}


/*
   DeterminaFormato(char *nomearq)
   Identifica o formato do arquivo com base em sua extensao:
     Se extensao for .jff: formato JFlap (XML)
     Caso contrario: formato texto do enunciado do EP.
*/
char DeterminaFormato(char *nomearq) {
  char aux[255];
  char *ret;
  char form;
  int i;

  strncpy(aux, nomearq, (int)strlen(nomearq)); aux[strlen(nomearq)]='\0';
  for (i=0; i<strlen(nomearq); i++) aux[i] = toupper(aux[i]);
  printf("%s \n", aux);
  if ((ret = strstr(aux, "JFF"))!=NULL) form = 'J'; else form = 'T';

  return(form);
}

/*
  Esta implementacao da funcao main serve exclusivamente para leitura e escrita de
  automatos, tanto no formato texto do enunciado do EP como no formato do JFlap.

  chamada:
     converte_afd <inp_afd> <out_afd>

  Os formatos dos arquivos de entrada e saida dependem das extensoes:
    Se extensao for .JFF: formato JFlap (XML)
    Qualquer outra extensao (ou sem extensao): formato texto:
        1a linha:   n  s  q0
        2a linha: vetor binario F indicando quais sao os estados finais
        3a linha em diante: matriz de transicao:
             Delta[i][j] = estado de destino dado o estado i e simbolo j
    Observacao: maiusculas e minusculas nao sao diferenciadas na extensao

  Exemplo:
    converte_afd <afd01.txt> <afd01.jff>  --> converte de TXT para JFF

*/
#ifdef _STANDALONE_
int main(int argc, char *argv[]) {
  tAFD aut;
  char inpformat, outformat;

  if (argc!=3) {
    printf("Chamada correta: \n");
    printf("   converte_afd <inp_afd> <out_afd> \n");
    printf("Se extensao for .jff, assume-se o formato do JFlap.\n");
    return(1);
  }

  inpformat = DeterminaFormato(argv[1]);
  outformat = DeterminaFormato(argv[2]);

  printf("%c  %c  \n", inpformat, outformat);

  if (inpformat=='J') LeAFDJFF(argv[1], &aut); else LeAFDTXT(argv[1], &aut);

  if (outformat=='J') EscreveAFDJFF(argv[2], &aut); else EscreveAFDTXT(argv[2], &aut);

  LiberaAFD (&aut);
  return(0);
}
#endif // _STANDALONE_

