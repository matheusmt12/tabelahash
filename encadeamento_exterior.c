#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "compartimento_hash.h"
#include "encadeamento_exterior.h"
#include "cliente.h"

int thash (int num){
	return num % 7;
}

void cria_hash(char *nome_arquivo_hash, int tam)
{
	CompartimentoHash *out;
	FILE *write;
	write = fopen (nome_arquivo_hash,"wb");
	if (write == NULL){
		return 0;
	}else{
		for (int i = 0; i < tam; i++){
			out = compartimento_hash (-1);
			salva_compartimento (out,write);
		}
	}fclose(write);

}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
	FILE *filehas = fopen(nome_arquivo_hash,"rb");
	FILE *filedad = fopen(nome_arquivo_dados,"rb");
	int pos = 0,hash = thash(cod_cli);
	if (filedad == NULL && filehas == NULL){
		return -1;
	}else{
		Cliente *cli;
		CompartimentoHash  *aux;

		fseek(filehas,(hash*tamanho_compartimento()),SEEK_SET);
		aux = le_compartimento(filehas);

		if (aux->prox == -1){
			return -1;
		}else{
			fseek(filedad,(aux->prox*tamanho_cliente()),SEEK_SET);
			cli = le_cliente(filedad);
			if (cli->cod_cliente == cod_cli && cli->status == OCUPADO){
				pos = aux->prox;
				return pos;
			}else{
				
				while(cli!= NULL){
					pos++;
					if (cli->cod_cliente == cod_cli && cli->status == OCUPADO){
						return pos;
					}
					cli = le_cliente(filedad);
					
				}
			}
		}

	}

	fclose(filehas);
	fclose(filedad);

    return -1;
}


int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
	FILE *dado = fopen(nome_arquivo_dados,"rb+");
	FILE *has = fopen (nome_arquivo_hash,"rb+");
	int pos = 0, hash = thash (cod_cli);
	Cliente *aux,*ant,*per,*novo;
	CompartimentoHash *comp;
	if (dado == NULL && has == NULL){
		return -1;
	}else{
		fseek(has,(hash*tamanho_compartimento()),SEEK_SET);
		comp = le_compartimento(has);

		if(comp->prox == -1){
			aux = le_cliente(dado);
			while (aux != NULL){
				if (aux->cod_cliente == cod_cli){
					return -1;
					}
				aux = le_cliente(dado);
				pos++;
			}
		novo = cliente(cod_cli,nome_cli,-1,OCUPADO);
		salva_cliente(novo,dado);
		
		CompartimentoHash *novoc = compartimento_hash(pos);
		fseek(has,(hash * tamanho_compartimento()),SEEK_SET);
		salva_compartimento(novoc,has);
		}else{
			fseek(dado,(comp->prox * tamanho_cliente()),SEEK_SET);
			aux = le_cliente (dado);
			per = aux;

			if (aux->cod_cliente == cod_cli){
				return -1;
			}
			
			if (aux->prox != -1){
				while(per->prox != -1 && per->status != LIBERADO){
					fseek(dado,(per->prox*tamanho_cliente()),SEEK_SET);
					ant = per;
					per = le_cliente(dado);
				}
			}
			if (per->status == LIBERADO){
				novo = cliente(cod_cli,nome_cli,per->prox,OCUPADO);
				fseek(dado,(ant->prox*tamanho_cliente()),SEEK_SET);
				salva_cliente(novo,dado);
				pos = ant->prox;
			}else{
				pos = ant->prox+1;
				fseek(dado,((pos)*tamanho_cliente()),SEEK_SET);
				novo = cliente(cod_cli,nome_cli,-1,OCUPADO);
				salva_cliente(novo,dado);

				fseek(dado,((ant->prox)*tamanho_cliente()),SEEK_SET);
				per->prox = pos;
				salva_cliente(per,dado);
			}
		}
	
	}
	fclose(dado);
	fclose(has);
	return pos;
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{	
	FILE *filehas = fopen(nome_arquivo_hash,"rb+");
	FILE *filedad = fopen(nome_arquivo_dados,"rb+");
	int pos = 0, hash = thash(cod_cli);
	if (filedad == NULL && filehas == NULL){
		return -1;
	}else{
		Cliente *cli;
		pos = busca(cod_cli,nome_arquivo_hash,nome_arquivo_dados);
		if (pos == -1){
			return -1;
		}
		fseek(filedad,(pos*tamanho_cliente()),SEEK_SET);
		cli = le_cliente(filedad);
		if ( cli->cod_cliente == cod_cli){
		cli->status = LIBERADO;
		fseek(filedad,(pos*tamanho_cliente()),SEEK_SET);
		salva_cliente(cli,filedad);
		}
	}
	fclose(filedad);
	fclose(filehas);
	return pos;
}