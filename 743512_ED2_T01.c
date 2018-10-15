/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 743512
 * Aluno: Bianca Gomes Rodrigues
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int * nregistros);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char * registro, Produto *novo);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

/*-----------------------*/

/* Índice Simples */
void Criar_iProduct (Is* iProduct, int * nRegistros);

void Criar_iBrand (Is* iBrand, int * nRegistros);

void Criar_iPrice (Isf *iPrice,  int * nRegistros);

/* Lista Invertida */
// void Criar_iCategory ();

void gerarChave(Produto * Novo);

void Inserir(Produto*Novo);

/*-----------------------*/


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/*----------*/
	// printf("%d\n", nregistros);
	/*----------*/


	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	criar_iprimary(iprimary, &nregistros);

	/*Alocar e criar índices secundários*/

	/* Alocando VARIÁVEIS que NÃO foram previamente alocadas */

	Is* iproduct = (Is*) malloc (MAX_REGISTROS * sizeof(Is)); 
	if (!iproduct) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	Is* ibrand = (Is*) malloc (MAX_REGISTROS * sizeof(Is));
  	if (!ibrand) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	Ir* icategory = (Ir*) malloc (MAX_REGISTROS * sizeof(Ir));
	if (!icategory) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	
	Isf *iprice = (Isf*) malloc (MAX_REGISTROS * sizeof(Isf));
	if (!iprice) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	Produto Novo;

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				ler_entrada(0, &Novo);
				nregistros++;
				criar_iprimary(iprimary, &nregistros);
				Criar_iProduct(iproduct, &nregistros);
				Criar_iBrand(ibrand, &nregistros);
				Criar_iPrice(iprice, &nregistros);
		
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);
				/*
				if(alterar([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);
				/*
				if(remover([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA );
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
			break;
			case 6:
				/*libera espaço*/
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */
				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  	char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%07.2f\n",  preco);

	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}


int carregar_arquivo()
{
	// scanf("%[^\n]\n", ARQUIVO);

    scanf("%[^\n]s", ARQUIVO);
	
    return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	gerarChave(&j);
	return j;
}

/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if(!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}

/* ---------------------------------------------- */
void gerarChave(Produto * Novo){

	Novo->pk[0] = Novo->nome[0];
	Novo->pk[1] = Novo->nome[1];
	Novo->pk[2] = Novo->marca[0];
	Novo->pk[3] = Novo->marca[1];
	Novo->pk[4] = Novo->data[0];
	Novo->pk[5] = Novo->data[1];
	Novo->pk[6] = Novo->data[3];
	Novo->pk[7] = Novo->data[4];
	Novo->pk[8] = Novo->ano[0];
	Novo->pk[9] = Novo->ano[1]; 

}

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int * nregistros){

	/*	A chave primária IPRIMARY será criada a partir da junção das duas primeiras letras do NOME DO PRODUTO(MARCA), das duas primeiras letras da MARCA, do dia e do mês da DATA DE REGISTRO e do ANO de LANÇAMENTO.
	Para que seja possível gerar a chave primária, precisamos demarcar os campos, para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i< (*nregistros); i++){

		indice_primario[i].rrn = i; 
		
		strcpy(indice_primario[i].pk, recuperar_registro(i).pk);	
	}

}

void Criar_iProduct (Is* iProduct, int * nRegistros ){

	/* O índice secundário IPRODUCT contém a CHAVE PRIMÁRIA (pk) e o NOME do PRODUTO do REGISTRO. Para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i < (*nRegistros); i++){
		
		strcpy(iProduct[i].pk, recuperar_registro(i).pk);

		strcpy(iProduct[i].string, recuperar_registro(i).nome); 

	}
}

void Criar_iBrand (Is* iBrand, int * nRegistros){

	/* O índice secundário IBRAND contém a CHAVE PRIMÁRIA (pk) e a MARCA do REGISTRO. Para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i < (*nRegistros); i++){

		strcpy(iBrand[i].pk, recuperar_registro(i).pk);

		strcpy(iBrand[i].string, recuperar_registro(i).marca);
	
	}
}

void Criar_iPrice (Isf *iPrice, int * nRegistros){

	/* O índice secundário ICATEGORY contém a CHAVE PRIMÁRIA (pk) e o PREÇO FINAL, ou seja, PREÇO COM DESCONTO.  Para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i < (*nRegistros); i++){

		strcpy(iPrice[i].pk, recuperar_registro(i).pk);

		/*Preço Original do Produto*/
		float Preco = atof(recuperar_registro(i).preco);

		// printf("Preço Original: %f\n", Preco );
				
		/* Desconto em Porcentagem*/
		float Desconto = atof(recuperar_registro(i).desconto);
		
		// printf("Desconto: %f\n", Desconto);
	
		Desconto/=100;

		// printf("Desconto: %f\n", Desconto);

		/* Valor do Desconto*/
		float Valor_Desconto;
		Valor_Desconto = Preco * Desconto;

		// printf("Valor do Desconto: %f\n", Valor_Desconto);

		/*Aplicando o Valor do Desconto*/
		Preco-=Valor_Desconto;

		// printf("Preço: %f\n", Preco);


		iPrice[i].price = Preco;
	
	}
	 

}

/* Realiza os scanfs na struct Produto */
void ler_entrada(char * registro, Produto *novo){

	//Código - NÃO é inserido pelo usuário 
	// char pk[TAM_PRIMARY_KEY];
	// gerarChave(novo);

	/*-----------------------*/

	/* Interação com o Usuário */

	/* CAMPOS DE TAMANHO VARIÁVEL */
	
	//Nome do Produto ou Modelo
	// char Nome[TAM_NOME];
	
	scanf("%[^\n]s", novo->nome);
	getchar();
	//Marca
	// char Marca[TAM_MARCA];
	scanf("%[^\n]s", novo->marca);
	getchar();
	/*-----------------------*/

	/* CAMPOS DE TAMANHO FIXO */

	//Data de Registro
	// char Data[TAM_DATA];	/* DD/MM/AAAA */
	scanf("%[^\n]s", novo->data);
	getchar();
	//Ano de Lançamento
	// char Ano[TAM_ANO];
	scanf("%[^\n]s", novo->ano);
	getchar();
	//Preço-Base
	// char Preço[TAM_PRECO];
	scanf("%[^\n]s", novo->preco);
	getchar();
	//Desconto
	// char Desconto[TAM_DESCONTO];
	scanf("%[^\n]s", novo->desconto);
	getchar();
	//Categorias
	// char Categoria[TAM_CATEGORIA];
	scanf("%[^\n]s", novo->categoria);
	getchar();
	/*-----------------------*/

		
	Inserir(novo);
}

void Inserir(Produto * Novo){

	//Registro Auxiliar
	char rAuxiliar[TAM_REGISTRO];

	sprintf(rAuxiliar, "%s@%s@%s@%s@%s@%s@%s@", Novo->nome, Novo->marca,Novo->data, Novo->ano,Novo->preco, Novo->desconto, Novo->categoria);

	//Precisamos obter o TAMANHO do REGISTRO AUXILIAR (rAuxiliar) para sabermos quantos "bytes" faltam para preencher totalmento o REGISTRO.
	int Tamanho = strlen(rAuxiliar);

	// printf("\n Tamanho = %d\n", Tamanho);

	//Preenchendo o REGISTRO por completo (192bytes)
	for(int i = Tamanho; Tamanho < TAM_REGISTRO-2; i++)
		rAuxiliar[i] = '#';

	printf("%d", strlen(rAuxiliar));
	// printf("\n Registro: %s \n", rAuxiliar);

	strcat(ARQUIVO, rAuxiliar);

	// printf("\n%s\n", ARQUIVO);

}

/* ---------------------------------------------- */
