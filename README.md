# ACH2043 - Introdução à Teoria da Computação
## Exercício-Programa: Minimização de AFDs
### Objetivo
Desenvolver um programa que minimize autômatos finitos determinísticos (AFDs) através da remoção de seus estados inacessíveis, inúteis e equivalentes. Seu programa deve receber um arquivo-texto contendo a especificação de um AFDs, e gerar um arquivo de saída contendo a especificação de um AFD equivalente minimizado.
Em outras palavras, dada uma especificação de um AFD, gere a especificação de um AFD minimizado quando remove se os estados inúteis e/ou a combinação de estados equivalentes.

Uma descrição do método de miminização de estados de AFDs e de sua implementação encontra-se em www.each.usp.br/ACH2043_2015/Minimizacao_AFD.pdf
  
Os slides são baseados (parcialmente) no livro do Prof. Marcus Ramos. Material adicional sobre minimização de estados encontram-se nos slides do livro, em http://www.univasf.edu.br/~marcus.ramos/livro-lfa/cap3.pdf
  
### Chamada do programa e especificações dos arquivos
A chamada do programa será nas formas:
<ul>
  <li>Implementacão em C
    <ul>
      <li>d[NUSP1]_[NUSP2].exe [FileInput] [FileOutput]</li>
    </ul>
  </li>
  <li>Implementacão em Java
    <ul>
      <li>java d[NUSP1]_[NUSP2] [FileInput] [FileOutput]</li>
    </ul>
</ul>
onde [NUSP1] e [NUSP2] são os números USP dos membros da dupla (ver Seção “Condições de Entrega”).

O arquivo [FileInput] deverá conter a descrição do AFD a ser minimizado, e estará organizado da seguinte forma:
<ul>
  <li>
    A primeira linha será um cabeçalho contendo os campos: n s q0, onde n é o número de estados, s é o número de símbolos do alfabeto e q0 é o estado inicial. Todos esses parâmetros são inteiros maiores ou iguais a zero.
  </li>
  <li>
    A segunda linha conterá um vetor binário F com n posições, onde F[i] = 1 se e somente se o estado i for de aceitação.
  </li>
  <li>
    As demais linhas corresponderão à matriz de transição Delta.
Essa matriz terá n linhas e s colunas, e a posição Delta[i, j] conterá o estado para o qual o autômato deverá ir, dado o estado i e o símbolo j.
  </li>
  <li>
    Quando não houver transição sobre o estado i com o símbolo j, Delta[i, j] = -1.
  </li>
</ul>
