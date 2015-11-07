/************************************
	ACH2043 - Introducao a Teoria da Computacao
	Prof. Marcelo de Souza Lauretto

	Estrutura de dados e funcoes sobre AFDs
	Adaptacao de:
	RAMOS, M. V. M.; NETO, J. J.; VEGA, I. S. Linguagens Formais. Ed. Bookman

*************************************/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include <errno.h>
#include <ctype.h>


/*
  tipo estruturado AFD:
    armazena: numero de estados, numero de simbolos, simbolo inicial,
              vetor indicador de estados finais e matriz de transicoes
*/
typedef struct {
  int n;   // numero de estados
  int s;   // numero de simbolos do alfabeto
  int q0;  // estado inicial
  int *F;  // vetor binario indicando os estados de aceitacao
  int **Delta; // matriz que representarah a funcao de transicao
}tAFD;


/* ***********************************
   As duas funcoes auxiliares abaixo implementam a funcao getline, que nao eh padrao em C.
   Foram obtidas atraves do forum:
     http://stackoverflow.com/questions/27369580/codeblocks-and-c-undefined-reference-to-getline
     author: BLUEPIXY
*/
ssize_t getdelim(char **linep, size_t *n, int delim, FILE *fp);
ssize_t getline(char **linep, size_t *n, FILE *fp);
/* **************************************************** */


/*
  InicializaAFD(AFD, n, s):
  Inicializa a estrutura para um AFD com n estados e s simbolos
  Aloca espaco para o vetor binario de estados finais e para a matriz de transicao
*/
void InicializaAFD(tAFD *aut, int n, int s);


/*
  LiberaAFD(AFD): Liberar o espaco ocupado por um AFD.
*/
void LiberaAFD (tAFD *aut);



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
int LeAFDTXT(char* nomearq, tAFD *aut);


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
int LeAFDJFF(char* nomearq, tAFD *aut);


/*
  EscreveAFDTXT(nomearq, aut)
  Escreve o automato no arquivo nomearq
  Lay-out: identico aa funcao LeAFDTXT

  Codigo de saida:
    1: escrita bem sucedida
    0: erro na escrita do arquivo
*/
int EscreveAFDTXT(char* nomearq, tAFD *aut);


/*
  EscreveAFDJFF(nomearq, aut)
  Escreve o automato no arquivo nomearq em formato JFlap

  Codigo de saida:
    1: escrita bem sucedida
    0: erro na escrita do arquivo
*/
int EscreveAFDJFF(char* nomearq, tAFD *aut);


char DeterminaFormato(char *nomearq);
