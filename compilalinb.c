#include "header.h"
#include <stdio.h>

typedef int (*funcp)();
void escreveInt(int n, unsigned char codigo[], int *index);

void compara(char var0, int idx0, int idx1, unsigned char codigo[], int *index);
void calculaDeslocamento(int linhaAtual, int parsedLines[], int num, unsigned char codigo[], int *index);

void soma(char var0, int idx0, char var1, int idx1, char var2, int idx2,
          unsigned char codigo[], int *index);
void sub(char var0, int idx0, char var1, int idx1, char var2, int idx2,
         unsigned char codigo[], int *index);
void mult(char var0, int idx0, char var1, int idx1, char var2, int idx2,
          unsigned char codigo[], int *index);
void sublEcx(char c, int n, unsigned char codigo[], int *index);
void addlEcx(char c, int n, unsigned char codigo[], int *index);
void imullEcx(char c, int n, unsigned char codigo[], int *index);
void atribui(char var0, int idx0, unsigned char codigo[], int *index);
// multvar

funcp CompilaLinB(FILE *f, unsigned char codigo[]) {
  int index = 0;
  int line = 1;
  int isParsing = 0;
  int lastIndex = 0;
  int parsedLines[50];
  parsedLines[0] = 0;


  while (!feof(f)) {
    if (isParsing == 1) {
      // Inicializacao da pilha
      //  pushq rbp
      codigo[++index] = 0x55;

      // movq %rsp, %rbp
      codigo[++index] = 0x48;
      codigo[++index] = 0x89;
      codigo[++index] = 0xe5;

      // subq $32, %rsp
      codigo[++index] = 0x48;
      codigo[++index] = 0x83;
      codigo[++index] = 0xec;
      codigo[++index] = 0x20;
      isParsing++;
    }
    char c = fgetc(f);
    switch (c) {
    case 'p':
    case 'v': {
      int idx0, idx1, idx2;
      char var0 = c, var1, var2, op;
      if (fscanf(f, "%d <= %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2,
                 &idx2) == 6) {
        switch (op) {
        case '+':
          soma(var0, idx0, var1, idx1, var2, idx2, codigo, &index);
          break;
        case '-':
          sub(var0, idx0, var1, idx1, var2, idx2, codigo, &index);
          break;
        case '*':
          mult(var0, idx0, var1, idx1, var2, idx2, codigo, &index);
          // mult
          break;
        default:
          break;
        }
      }
    }

    break;
    case 'r':
      // movl V1 pra eax
      codigo[++index] = 0x8b;
      codigo[++index] = 0x45;
      codigo[++index] = 0xf8;
      // leave e ret
      codigo[++index] = 0xc9;
      codigo[++index] = 0xc3;
      fscanf(f, "et");
      break;
    case 'i': {
      int idx0, idx1;
      char var0;

      if (fscanf(f, "f %c%d %d", &var0, &idx0, &idx1) == 3) {
        compara(var0, idx0, idx1, codigo, &index);
        if (isParsing == 0) {
          codigo[++index] = 0x00;
          codigo[++index] = 0x00;
          codigo[++index] = 0x00;
          codigo[++index] = 0x00;
        }
        else {
            calculaDeslocamento(line, parsedLines, idx1, codigo, &index);

        }
      }
      break;
    }
    default:
      break;
    }

    if (isParsing == 0) {

      parsedLines[line] = index;

      lastIndex = index;
      line++;
      fscanf(f, " ");

      // checa se já chegou no final do arquivo
      if (feof(f)) {
        isParsing++;
        index = lastIndex = -1;

        line = 1;

        rewind(f);
      }
    } else {
      lastIndex = index;
      line++;
      fscanf(f, " ");
    }
  }

  printf("Codigo = { ");
  for (int i = 0; i < +index; i++) {
    printf("%02x ", codigo[i]);
  }
  printf("}\n");

  printf("Num linhas: %d\n", line);
  printf("Linhas = { ");

  for (int i = 1; i < line; i++) {

    printf("%d ", parsedLines[i]);
  }
  printf("}\n");

  funcp foda = (funcp)codigo;
  return foda;
}

void movlEcx(char c, int n, unsigned char codigo[], int *index) {
  int index2 = *index;

  switch (c) {
  case '$':
    codigo[++(index2)] = 0xb9; // movl pra exc
    int numBytes = sizeof(int);
    for (int i = 0; i < numBytes; i++) {
      codigo[++(index2)] = (n >> (i * 8)) & 0xFF;
    }
    break;
  case 'v':
    switch (n) {
    case 1:
      codigo[++(index2)] = 0x8b;
      codigo[++(index2)] = 0x4d;
      codigo[++(index2)] = 0xf8;
      break;
    case 2:
      codigo[++(index2)] = 0x8b;
      codigo[++(index2)] = 0x4d;
      codigo[++(index2)] = 0xf0;
      break;
    case 3:
      codigo[++(index2)] = 0x8b;
      codigo[++(index2)] = 0x4d;
      codigo[++(index2)] = 0xe8;
      break;
    case 4:
      codigo[++(index2)] = 0x8b;
      codigo[++(index2)] = 0x4d;
      codigo[++(index2)] = 0xe0;
      break;
    default:
      break;
    }
    break;
  case 'p':
    codigo[++(index2)] = 0x89;
    if (n == 1) {
      // movl edi pra ecx
      codigo[++(index2)] = 0xf9;
    } else {
      // movl esi pra ecx
      codigo[++(index2)] = 0xf1;
    }
  }

  *index = index2;
}

void addlEcx(char c, int n, unsigned char codigo[], int *index) {
  int index2 = *index;
  switch (c) {
  case '$':
    if (n >= -128 && n <= 128) {
      codigo[++(index2)] = 0x83; // addl constante
      codigo[++(index2)] = 0xc1;
      unsigned char hex = (unsigned char)n;
      codigo[++(index2)] = hex;
    } else {
      codigo[++(index2)] = 0x81; // addl constante
      codigo[++(index2)] = 0xc1;
      int numBytes = sizeof(int);
      for (int i = 0; i < numBytes; i++) {
        codigo[++(index2)] = (n >> (i * 8)) & 0xFF;
      }
    }
    break;
  case 'v':
    codigo[++(index2)] = 0x03;
    codigo[++(index2)] = 0x4d;

    if (n == 1)
      codigo[++(index2)] = 0xf8;
    else if (n == 2)
      codigo[++(index2)] = 0xf0;
    else if (n == 3)
      codigo[++(index2)] = 0xe8;
    else if (n == 4)
      codigo[++(index2)] = 0xe0;
    break;
  case 'p':
    codigo[++(index2)] = 0x01;
    if (n == 1)
      codigo[++(index2)] = 0xf9;
    else
      codigo[++(index2)] = 0xf1;
    break;
  default:
    break;
  }
  *index = index2;
}

void sublEcx(char c, int n, unsigned char codigo[], int *index) {
  int index2 = *index;
  switch (c) {
  case '$':
    if (n >= -128 && n <= 128) {
      codigo[++(index2)] = 0x83; // subl constante 1byte
      codigo[++(index2)] = 0xe9;
      unsigned char hex = (unsigned char)n;
      codigo[++(index2)] = hex;
    } else {
      codigo[++(index2)] = 0x81; // subl constante +4byte
      codigo[++(index2)] = 0xe9;
      int numBytes = sizeof(int);
      for (int i = 0; i < numBytes; i++) {
        codigo[++(index2)] = (n >> (i * 8)) & 0xFF;
      }
    }
    break;
  case 'v':
    codigo[++(index2)] = 0x2b;
    codigo[++(index2)] = 0x4d;

    if (n == 1)
      codigo[++(index2)] = 0xf8;
    else if (n == 2)
      codigo[++(index2)] = 0xf0;
    else if (n == 3)
      codigo[++(index2)] = 0xe8;
    else if (n == 4)
      codigo[++(index2)] = 0xe0;
    break;
  case 'p':
    codigo[++(index2)] = 0x29;
    if (n == 1)
      codigo[++(index2)] = 0xf9;
    else
      codigo[++(index2)] = 0xf1;
    break;
  default:
    break;
  }
  *index = index2;
}

void imullEcx(char c, int n, unsigned char codigo[], int *index) {
  int index2 = *index;
  switch (c) {
  case '$':
    if (n >= -128 && n <= 128) {
      codigo[++(index2)] = 0x6b; // imull constante 1byte
      codigo[++(index2)] = 0xc9;
      unsigned char hex = (unsigned char)n;
      codigo[++(index2)] = hex;
    } else {
      codigo[++(index2)] = 0x69; // imull constante +4byte
      codigo[++(index2)] = 0xc9;
      int numBytes = sizeof(int);
      for (int i = 0; i < numBytes; i++) {
        codigo[++(index2)] = (n >> (i * 8)) & 0xFF;
      }
    }
    break;
  case 'v':
    codigo[++(index2)] = 0x0f;
    codigo[++(index2)] = 0xaf;
    codigo[++(index2)] = 0x4d;

    if (n == 1)
      codigo[++(index2)] = 0xf8;
    else if (n == 2)
      codigo[++(index2)] = 0xf0;
    else if (n == 3)
      codigo[++(index2)] = 0xe8;
    else if (n == 4)
      codigo[++(index2)] = 0xe0;
    break;
  case 'p':
    codigo[++(index2)] = 0x0f;
    codigo[++(index2)] = 0xaf;
    if (n == 1)
      codigo[++(index2)] = 0xcf;
    else
      codigo[++(index2)] = 0xce;
    break;
  default:
    break;
  }
  *index = index2;
}

void atribui(char var0, int idx0, unsigned char codigo[], int *index) {
  int index2 = *index;
  if (var0 == 'v') {
    // Escrever o inicio de movl de ecx pra pilha
    codigo[++(index2)] = 0x89;
    codigo[++(index2)] = 0x4d;
    switch (idx0) // Pra qual variavel estatica sera atribuido?
    {
    case 1:
      // movl %ecx, -8(%rbp)
      codigo[++(index2)] = 0xf8;
      break;
    case 2:
      // movl %ecx, -16(%rbp)
      codigo[++(index2)] = 0xf0;
      break;
    case 3:
      // movl %ecx, -24(%rbp)
      codigo[++(index2)] = 0xe8;
      break;
    case 4:
      // movl %ecx, -32(%rbp)
      codigo[++(index2)] = 0xe0;
      break;
    }
  } else {
    // Atribui pra parametro
    codigo[++(index2)] = 0x89;
    if (idx0 == 1) {
      codigo[++(index2)] = 0xcf;
    } else {
      codigo[++(index2)] = 0xce;
    }
  }
  *index = index2;
}

void soma(char var0, int idx0, char var1, int idx1, char var2, int idx2,
          unsigned char codigo[], int *index) {

  // Movl primeiro parametro pra %ecx
  movlEcx(var1, idx1, codigo, index);

  // soma segundo parametro ao ecx
  addlEcx(var2, idx2, codigo, index);

  // Escrever o inicio de movl pra pilha
  atribui(var0, idx0, codigo, index);
}

void mult(char var0, int idx0, char var1, int idx1, char var2, int idx2,
          unsigned char codigo[], int *index) {

  // Movl primeiro parametro pra %ecx
  movlEcx(var1, idx1, codigo, index);

  // soma segundo parametro ao ecx
  imullEcx(var2, idx2, codigo, index);

  // Escrever o inicio de movl pra pilha
  atribui(var0, idx0, codigo, index);
}

void sub(char var0, int idx0, char var1, int idx1, char var2, int idx2,
         unsigned char codigo[], int *index) {
  // Movl primeiro parametro pra %ecx
  movlEcx(var1, idx1, codigo, index);

  // sub segundo parametro ao ecx
  sublEcx(var2, idx2, codigo, index);

  // Escrever o inicio de movl pra pilha
  atribui(var0, idx0, codigo, index);
}

void compara(char var0, int idx0, int idx1, unsigned char codigo[], int *index){
  int index2 = *index;

  // Código inicial
  codigo[++(index2)] = 0x83;
  if (var0 == 'v'){

    // Códigos iniciais
    codigo[++(index2)] = 0x7c;
    codigo[++(index2)] = 0x24;

    switch (idx0) // Pra qual variavel estatica sera atribuido?
    {
    case 1:
      // movl %ecx, -8(%rbp)
      codigo[++(index2)] = 0xf8;
      break;
    case 2:
      // movl %ecx, -16(%rbp)
      codigo[++(index2)] = 0xf0;
      break;
    case 3:
      // movl %ecx, -24(%rbp)
      codigo[++(index2)] = 0xe8;
      break;
    case 4:
      // movl %ecx, -32(%rbp)
      codigo[++(index2)] = 0xe0;
      break;
    }
  }
  else {
    if (idx0 == 1) codigo[++(index2)] = 0xff;
    else codigo[++(index2)] = 0xfe;
  }

  codigo[++(index2)] = 0x00;
  codigo[++(index2)] = 0x0f;
  codigo[++(index2)] = 0x85;


  *index = index2;
}


void calculaDeslocamento(int linhaAtual, int parsedLines[], int num, unsigned char codigo[], int *index) {
  int index2 = *index;
  int deslocamento;

  deslocamento = parsedLines[num - 1] - parsedLines[linhaAtual];
  printf("Line: %d\n", linhaAtual);
  printf("Linha atual: %d\n", parsedLines[linhaAtual + 1]);
  printf("Linha final: %d\n", parsedLines[num - 1]);
  printf("Deslocamento: %d\n", deslocamento);

  int numBytes = sizeof(int);
  for (int i = 0; i < numBytes; i++) {
    codigo[++(index2)] = (deslocamento >> (i * 8)) & 0xFF;
  }
 // unsigned char hex = (unsigned char) deslocamento;
 // codigo[++(index2)] = hex;

  *index = index2;
}


