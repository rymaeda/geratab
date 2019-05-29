/*
**
** Programa para gerar a tabelas de combinacoes das engrenagens
** de passos de roscas.
**
** Inicio: 08 de agosto de 2015
** 08/08/15: gera combinacoes.
** 09/08/15: le dados de arquivo.
** 10/08/15: escreve resultados num vetor e ordena
** 11/08/15: elimina combinacoes reduntantes e opcionalmente os equivalentes (comuta NA/NB e NC/ND),
**                 implementacao  de locale e limitacao do passo maximo na saida (duas  vezes o passo do fuso).
** 12/08/15: gera saida html, implementacao de pesquisa tabela.
** 23/08/15: refazimento da funcao learquivo.
** 06/12/16: restricao dos tamanhos maximos das engrenagem AB e CD.
** 01/01/18: soh gera os passos listados no arquivo de entrada.
** 06/01/18: nomeia e cria arquivo de saida automaticamente.
** 07/01/18: rearranjo de código, inclusão de hora e data na tabela.
** 08/01/18: mudanca de criterio de classificacao de combinacoes com uso do produto vetorial.
** 11/01/18: remocao de redundancias durante a geracao de combinacoes. Menor consumo de memoria.
** 27/01/18: correcao rotina de solucao com 2 engrenagens.
** 26/03/19: melhora no criterio para classificacao de  melhor solucao.
** 31/03/19: remocao de codigo morto.
** 26/05/19: correcao no uso do kludge factor.
**
** Ricardo Y. Maeda - rymaeda AT yahoo.com
**
** Obs: entende-se que sao utilizadas 4 ou 2 engenagens na caixa norton.
** Onde o passo eh calculado com: p= K*PF*(NA/NB)*(NC/ND) ou p= K*PF*(NA/ND)
** Considera-se que as engrenagens tem no maximo 256 dentes.
**
*/


/* --- The following code comes from c:\lcc\lib\wizard\textmode.tpl. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define MAX_OBJETIVOS 200
#define MAX_COMBINACOES 120000
#define NE 4	/* numero de engrenagens na cx norton*/

/* flags (variaveis de um bit) */
#define OPCAO_TXT 1
#define OPCAO_HTML 2
#define OPCAO_NAO_COMUTATIVO 16
#define OPCAO_LIMITA_PASSO 32
#define OPCAO_NAO_REDUZ 64
#define OPCAO_MM 128
#define OPCAO_POL 256

typedef struct {
	unsigned short NA;
	unsigned short NB;
	unsigned short NC;
	unsigned short ND;
	double passo;
	double sigma;
}
COMBINACAO, *pCOMBINACAO;

int N[50];	/* numero de dentes da i-esima engrenagem */
int iN;		/* numero de engrenagens em N[] */
double PF;		/* passo do fuso do torno */
double PD[MAX_OBJETIVOS]; /* passos desejados milimetros*/
double PDP[MAX_OBJETIVOS]; /* passos desejados polegadas*/
int iPD;		/* numero de resultados em milimetros de PD */
int iPDP;		/* numero de resultados em polegadas de PD */
char ArquivoSaida[100];
COMBINACAO combinacao[MAX_COMBINACOES];
int icomb;	/* numero de combinacoes */
unsigned short SinalizaOpcoes;
double FatorK;
int MinAB= 10;
int MaxAB= 1000;
int MinCD= 10;
int MaxCD= 1000;
char NomeArquivo[256];
char analise; /* ativa  modo de analise */

void LeArquivo(char *filename){
	FILE *fp1;
	char oneword[256];
	char *c;
	double aux;
	int seletor;	/* se refere a entrada numerica de: */
					/* 1 numero de dentes da engrenagem */
					/* 2 passos desejados em mm */
					/* 3 passo do fuso em mm */
					/* 4 passos desejados em polegadas */
					/* 5 passo do fuso em polegadas */
					/* 6 fator K */
					/* 7 min a + b */
					/* 8 max a + b */
					/* 9 min c + d */
					/* 10 max c + d */

	FatorK=1.0;
	fp1 = fopen(filename, "r");
	if (!fp1){
		fprintf(stderr,"Erro ao ler o arquivo de entrada. O arquivo existe?.\n");
		return;
	}
	seletor= 0;
	do {
		c = fgets(oneword, 255, fp1);
		if (oneword[strlen(oneword)-1]=='\n') /* remove o '\n' no final da string */	oneword[strlen(oneword)-1]=0;
		if (oneword[0]=='#')			continue;
		if (oneword[0]>'9') /* se a entrada nao eh numerica prepara para leitura de variavel */			seletor=0;
		if (c != NULL){
			if (!strcmp(oneword, "NumeroDeDentes")||seletor==1){
				if (seletor){
					N[iN]= atoi(oneword);
					iN++;
				}
				else
					seletor= 1;
			}
			if (!strcmp(oneword, "PassosDesejados-mm")||seletor==2){
				if (seletor){
					PD[iPD]= atof(oneword);
					iPD++;
				}
				else
					seletor= 2;
			}
			if (!strcmp(oneword, "PassoDoFuso-mm")||seletor==3){
				if (seletor) PF= atof(oneword);
				else
					seletor= 3;
			}
			if (!strcmp(oneword, "PassosDesejados-pol")||seletor==4){
				if (seletor){
					PDP[iPDP]= atof(oneword);
					iPDP++;
				}
				else
					seletor= 4;
			}
			if (!strcmp(oneword, "PassoDoFuso-pol")||seletor==5){
				if (seletor){
					aux= atof(oneword);
					if (aux>=0.0) PF= 25.4/aux;
					else{
						fprintf(stderr,"Passo por polegada nao pode ser menor ou igual a zero!\n");
						exit(-1);
					}
				}
				else
					seletor= 5;
			}
			if (!strcmp(oneword, "FatorK")||seletor==6){
				if (seletor) FatorK= atof(oneword);
				else
					seletor= 6;
			}
			if (!strcmp(oneword, "MinAB")||seletor==7){
				if (seletor) MinAB= atoi(oneword);
				else
					seletor= 7;
			}
			if (!strcmp(oneword, "MaxAB")||seletor==8){
				if (seletor) MaxAB= atoi(oneword);
				else
					seletor= 8;
			}
			if (!strcmp(oneword, "MinCD")||seletor==9){
				if (seletor) MinCD= atoi(oneword);
				else
					seletor= 9;
			}
			if (!strcmp(oneword, "MaxCD")||seletor==10){
				if (seletor) MaxCD= atoi(oneword);
				else
					seletor= 10;
			}
		}
	}
	while (c != NULL);              /* repeat until NULL          */
	fclose(fp1);
}

int Compara0(pCOMBINACAO elem1, pCOMBINACAO elem2){
	if ( elem1->passo < elem2->passo) return -1;
	else if ( elem1->passo > elem2->passo) return 1;
	else if (elem1->sigma < elem2->sigma) return -1;
	else if (elem1->sigma > elem2->sigma) return 1;
	else if (elem1->NA*elem1->NC > elem2->NA*elem2->NC) return -1;
	else if (elem1->NA*elem1->NC < elem2->NA*elem2->NC) return 1;
	else 	return 0;/* a=b */
}

int Compara(const void *p1, const void *p2){ /* formato utilizado pela std qsort() */
	return Compara0((pCOMBINACAO)p1, (pCOMBINACAO)p2);
}

int RemoveRedundancias(void){
	int i, j;
	i= 0;
	for (j=1; j<icomb; j++){
		if((combinacao+i)->passo!=(combinacao+j)->passo){
			if (j>(i+1)) memcpy(combinacao+i+1, combinacao+j, sizeof(COMBINACAO));
			i++;
		}
	}
	icomb= i + 1;
	return 0;
}

int GeraCombinacoes(void){
	int i, j, k, l;
	int cont;
	double p;
	cont= 0;
	/* gera combinacoes com o uso de 4 engrenagens */
	for(i=0; i< iN; i++){
		for(j=0; j< iN; j++){
			if (j==i) continue; /*elimina repeticao de engrenagem*/
			for(k=0; k< iN; k++){
				if(k==i||k==j) continue; /*elimina repeticao de engrenagem*/
				for(l=0; l< iN; l++){
					if (l==i||l==j||l==k) continue; /*elimina repeticao de engrenagem*/
					p= (PF/FatorK*(double)N[i]*(double)N[k])/((double)N[j]*(double)N[l]);
					if (p>(4*PF/FatorK)) continue; /* limita passo maximo*/
					/* Elimina combinacao que excede o minimo ou o maximo */
					if (((N[i]+N[j])>MaxAB)||((N[i]+N[j])<MinAB)||
						    ((N[k]+N[l])>MaxCD)||((N[k]+N[l])<MinCD)){
						//printf("A+B= %d\nC+D= %d\n", N[i]+N[k],N[k]+N[l]);
						continue;
					}
					/* Guarda combinacao */
					combinacao[cont].NA= N[i];
					combinacao[cont].NB= N[j];
					combinacao[cont].NC= N[k];
					combinacao[cont].ND= N[l];
					combinacao[cont].passo= p;
					combinacao[cont].sigma= fabs((double)N[i]*N[l]-N[k]*N[j])/(sqrt(N[i]*N[i]+N[j]*N[j])*sqrt(N[k]*N[k]+N[l]*N[l])); /*produto vetorial*/
					cont++;
					if (cont==(MAX_COMBINACOES-1)){
						icomb= cont;
						qsort(combinacao, cont, sizeof(COMBINACAO), Compara);
						RemoveRedundancias();
						cont=icomb;
						if (cont==MAX_COMBINACOES){
							fprintf(stderr, "Numero maximo de combinacoes excedido! Abortando.\n");
							fprintf(stderr, "Numero maximo de combinacoes= %d\n\n", MAX_COMBINACOES);
							exit(-1);
						}
					}
				}
			}
		}
	}
	/* gera combinacoes com 2 engrenagens */
	for(i=0; i< iN; i++){
		for(j=0; j< iN; j++){
			if (j==i) continue; /*elimina repeticao de engrenagem*/
			p= (PF/FatorK*N[i])/(N[j]);
			if (p>4*PF/FatorK)	continue; /* limita passo maximo */
//			if (((N[i]+N[j])>MaxAB)||((N[i]+N[j])<MinAB)){/* Verifica tamanhos minimos e maximos das engrenagens */
//				//printf("A+B= %d\n", N[i]+N[j]);
//				continue;
//			}
			//fprintf(stderr, "(%d|%d|%f/", N[i], N[j], p);
			combinacao[cont].NA= N[i];
			combinacao[cont].NB= 0;
			combinacao[cont].NC= 0;
			combinacao[cont].ND= N[j];
			combinacao[cont].passo= p;
			combinacao[cont].sigma= 0.0;
			cont++;
					if (cont==MAX_COMBINACOES){
						icomb= cont;
						qsort(combinacao, cont, sizeof(COMBINACAO), Compara);
						RemoveRedundancias();
						cont=icomb;
						if (cont==MAX_COMBINACOES){
							fprintf(stderr, "Numero maximo de combinacoes excedido! Abortando.\n");
							fprintf(stderr, "Numero maximo de combinacoes= %d\n\n", MAX_COMBINACOES);
							exit(-1);
						}
					}
		}
	}
	icomb= cont;
	qsort(combinacao, cont, sizeof(COMBINACAO), Compara);
	RemoveRedundancias();
	return cont;
}

int PesquisaTabela(double passo){ /* faz pesquisa pelo metodo da dicotomia customizado */
	int i, f, m; /* inicio, fim e meio do intervalo */
	i= 0;
	f= icomb;
	while((f-i)>1){
		m= (i+f)/2;
		if (combinacao[m].passo<passo)
			i=m;
		else
			f=m;
	}
	return i;
}

/*
** Gera o nome do arquivo de saida a partir do nome do arquivo de entrada
*/
void AjustaNomeArquivoSaida(void){
	int i, l;
	l= strlen(NomeArquivo);
	for(i=l; i; i--)
		if(NomeArquivo[i]=='.'){
			NomeArquivo[i]= 0;
			break;
		}
	strncat(NomeArquivo, ".html", 255);
	return;
}

/*
**
** Gera arquivo de saída com os passos encontrados.
**
*/
int ImprimeTabela(void){
	int i;
	FILE *fp;
	struct tm *t;
	time_t timet;
	char buf[255];
	AjustaNomeArquivoSaida();
	fp= fopen(NomeArquivo, "w");
	fprintf(fp,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n");
	fprintf(fp,"<html>\n<head>\n<meta content=\"text/html; charset=ISO-8859-1\"\nhttp-equiv=\"content-type\">\n");
	fprintf(fp,"<title>Geratab</title>\n</head>\n<body>\n");
	fprintf(fp,"<big><big>Foram geradas %d combinacoes.<br>\nPasso do fuso= %2.4fmm<br>\nEngrenagens consideradas: \n", icomb, PF);
	for (i= 0; i<iN; i++)
		fprintf(fp," %d", N[i]);
	fprintf(fp,"</big><br><br>");
	fprintf(fp,"<table style=\"text-align: center; width:  600px;\" border=\"1\"\n");
	fprintf(fp,"cellpadding=\"2\" cellspacing=\"2\">\n<tbody>\n");
	fprintf(fp,"<tr>\n<td>A</td>\n<td>B</td>\n<td>C</td>\n<td>D</td>\n");
	fprintf(fp,"<td style=\"background-color: rgb(102, 255, 255);\">Passo<br>(mm)</td>\n");
	fprintf(fp,"<td>seno</td></tr>\n");
	for (i= 0; i<icomb; i++){
		fprintf(fp, "<tr>\n<td>%3d</td>\n", combinacao[i].NA);
		if (combinacao[i].NB)
			fprintf(fp, "<td>%3d</td>\n<td>%3d</td>\n", combinacao[i].NB, combinacao[i].NC);
		else
			fprintf(fp, "<td>QQ</td>\n<td>--</td>\n");
		fprintf(fp, "<td>%3d</td>\n", combinacao[i].ND);
		fprintf(fp, "<td style=\"background-color: rgb(102, 255, 255);\">%2.4f</td>\n", combinacao[i].passo);
		fprintf(fp, "<td>%2.2f | %2.2f | %1.5f</td></tr>\n", (double)combinacao[i].NA/combinacao[i].NB, (double)combinacao[i].NC/combinacao[i].ND, combinacao[i].sigma);
	}
	fprintf(fp,"</big></tbody>\n</table>\n");
	time(&timet);
	t= localtime(&timet);
	strftime(buf, 100,"Data: %d/%m/%y  Hora: %H:%M:%S\n", t);
	fprintf(fp, buf);
	fprintf(fp, "<br>\n</body>\n</html>\n");
	return 0;
}

/*
** Gera arquivo de saída com os passos indicados no arquivo de entrada.
*/
int ImprimeDesejados(void){
	int i, it;
	FILE *fp;
	struct tm *t;
	time_t timet;
	char buf[255];
	AjustaNomeArquivoSaida();
	fp= fopen(NomeArquivo, "w");
	fprintf(fp, "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n");
	fprintf(fp, "<html>\n<head>\n<meta content=\"text/html; charset=ISO-8859-1\"\nhttp-equiv=\"content-type\">\n");
	fprintf(fp, "<title>Geratab</title>\n</head>\n<body>\n");
	fprintf(fp, "<big><big>Passo do fuso= %2.4gmm<br>\nEngrenagens consideradas: \n", PF);
	for (i= 0; i<iN; i++)
		fprintf(fp, " %d", N[i]);
	if (FatorK!=1.0) fprintf(fp, "<br>\nKludge Fator= %2.2f<br>\n", FatorK);
	fprintf(fp, "</big><br><br>");
	fprintf(fp, "<table style=\"text-align: center; width:  600px;\" border=\"1\"\n");
	fprintf(fp, "cellpadding=\"2\" cellspacing=\"2\">\n<tbody>\n");
	if (iPD){
		fprintf(fp, "<big>Passos em milímetros</big>\n");
		fprintf(fp, "<tr>\n<td>A</td>\n<td>B</td>\n<td>C</td>\n<td>D</td>\n<td>Passo<br>Nominal<br>(mm)</td>\n");
		fprintf(fp, "<td style=\"background-color: rgb(102, 255, 255);\">");
		fprintf(fp, "Passo<br>Efetivo<br>(mm)</td>\n<td>Erro</td>\n</tr>\n");
		for (i= 0; i<iPD; i++){
			it= PesquisaTabela(PD[i]);
			if ((combinacao[it+1].passo-PD[i])<(PD[i]-combinacao[it].passo))	it++;
			fprintf(fp, "<tr>\n<td>%3d</td>\n", combinacao[it].NA);
			if (combinacao[it].NB)
				fprintf(fp, "<td>%3d</td>\n<td>%3d</td>\n", combinacao[it].NB,  combinacao[it].NC);
			else
				fprintf(fp, "<td>QQ</td>\n<td>--</td>\n");
			fprintf(fp, "<td>%3d</td>\n", combinacao[it].ND);
			fprintf(fp, "<td>%2.2f</td>\n", PD[i]);
			fprintf(fp, "<td style=\"background-color: rgb(102, 255, 255);\">%2.4f</td>\n", combinacao[it].passo);
			fprintf(fp, "<td>%1.4f%%</td>\n</tr>\n", 100*(combinacao[it].passo-PD[i])/combinacao[it].passo);
		}
		fprintf(fp, "</tbody>\n</table>\n<br>\n</body>\n</html>\n");
	}
	fprintf(fp, "<br><br>");
	if (iPDP){
		fprintf(fp, "<big>Passos em polegadas</big>\n");
		fprintf(fp, "<table style=\"text-align: center; width:  600px;\" border=\"1\"\n");
		fprintf(fp, "cellpadding=\"2\" cellspacing=\"2\">\n<tbody>\n");
		fprintf(fp, "<tr>\n<td>A</td>\n<td>B</td>\n<td>C</td>\n<td>D</td>\n<td>Passo<br>Nominal<br>(fios/pol)</td>\n");
		fprintf(fp, "<td>Passo<br>Nominal<br>(mm)</td>\n");
		fprintf(fp, "<td style=\"background-color: rgb(102, 255, 255);\">");
		fprintf(fp, "Passo<br>Efetivo<br>(mm)</td>\n<td>Erro</td>\n</tr>\n");
		for (i= 0; i<iPDP; i++){
			it= PesquisaTabela(25.4/PDP[i]);
			if ((combinacao[it+1].passo-(25.4/PDP[i]))<((25.4/PDP[i])-combinacao[it].passo)) it++;
			fprintf(fp, "<tr>\n<td>%3d</td>\n", combinacao[it].NA);
			if (combinacao[it].NB)
				fprintf(fp, "<td>%3d</td>\n<td>%3d</td>\n", combinacao[it].NB,  combinacao[it].NC);
			else
				fprintf(fp, "<td>QQ</td>\n<td>--</td>\n");
			fprintf(fp, "<td>%3d</td>\n", combinacao[it].ND);
			fprintf(fp, "<td>%2.1f</td>\n", PDP[i]);
			fprintf(fp, "<td>%2.4f</td>\n", (25.4/PDP[i]));
			fprintf(fp, "<td style=\"background-color: rgb(102, 255, 255);\">");
			fprintf(fp, "%2.4f</td>\n", combinacao[it].passo);
			fprintf(fp, "<td>%1.4f%%</td>\n</tr>\n", 100*(combinacao[it].passo-(25.4/PDP[i]))/combinacao[it].passo);
		}
		fprintf(fp, "<br></big></tbody>\n</table>\n\n");
	}
	fprintf(fp, "Observacao: QQ eh qualquer engrenagem que caiba na posicao B da grade.<br>");
	time(&timet);
	t= localtime(&timet);
	strftime(buf, 100,"Data: %d/%m/%y  Hora: %H:%M:%S\n", t);
	fprintf(fp, buf);
	fprintf(fp, "<br>\n</body>\n</html>\n");
	return 0;
}

void Usage(char *programName){
	fprintf(stderr, "\n\n");
	fprintf(stderr,"*****************************************************\n");
	fprintf(stderr,"** Programa para gerar combinacoes de engrenagens. **\n");
	fprintf(stderr,"*****************************************************\n");
#ifdef __LCC__
	fprintf(stderr,"********* Compilacao: %s  %s *********\n", __DATE__, __TIME__);
	fprintf(stderr,"*****************************************************\n\n");
#endif
	fprintf(stderr,"Informar o arquivo de entrada com a opcao '-f':\n");
	fprintf(stderr,"Por exemplo: %s -f arquivo-de-entrada.txt\n\n\n",programName);
	fprintf(stderr,"A opcao -t gera **todos** os passos encontrados.\n");
}

/* returns the index of the first argument that is not an option; i.e.
does not start with a dash or a slash
*/
int HandleOptions(int argc,char *argv[]){
	int i, firstnonoption=0;

	for (i=1; i< argc;i++) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'f':
				strcpy(NomeArquivo, argv[i+1]);
				LeArquivo(NomeArquivo);
				break;
			case 't':
				analise= 1;
				break;
				/* An argument -? means help is requested */
			case '?':
				Usage(argv[0]);
				break;
			case 'h':
			case 'H':
				if (!strcmp(argv[i]+1,"help")) {
					Usage(argv[0]);
					break;
				}
			default:
				fprintf(stderr,"unknown option %s\n",argv[i]);
				break;
			}
		}
		else {
			firstnonoption = i;
			break;
		}
	}
	return firstnonoption;
}

int main(int argc,char *argv[]){
#ifdef __LCC__
	setlocale(LC_NUMERIC, "Portuguese_Brazil.1252");
#endif
	if (argc == 1) {
		/* If no arguments we call the Usage routine and exit */
		Usage(argv[0]);
		return 1;
	}
	/* handle the program options */
	HandleOptions(argc,argv); /* arquivo de entrada eh lido */
	GeraCombinacoes();
	if ((iPD+iPDP)==0)
		return -1;
	if (analise){
		ImprimeTabela();
		return 0;
	}
	ImprimeDesejados();
	return 0;
}
