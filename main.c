/*
 * PSE em Ação - Sistema de Planejamento e Acompanhamento de Ações do PSE
 * Versão 1.2 - Projeto acadêmico em Linguagem C (C99)
 *
 * Os registros ficam em memória durante a execução e são perdidos ao encerrar.
 * O módulo de bullying trabalha somente com indicadores coletivos agregados.
 * Não inserir nomes, códigos individuais, relatos pessoais ou dados clínicos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_ACOES 100
#define MAX_INDICADORES 100
#define TAM_ESCOLA 80
#define TAM_TEXTO 100
#define TAM_DATA 11
#define TAM_LINHA 256

typedef struct {
    int codigo;
    char escola[TAM_ESCOLA];
    char tema[TAM_TEXTO];
    char dataPrevista[TAM_DATA];
    char publicoAlvo[TAM_TEXTO];
    char responsavel[TAM_TEXTO];
    int participantesPrevistos;
    int participantesPresentes;
    int situacao; /* 1 = planejada, 2 = realizada, 3 = cancelada */
} Acao;

typedef struct {
    char escola[TAM_ESCOLA];
    char periodo[TAM_TEXTO];
    int verbal;
    int fisica;
    int psicologica;
    int exclusaoSocial;
    int acolhimentosColetivos;
    int quantidadeFeedbacks;
    int somaNotasFeedback;
} IndicadorColetivo;

static Acao acoes[MAX_ACOES];
static IndicadorColetivo indicadores[MAX_INDICADORES];
static int totalAcoes = 0;
static int totalIndicadores = 0;

/* Le uma linha inteira e remove o caractere de quebra de linha. */
int lerLinha(const char *mensagem, char destino[], int tamanho) {
    size_t comprimento;
    int caractere;

    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) return 0;

    comprimento = strlen(destino);
    if (comprimento > 0 && destino[comprimento - 1] == '\n') {
        destino[comprimento - 1] = '\0';
    } else {
        caractere = getchar();
        if (caractere != '\n' && caractere != EOF) {
            while ((caractere = getchar()) != '\n' && caractere != EOF) { }
            printf("Texto muito longo. O limite deste campo é de %d caracteres.\n", tamanho - 1);
            return -1;
        }
    }
    return 1;
}

int textoTemConteudo(const char texto[]) {
    int i;
    for (i = 0; texto[i] != '\0'; i++) {
        if (!isspace((unsigned char)texto[i])) return 1;
    }
    return 0;
}

void lerTexto(const char *mensagem, char destino[], int tamanho) {
    int resultado;
    do {
        resultado = lerLinha(mensagem, destino, tamanho);
        if (resultado == 0) {
            destino[0] = '\0';
            return;
        }
        if (resultado == 1 && !textoTemConteudo(destino)) {
            printf("Este campo é obrigatório. Tente novamente.\n");
            resultado = -1;
        }
    } while (resultado != 1);
}

int lerInteiro(const char *mensagem, int minimo, int maximo) {
    char linha[TAM_LINHA];
    char *fim;
    long valor;

    while (1) {
        if (lerLinha(mensagem, linha, sizeof(linha)) == 0) return minimo;
        valor = strtol(linha, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (fim == linha || *fim != '\0' || valor < minimo || valor > maximo) {
            printf("Entrada inválida. Digite um número inteiro entre %d e %d.\n", minimo, maximo);
        } else {
            return (int)valor;
        }
    }
}

int anoBissexto(int ano) {
    return (ano % 400 == 0) || (ano % 4 == 0 && ano % 100 != 0);
}

int dataValida(const char data[]) {
    int dia, mes, ano, diasMes;
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') return 0;
    if (!isdigit((unsigned char)data[0]) || !isdigit((unsigned char)data[1]) ||
        !isdigit((unsigned char)data[3]) || !isdigit((unsigned char)data[4]) ||
        !isdigit((unsigned char)data[6]) || !isdigit((unsigned char)data[7]) ||
        !isdigit((unsigned char)data[8]) || !isdigit((unsigned char)data[9])) return 0;

    dia = (data[0] - '0') * 10 + (data[1] - '0');
    mes = (data[3] - '0') * 10 + (data[4] - '0');
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 +
          (data[8] - '0') * 10 + (data[9] - '0');
    if (ano < 2000 || mes < 1 || mes > 12 || dia < 1) return 0;

    diasMes = 31;
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) diasMes = 30;
    else if (mes == 2) diasMes = anoBissexto(ano) ? 29 : 28;
    return dia <= diasMes;
}

void lerData(char destino[]) {
    do {
        lerTexto("Data prevista (DD/MM/AAAA): ", destino, TAM_DATA);
        if (!dataValida(destino)) printf("Data inválida. Use uma data real no formato DD/MM/AAAA.\n");
    } while (!dataValida(destino));
}

int codigoExiste(int codigo) {
    int i;
    for (i = 0; i < totalAcoes; i++) {
        if (acoes[i].codigo == codigo) return 1;
    }
    return 0;
}

const char *textoSituacao(int situacao) {
    switch (situacao) {
        case 1: return "Planejada";
        case 2: return "Realizada";
        case 3: return "Cancelada";
        default: return "Indefinida";
    }
}

void pausar(void) {
    char linha[8];
    printf("\nPressione ENTER para continuar...");
    fgets(linha, sizeof(linha), stdin);
}

void cadastrarAcao(void) {
    Acao nova;
    if (totalAcoes >= MAX_ACOES) {
        printf("Limite de %d ações atingido.\n", MAX_ACOES);
        return;
    }

    printf("\n=== CADASTRO DE AÇÃO DO PSE ===\n");
    while (1) {
        nova.codigo = lerInteiro("Código numérico da ação: ", 1, 999999999);
        if (!codigoExiste(nova.codigo)) break;
        printf("Código duplicado. Informe outro código.\n");
    }
    lerTexto("Nome da escola: ", nova.escola, TAM_ESCOLA);
    lerTexto("Tema da ação: ", nova.tema, TAM_TEXTO);
    lerData(nova.dataPrevista);
    lerTexto("Público-alvo: ", nova.publicoAlvo, TAM_TEXTO);
    lerTexto("Responsável pela ação: ", nova.responsavel, TAM_TEXTO);
    nova.participantesPrevistos = lerInteiro("Quantidade prevista de participantes: ", 0, 1000000);
    nova.participantesPresentes = 0;
    nova.situacao = 1;

    acoes[totalAcoes++] = nova;
    printf("\nAção cadastrada com sucesso! Código: %d\n", nova.codigo);
}

void imprimirAcao(const Acao *acao) {
    printf("\n----------------------------------------\n");
    printf("Código: %d\nEscola: %s\nTema: %s\nData prevista: %s\n", acao->codigo,
           acao->escola, acao->tema, acao->dataPrevista);
    printf("Público-alvo: %s\nResponsavel: %s\n", acao->publicoAlvo, acao->responsavel);
    printf("Participantes previstos: %d\nParticipantes presentes: %d\nSituação: %s\n",
           acao->participantesPrevistos, acao->participantesPresentes,
           textoSituacao(acao->situacao));
}

void listarAcoes(void) {
    int i;
    printf("\n=== AÇÕES CADASTRADAS ===\n");
    if (totalAcoes == 0) {
        printf("Nenhuma ação cadastrada.\n");
        return;
    }
    for (i = 0; i < totalAcoes; i++) imprimirAcao(&acoes[i]);
    printf("\nTotal de acoes: %d\n", totalAcoes);
}

void pesquisarAcoes(void) {
    int opcao, codigo, i, encontrados = 0;
    char termo[TAM_ESCOLA];
    printf("\n=== PESQUISAR AÇÕES ===\n1 - Por código\n2 - Por escola\n3 - Por tema\n");
    opcao = lerInteiro("Escolha: ", 1, 3);

    if (opcao == 1) {
        codigo = lerInteiro("Informe o código: ", 1, 999999999);
        for (i = 0; i < totalAcoes; i++) {
            if (acoes[i].codigo == codigo) {
                imprimirAcao(&acoes[i]);
                encontrados++;
            }
        }
    } else {
        lerTexto(opcao == 2 ? "Informe o nome da escola: " : "Informe o tema: ", termo, sizeof(termo));
        for (i = 0; i < totalAcoes; i++) {
            const char *campo = (opcao == 2) ? acoes[i].escola : acoes[i].tema;
            if (strstr(campo, termo) != NULL) {
                imprimirAcao(&acoes[i]);
                encontrados++;
            }
        }
    }
    if (encontrados == 0) printf("Nenhuma ação encontrada.\n");
    else printf("\nRegistros encontrados: %d\n", encontrados);
}

void atualizarAcao(void) {
    int codigo, i, novaSituacao;
    printf("\n=== ATUALIZAR SITUAÇÃO DA AÇÃO ===\n");
    if (totalAcoes == 0) {
        printf("Não existem ações cadastradas.\n");
        return;
    }
    codigo = lerInteiro("Código da ação: ", 1, 999999999);
    for (i = 0; i < totalAcoes; i++) {
        if (acoes[i].codigo == codigo) {
            imprimirAcao(&acoes[i]);
            printf("\n1 - Planejada\n2 - Realizada\n3 - Cancelada\n");
            novaSituacao = lerInteiro("Nova situação: ", 1, 3);
            acoes[i].situacao = novaSituacao;
            if (novaSituacao == 2) {
                acoes[i].participantesPresentes = lerInteiro("Quantidade efetiva de participantes: ", 0, 1000000);
                if (acoes[i].participantesPresentes > acoes[i].participantesPrevistos)
                    printf("Aviso: o número de participantes presentes supera a previsão cadastrada.\n");
            } else {
                acoes[i].participantesPresentes = 0;
            }
            printf("Situação atualizada com sucesso!\n");
            return;
        }
    }
    printf("Ação com código %d não encontrada.\n", codigo);
}

void gerarResumo(void) {
    int i, planejadas = 0, realizadas = 0, canceladas = 0;
    long previstosRealizadas = 0, presentes = 0;
    double percentual = 0.0;

    for (i = 0; i < totalAcoes; i++) {
        if (acoes[i].situacao == 1) planejadas++;
        else if (acoes[i].situacao == 2) {
            realizadas++;
            previstosRealizadas += acoes[i].participantesPrevistos;
            presentes += acoes[i].participantesPresentes;
        } else if (acoes[i].situacao == 3) canceladas++;
    }
    if (previstosRealizadas > 0) percentual = (double)presentes * 100.0 / (double)previstosRealizadas;

    printf("\n========== RESUMO GERAL DO PSE ==========\n");
    printf("Total de ações cadastradas: %d\nAções planejadas: %d\nAções realizadas: %d\nAções canceladas: %d\n",
           totalAcoes, planejadas, realizadas, canceladas);
    printf("Participantes previstos nas acoes realizadas: %ld\nParticipantes efetivos: %ld\n",
           previstosRealizadas, presentes);
    if (previstosRealizadas > 0) printf("Percentual de participação: %.2f%%\n", percentual);
    else printf("Percentual de participação: não calculável (não há previsão nas ações realizadas).\n");
    printf("=========================================\n");
}

void registrarIndicadoresColetivos(void) {
    IndicadorColetivo novo;
    int minimoNotas, maximoNotas;
    if (totalIndicadores >= MAX_INDICADORES) {
        printf("Limite de %d registros coletivos atingido.\n", MAX_INDICADORES);
        return;
    }

    printf("\n=== REGISTRO DE INDICADORES COLETIVOS ===\n");
    printf("ATENÇÃO: informe somente totais agregados. Não digite nomes, códigos de estudantes ou relatos individuais.\n");
    lerTexto("Escola: ", novo.escola, TAM_ESCOLA);
    lerTexto("Período de referência (ex.: 1º semestre/2026): ", novo.periodo, TAM_TEXTO);
    novo.verbal = lerInteiro("Quantidade agregada de ocorrências verbais: ", 0, 1000000);
    novo.fisica = lerInteiro("Quantidade agregada de ocorrências físicas: ", 0, 1000000);
    novo.psicologica = lerInteiro("Quantidade agregada de ocorrências psicológicas: ", 0, 1000000);
    novo.exclusaoSocial = lerInteiro("Quantidade agregada de ocorrências de exclusão social: ", 0, 1000000);
    novo.acolhimentosColetivos = lerInteiro("Quantidade total de acolhimentos coletivos realizados: ", 0, 1000000);
    novo.quantidadeFeedbacks = lerInteiro("Quantidade de respostas de feedback anônimas e agregadas: ", 0, 1000000);

    minimoNotas = novo.quantidadeFeedbacks;
    maximoNotas = novo.quantidadeFeedbacks * 5;
    if (novo.quantidadeFeedbacks == 0) novo.somaNotasFeedback = 0;
    else novo.somaNotasFeedback = lerInteiro("Soma das notas recebidas (cada nota de 1 a 5): ", minimoNotas, maximoNotas);

    indicadores[totalIndicadores++] = novo;
    printf("Indicadores coletivos registrados com sucesso.\n");
}

void listarIndicadoresColetivos(void) {
    int i, totalOcorrencias, totalFeedbacks = 0, somaNotas = 0;
    double media;
    printf("\n=== PAINEL DE INDICADORES COLETIVOS ===\n");
    if (totalIndicadores == 0) {
        printf("Nenhum indicador coletivo cadastrado.\n");
        return;
    }
    for (i = 0; i < totalIndicadores; i++) {
        totalOcorrencias = indicadores[i].verbal + indicadores[i].fisica +
            indicadores[i].psicologica + indicadores[i].exclusaoSocial;
        totalFeedbacks += indicadores[i].quantidadeFeedbacks;
        somaNotas += indicadores[i].somaNotasFeedback;
        printf("\nEscola: %s | Período: %s\n", indicadores[i].escola, indicadores[i].periodo);
        printf("Ocorrências agregadas: %d\n", totalOcorrencias);
        printf("Verbais: %d | Físicas: %d | Psicológicas: %d | Exclusão social: %d\n",
               indicadores[i].verbal, indicadores[i].fisica,
               indicadores[i].psicologica, indicadores[i].exclusaoSocial);
        printf("Acolhimentos coletivos: %d\nRespostas de feedback: %d\n",
               indicadores[i].acolhimentosColetivos, indicadores[i].quantidadeFeedbacks);
        if (indicadores[i].quantidadeFeedbacks > 0) {
            media = (double)indicadores[i].somaNotasFeedback / indicadores[i].quantidadeFeedbacks;
            printf("Média de satisfação: %.2f/5\n", media);
        } else printf("Média de satisfação: sem respostas.\n");
    }
    printf("\n--- CONSOLIDADO GERAL ---\nRegistros coletivos: %d\nTotal de respostas de feedback: %d\n",
           totalIndicadores, totalFeedbacks);
    if (totalFeedbacks > 0) {
        media = (double)somaNotas / totalFeedbacks;
        printf("Média geral de satisfação: %.2f/5\n", media);
    } else printf("Média geral de satisfação: sem respostas.\n");
}

void exibirMenu(void) {
    printf("\n==============================================\n");
    printf("                 PSE EM AÇÃO                  \n");
    printf(" Sistema de Planejamento e Acompanhamento de Ações do PSE\n");
    printf("==============================================\n");
    printf("1 - Cadastrar ação do PSE\n2 - Listar todas as ações\n3 - Pesquisar ações\n");
    printf("4 - Atualizar situação de uma ação\n5 - Gerar resumo estatístico das ações\n");
    printf("6 - Registrar indicadores coletivos de bullying e acolhimento\n");
    printf("7 - Consultar painel de indicadores coletivos\n0 - Encerrar sistema\n");
    printf("----------------------------------------------\n");
}

int main(void) {
    int opcao;
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    do {
        exibirMenu();
        opcao = lerInteiro("Selecione uma opção: ", 0, 7);
        switch (opcao) {
            case 1: cadastrarAcao(); break;
            case 2: listarAcoes(); break;
            case 3: pesquisarAcoes(); break;
            case 4: atualizarAcao(); break;
            case 5: gerarResumo(); break;
            case 6: registrarIndicadoresColetivos(); break;
            case 7: listarIndicadoresColetivos(); break;
            case 0: printf("\nSistema encerrado. Obrigado por utilizar o PSE em Ação!\n"); break;
            default: printf("Opção inválida.\n");
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
    return 0;
}
