#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <sys/time.h>
#ifdef _WIN32
    #include <time.h>
#else
    #include <sys/times.h>
    #include <sys/unistd.h>
#endif

#define MAX 2111         //Numero primo como escolha para o tamanho da tabela hash, porque foi recomendado que fosse :p
#define TAM_PALAVRA 46       //Maior palavra da lingua portuguesa tem 46 letras (pneumoultramicroscopicossilicovulcanoconiótico)

typedef struct {
    char Chave[TAM_PALAVRA];
    int contador;
}Registro;

typedef int Indice;

typedef struct {
    Registro Item[MAX + 1];
    Indice n;
}Tabela;

typedef Registro TipoDicionario[MAX];

typedef unsigned TipoPesos[TAM_PALAVRA];

void inicializaSeq (Tabela *T) {
    T->n = 0;
}

Indice pesquisaSeq (char *palavra, Tabela *T) {
    int i;

    strcpy(T->Item[0].Chave, palavra);
    i = T->n + 1;

    do {
        i--;
    } while (strcmp(T->Item[i].Chave, palavra));

    return i;
}

void insereSeq (char *palavra, Tabela *T) {
    Registro x;

    if (T->n == MAX)
        printf("Erro : tabela cheia\n");
    else {
        strcpy(x.Chave, palavra);
        x.contador = 1;
        T->n++;
        T->Item[T->n] = x;
    }
}

void imprimeSeq(Tabela *T, FILE *arquivo) {
    int i = 1;

    while (i <= T->n){
        fprintf(arquivo, "%s  %d\n", T->Item[i].Chave, T->Item[i].contador);
        i++;
    }
}

void inicializaHash(TipoDicionario T){
    int i;

    for (i = 0; i < MAX; i++)
        T[i].contador = 0;
}

void geraPesos(TipoPesos p) {
    /* -Gera valores randomicos entre 1 e 10.000- */
    int i;
    struct timeval semente;

    /* Utilizar o tempo como semente para a funcao srand() */
    gettimeofday(&semente,NULL);
    srand((int)(semente.tv_sec + 1000000 * semente.tv_usec));

    for (i = 0; i < TAM_PALAVRA; i++)
        p[i] = 1 + (int) (10000.0 * rand()/(RAND_MAX + 1.0));
}

Indice h(char *Chave, TipoPesos p) {
    int i;
    unsigned int Soma = 0;
    int comp = strlen(Chave);

    for (i = 0; i < comp; i++)
        Soma += (unsigned int)Chave[i] * p[i];

    return (Soma % MAX);
}

Indice pesquisaHash(char *palavra, TipoPesos p, TipoDicionario T){
    unsigned int  i = 0;
    unsigned int  Inicial;
    Inicial = h(palavra, p);

    while ((T[(Inicial + i) % MAX].contador != 0 &&
        strcmp (T[(Inicial + i) % MAX].Chave, palavra) != 0 && i < MAX))
        i++;

    if (strcmp( T[(Inicial + i) % MAX].Chave, palavra) == 0)
        return ((Inicial + i) % MAX);
    else
        return MAX;  /* Pesquisa sem sucesso */
}


 void insereHash(char *palavra, TipoPesos p, TipoDicionario T){
    unsigned int i = 0;
    unsigned int Inicial;
    Registro x;
    strcpy(x.Chave, palavra);

    Inicial = h(x.Chave, p);
    while (T[(Inicial + i) % MAX].contador != 0)
        i++;

    if (i < MAX) {
        strcpy(T[(Inicial + i) % MAX].Chave, x.Chave);
        T[(Inicial + i) % MAX].contador = 1;
    }
    else
        printf("Tabela cheia\n");
}

void imprimeHash(TipoDicionario T, FILE *arquivo) {
    int i;

    for (i = 0; i < MAX; i++) {
        if (T[i].contador != 0)
            fprintf(arquivo, "%s  %d\n", T[i].Chave, T[i].contador);
    }
}

void insertionSort (Registro* v, int n) {
    int i,j;
    Registro aux;

    for (i = 1; i < n; i++) {
        aux = v[i];
        j = i - 1;
        while (( j >= 0 ) && (strcmp(aux.Chave, v[j].Chave) < 0)) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = aux;
    }
}

void Particao(int Esq, int Dir, int *i, int *j, Registro *A) {
    *i = Esq;
    *j = Dir;
    Registro x = A[(*i + *j)/2]; // obtem o pivo x

    do {
        while (strcmp(A[*i].Chave, x.Chave) < 0) (*i)++;
        while (strcmp(A[*j].Chave, x.Chave) > 0) (*j)--;

        if (*i <= *j) {
            Registro w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;
            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void Ordena(int Esq, int Dir, Registro *A){
    int i,j;
    Particao(Esq, Dir, &i, &j, A);

    if (Esq < j) Ordena(Esq, j, A);
    if (i < Dir) Ordena(i, Dir, A);
}

void quickSort(Registro *A, int n){
    Ordena(0, n-1, A);
}

int validaChar (char *c) {

    if (ispunct((unsigned char)*c) || *c == ' ' || *c == EOF || *c == '\n')
        return 0;
    else if (isupper((unsigned char)*c))
        *c = tolower((unsigned char)*c);

    return 1;
}

int lePalavra (FILE *arquivo, char *palavra) {
    char c = fgetc(arquivo);
    int i = 0;

    while (validaChar(&c)){
        palavra[i] = c;
        i++;
        c = fgetc(arquivo);
    }

    if (c == EOF)
        return 0;
    else{
        palavra[i] = '\0';
        return 1;
    }
}

float getTime() {
#ifdef _WIN32
    clock_t start = clock();
    return (float)start / CLOCKS_PER_SEC;
#else
    struct tms tempo;
    clock_t tempotic;
    float cpt = sysconf(_SC_CLK_TCK);
    times(&tempo);
    tempotic = tempo.tms_utime ;
    return ( (float) (tempotic/cpt) );
#endif
}



int main(int argc, char *argv[]){
    TipoDicionario Dicionario;
    TipoPesos Pesos;
    Tabela TabSeq;
    FILE *arquivoSource, *arquivoDestino;
    char *palavra = (char*)calloc(TAM_PALAVRA, sizeof(char));
    char nomeArqSrc[30], nomeArqDst[30];
    int indice;
    float tempoInicial = getTime();

    strcpy(nomeArqSrc, argv[1]);
    strcpy(nomeArqDst, argv[4]);

    arquivoSource = fopen(nomeArqSrc, "r");
    arquivoDestino = fopen(nomeArqDst, "w");

    if (arquivoSource == NULL) {
        printf("Erro na abertura do arquivo\n");
        system("pause");
        exit(1);
    }

    if (atoi(argv[2]) == 0){
        inicializaSeq(&TabSeq);
        while(lePalavra(arquivoSource, palavra)) {           //Enquanto o programa conseguir ler palavras
            indice = pesquisaSeq(palavra, &TabSeq);
            if (palavra[0] != '\0'){                    //Esse caso só acontece quando o programa lê dois caracteres invalidos (segundo a funçao validaChar) seguidos
                if (indice == 0)                          //Nao achou a palavra na tabela
                    insereSeq(palavra, &TabSeq);
                else                                      //Achou a palavra na tabela
                    TabSeq.Item[indice].contador++;
            }
        }
        if (atoi(argv[3]) == 0)
            insertionSort(TabSeq.Item, TabSeq.n + 1);
        else
            quickSort(TabSeq.Item, TabSeq.n + 1);
        imprimeSeq(&TabSeq, arquivoDestino);
    }

    else if (atoi(argv[2]) == 1){
        inicializaHash(Dicionario);
        geraPesos(Pesos);
        while(lePalavra(arquivoSource, palavra)) {
            indice = pesquisaHash(palavra, Pesos, Dicionario);
            if (palavra[0] != '\0'){
                if (indice == MAX)                               //Nao achou a palavra na tabela
                    insereHash(palavra, Pesos, Dicionario);
                else                                             //Achou a palavra
                    Dicionario[indice].contador++;
            }
        }
        if (atoi(argv[3]) == 0)
            insertionSort(Dicionario, MAX);
        else
            quickSort(Dicionario, MAX);
        imprimeHash(Dicionario, arquivoDestino);
    }

    fprintf(arquivoDestino, "\nTEMPO DE EXECUÇÃO: %.5fs\n", getTime() - tempoInicial);             //Calcula o tempo de execução

    free(palavra);
    fclose(arquivoSource);
    fclose(arquivoDestino);
    return 0;
}
