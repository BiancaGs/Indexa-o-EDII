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
void Criar_iPrimary(Ip *indice_primario, int * nregistros);

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

void gerarChave(Produto* Novo);

void Inserir(Produto* Novo, Ip *iPrimary, Is* iProduct, Is* iBrand, Ir* iCategory, Isf *iPrice, int * nRegistros, int nCat);

/* Realiza três tipos de BUSCA, por CÓDIGO, NOME DO PRODUTO ou MODELO e por NOME DA MARCA e CATEGORIA.*/
void Busca_Produto(Ip* iPrimary, Is* iProduct, Is* iBrand, Ir* iCategory, int* nRegistros, int nCat);

void Listar(Ip* iPrimary, Is* iProduct, Is* iBrand, Ir* iCategory, int* nRegistros, int nCat);



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

	Criar_iPrimary(iprimary, &nregistros);

	/*-----------------------*/

	/*Alocar e criar índices secundários*/

	/* Alocando VARIÁVEIS que NÃO foram previamente alocadas */

	Is* iproduct = (Is*) malloc (MAX_REGISTROS * sizeof(Is)); 
	if (!iproduct) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	Criar_iProduct(iproduct, &nregistros);

	Is* ibrand = (Is*) malloc (MAX_REGISTROS * sizeof(Is));
  	if (!ibrand) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	Criar_iBrand(ibrand, &nregistros);
	
	Ir* icategory = (Ir*) malloc (MAX_REGISTROS * sizeof(Ir));
	if (!icategory) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	//Criar_iCategory(icategory, &nregistros);
	
	Isf *iprice = (Isf*) malloc (MAX_REGISTROS * sizeof(Isf));
	if (!iprice) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	Criar_iPrice(iprice, &nregistros);

	/*-----------------------*/

	Produto Novo;

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				//ler_entrada(0, &Novo);
				Inserir(&Novo, iprimary, iproduct, ibrand, icategory, iprice, &nregistros, ncat );
				//Criar_iPrimary(iprimary, &nregistros);
				//Criar_iProduct(iproduct, &nregistros);
				//Criar_iBrand(ibrand, &nregistros);
				//Criar_iPrice(iprice, &nregistros);
		
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
				Busca_Produto(iprimary, iproduct, ibrand, icategory, &nregistros,  ncat);
				
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
				Listar(iprimary, iproduct, ibrand, icategory, &nregistros,  ncat);
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
	Novo->pk[10] = '\0';

}

/*A função responsável por COMPARAR as informações para a função QSORT, por definição recebe dois CONST VOID */
int Compara_iPrimary (const void * pCodigo, const void * sCodigo){

	/* A ORDENAÇÃO do iPrimary será realizado pelo CÓDIGO (ÍNDICE PRIMÁRIO)*/

	/* Existem três possíveis RESULTADOS(int) que a função STRCMP( , ) pode retornar:
		<0 - Indica que o PRIMEIRO parâmetro é MENOR que SEGUNDO
		0  - Indica que os parâmentros são IGUAIS
		>0 - Indice que o PRIMEIRO parâmentro é MAIOR que o SEGUNDO parâmentro
	
		Além disso, é importante ressaltar que função receber dois parâmentro CONST CHAR*
	*/

	/* Por definição estamos recebendo CONST VOID *, então precisamos realizar a conversão, para por conseguinte podermos acessar o dados que desejamos*/

	// printf("\nCOMPARAÇAO\n");

	/* Primeiro Código*/
	// char pCod[TAM_PRIMARY_KEY];
	// strcpy(pCod,(*(Ip *)pCodigo).pk);

	// // printf("\nPrimeiro Código: %s\n", pCod);
	
	// /*Segundo Código*/
	// char sCod[TAM_PRIMARY_KEY];
	// strcpy(sCod, (*(Ip*)sCodigo).pk);

	// printf("\nSegundo Código: %s\n", sCod);

	// int Resultado = strcmp(pCod, sCod);

	int Resultado = strcmp((*(Ip *)pCodigo).pk,  (*(Ip*)sCodigo).pk);
	// printf("Resultado (Comparaçao): %d\n");
	
	return Resultado;
	
}



/* (Re)faz o índice respectivo */
void Criar_iPrimary(Ip *indice_primario, int * nregistros){

	/*	A chave primária IPRIMARY será criada a partir da junção das duas primeiras letras do NOME DO PRODUTO(MARCA), das duas primeiras letras da MARCA, do dia e do mês da DATA DE REGISTRO e do ANO de LANÇAMENTO.
	Para que seja possível gerar a chave primária, precisamos demarcar os campos, para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i< (*nregistros); i++){

		indice_primario[i].rrn = i; 
		
		strcpy(indice_primario[i].pk, recuperar_registro(i).pk);	
	}


	qsort(indice_primario, *nregistros, sizeof(Ip), Compara_iPrimary);

	/*VERIFICAR RESULTADO DA ORDENAÇÃO*/
	// for(int i = 0; i< (*nregistros); i++){
	// 		printf("RRN: %d PK: %s\n", indice_primario[i].rrn, indice_primario[i].pk);
			
	// }

}

/*A função responsável por COMPARAR as informações para a função QSORT, por definição recebe dois CONST VOID */
/* A ORDENAÇÃO do iProduct será realizada incialmente pelo NOME DO PRODUTO (MODELO) e em caso de empate pelo CÓDIGO (ÍNDICE PRIMÁRIO) */
int Compara_iProduct(const void * pNome , const void * sNome){

	// printf("\nCOMPARAÇAO\n");

	// /* Primeiro Nome*/
	// char pN[TAM_NOME];
	// strcpy(pN,(*(Is *)pNome).string);

	// // printf("\nPrimeiro: %s\n", pN);
	
	// /*Segundo Código*/
	// char sN[TAM_NOME];
	// strcpy(sN, (*(Is*)sNome).string);

	// // printf("\nSegundo: %s\n", sN);

	// // int Resultado = strcmp((*(Is *)pNome).string , (*(Is*)sNome).string);

	// int Resultado = strcmp(pN, sN);

	/*Monitoria*/

	int Resultado = strcmp((*(Is *)pNome).string,(*(Is*)sNome).string);
	// printf("Resultado (Comparaçao): %d\n");

	if(Resultado == 0){
		Resultado = strcmp((*(Is *)pNome).pk,(*(Is*)sNome).pk);
	}


	/*ORDENAR CASO RESULTADO SEJA ZERO*/
	
	return Resultado;

}

void Criar_iProduct (Is* iProduct, int * nRegistros ){

	/* O índice secundário IPRODUCT contém a CHAVE PRIMÁRIA (pk) e o NOME do PRODUTO do REGISTRO. Para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i < (*nRegistros); i++){
		
		strcpy(iProduct[i].pk, recuperar_registro(i).pk);

		strcpy(iProduct[i].string, recuperar_registro(i).nome); 

	}

	qsort(iProduct, *nRegistros, sizeof(Is), Compara_iProduct);

	//qsort(iProduct, *nRegistros, sizeof(Is), Compara_iPrimary);

}

/*A função responsável por COMPARAR as informações para a função QSORT, por definição recebe dois CONST VOID */
/* A ORDENAÇÃO do iBrand será realizada incialmente pela MARCA e em caso de empate pelo CÓDIGO (ÍNDICE PRIMÁRIO) */
int Compara_iBrand(const void * pMarca , const void * sMarca){

	// printf("\nCOMPARAÇAO\n");

	/* Primeiro Marca*/
	char pM[TAM_MARCA];
	strcpy(pM,(*(Is *)pMarca).string);

	// printf("\nPrimeiro: %s\n", pCod);
	
	/*Segundo Marca*/
	char sM[TAM_MARCA];
	strcpy(sM, (*(Is*)sMarca).string);

	// printf("\nSegundo: %s\n", sCod);

	int Resultado = strcmp(pM, sM);

	// int Resultado = strcmp((*(Is *)pMarca).string), (*(Is*)sMarca).string));
	// printf("Resultado (Comparaçao): %d\n");

	/*Monitoria*/

	if(Resultado == 0){
		Resultado = strcmp((*(Is *)pMarca).pk,(*(Is*)sMarca).pk);
	}

	/*ORDENAR CASO RESULTADO SEJA ZERO - IGUAIS*/
	
	return Resultado;

}

void Criar_iBrand (Is* iBrand, int * nRegistros){

	/* O índice secundário IBRAND contém a CHAVE PRIMÁRIA (pk) e a MARCA do REGISTRO. Para isso podemos utilizar a função "recuperar_registro" que retorna um produto com cada campo demarcado, inclusive com um "índice primário" gerado pela função "gerarChave" */

	for(int i = 0; i < (*nRegistros); i++){

		strcpy(iBrand[i].pk, recuperar_registro(i).pk);

		strcpy(iBrand[i].string, recuperar_registro(i).marca);
	
	}

	qsort(iBrand, *nRegistros, sizeof(Is), Compara_iBrand);

}

/*A função responsável por COMPARAR as informações para a função QSORT, por definição recebe dois CONST VOID */
/* A ORDENAÇÃO do iPrice será realizada incialmente pela PREÇO FINAL e pelo CÓDIGO (ÍNDICE PRIMÁRIO) */
int Compara_iPrice(const void * pPreco , const void * sPreco){

	/* A função STRCMP não pode ser utilizada para FLOAT, apenas para CONST CHAR*, entretanto por convenção vamos aderir as mesmas possibilidades para RESULTADO
		<0 - Indica que o PRIMEIRO parâmetro é MENOR que SEGUNDO
		0  - Indica que os parâmentros são IGUAIS
		>0 - Indice que o PRIMEIRO parâmentro é MAIOR que o SEGUNDO parâmentro
	*/

	/* Por definição estamos recebendo CONST VOID *, então precisamos realizar a conversão, para por conseguinte podermos acessar o dados que desejamos*/

	/* Primeiro Preco*/
	float pP;
	pP = (*(Isf *)pPreco).price;
	
	/*Segundo Preco*/
	float sP;
	sP = (*(Isf*)sPreco).price;

	// int Resultado = strcmp(pP, sP);

	int Resultado;

	if(pP < sP)
		Resultado = -1;
	else if( pP == sP)
		Resultado = 0;
	else if (pP > sP)
		Resultado = 1;

	/*Monitoria*/
	
	if(Resultado == 0){
		Resultado = strcmp((*(Isf *)pPreco).pk,(*(Isf*)sPreco).pk);
	}
	

	return Resultado;

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
	 
	qsort(iPrice, *nRegistros, sizeof(Isf), Compara_iPrice);

}

/* Realiza os scanfs na struct Produto */
void ler_entrada(char * registro, Produto *novo){
}

void Inserir(Produto* Novo, Ip *iPrimary, Is* iProduct, Is* iBrand, Ir* iCategory, Isf *iPrice, int * nRegistros, int nCat){

	int nRegistrosAx = *nRegistros;
	//printf("%d", nRegistrosAx);

	

	//Código - NÃO é inserido pelo usuário 
	// char pk[TAM_PRIMARY_KEY];
	// gerarChave(novo);

	/*-----------------------*/

	/* Interação com o Usuário */

	/* CAMPOS DE TAMANHO VARIÁVEL */
	
	//Nome do Produto ou Modelo
	// char Nome[TAM_NOME];
	
	scanf("%[^\n]s", Novo->nome);
	getchar();
	//Marca
	// char Marca[TAM_MARCA];
	scanf("%[^\n]s", Novo->marca);
	getchar();
	/*-----------------------*/

	/* CAMPOS DE TAMANHO FIXO */

	//Data de Registro
	// char Data[TAM_DATA];	/* DD/MM/AAAA */
	scanf("%[^\n]s", Novo->data);
	getchar();
	//Ano de Lançamento
	// char Ano[TAM_ANO];
	scanf("%[^\n]s", Novo->ano);
	getchar();
	//Preço-Base
	// char Preço[TAM_PRECO];
	scanf("%[^\n]s", Novo->preco);
	getchar();
	//Desconto
	// char Desconto[TAM_DESCONTO];
	scanf("%[^\n]s", Novo->desconto);
	getchar();
	//Categorias
	// char Categoria[TAM_CATEGORIA];
	scanf("%[^\n]s", Novo->categoria);
	getchar();
	/*-----------------------*/

	gerarChave(Novo);

	Ip * Busca = (Ip*)bsearch(Novo->pk, iPrimary, *nRegistros, sizeof(Ip), Compara_iPrimary);

	if(Busca != NULL)
		printf(ERRO_PK_REPETIDA, Novo->pk);
	
	else{
		
		nRegistrosAx++;
		
		//printf("%d", nRegistrosAx);

		*nRegistros = nRegistrosAx;
		
		//Registro Auxiliar
		char rAuxiliar[193]; //TAM_REGISTRO
		rAuxiliar[192] = '\0';

		sprintf(rAuxiliar, "%s@%s@%s@%s@%s@%s@%s@", Novo->nome, Novo->marca,Novo->data, Novo->ano,Novo->preco, Novo->desconto, Novo->categoria);

		//Precisamos obter o TAMANHO do REGISTRO AUXILIAR (rAuxiliar) para sabermos quantos "bytes" faltam para preencher totalmento o REGISTRO.
		int Tamanho = strlen(rAuxiliar);

		// printf("\nTamanho = %d\n", Tamanho);

		int i;
		//Preenchendo o REGISTRO por completo (192bytes)
		for(i = Tamanho; i < 192; i++)
			rAuxiliar[i] = '#';

		// printf("\nTamanho - Final = %d", strlen(rAuxiliar));
		// printf("\n Registro: %s \n", rAuxiliar);


			strcat(ARQUIVO, rAuxiliar);
			Criar_iPrimary(iPrimary, nRegistros);
			Criar_iProduct(iProduct, nRegistros);
			Criar_iBrand(iBrand, nRegistros);
			Criar_iPrice(iPrice, nRegistros);
		
		}

}



void Busca_iPrimary(char *pk, Ip* iPrimary, int* nRegistros){

	Ip * Busca = (Ip*)bsearch(pk, iPrimary, *nRegistros, sizeof(Ip), Compara_iPrimary);

	if(Busca == NULL)
		printf(REGISTRO_N_ENCONTRADO);
	
	else{
		//Registro do produto procurado
		int Registro = Busca -> rrn;
		//Produto Auxiliar = recuperar_registro(Registro);
		exibir_registro(Registro, 0);

	}	
		
}

/* Realiza três tipos de BUSCA, por CÓDIGO, NOME DO PRODUTO ou MODELO e por NOME DA MARCA e CATEGORIA.*/
void Busca_Produto(Ip* iPrimary, Is* iProduct, Is* iBrand, Ir* iCategory, int* nRegistros, int nCat){

	//printf(INICIO_BUSCA );

	
	int Opcao;
	scanf("%d", &Opcao);

	// if(!*nRegistros){
	// 	printf(ARQUIVO_VAZIO);
   	// 	return;
  	// }

	char pk[TAM_PRIMARY_KEY];

	switch(Opcao){
		case 1:
			getchar();
			scanf("%[^\n]s", pk);
			// printf("%s\n", pk);
			getchar();
			Busca_iPrimary(pk, iPrimary, nRegistros);
		break;

	}

}

void Listar(Ip* iPrimary, Is* iProduct, Is* iBrand, Ir* iCategory, int* nRegistros, int nCat){

	int Registro = -1;

	int Opcao;
	scanf("%d", &Opcao);

	if(!(*nRegistros))
		printf(REGISTRO_N_ENCONTRADO);

	switch(Opcao){
		case 1:
			for(int i = 0; i < (*nRegistros)-1; i++){
				exibir_registro(iPrimary[i].rrn, 0);
				printf("\n");
			}
			exibir_registro(iPrimary[(*nRegistros)-1].rrn, 0);

		break;
		
		case 3:
			for(int i = 0; i < (*nRegistros)-1; i++){
				Ip * Busca = (Ip*)bsearch(iBrand[i].pk, iPrimary, *nRegistros, sizeof(Ip), Compara_iPrimary);
				if(Busca != NULL)
					Registro = Busca->rrn;
				exibir_registro(Registro, 0);
				printf("\n");
			}
			Ip * Busca = (Ip*)bsearch(iBrand[*nRegistros].pk, iPrimary, *nRegistros, sizeof(Ip), Compara_iPrimary);
			if(Busca != NULL)
					Registro = Busca->rrn;
			exibir_registro(Registro, 0);
		break;

	}
}

/* ---------------------------------------------- */
