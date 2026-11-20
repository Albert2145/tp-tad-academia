/*
 * matricula.h - TAD Matricula
 *
 * Representa os contratos entre um aluno e um plano (tabela Matricula do
 * banco). E um historico: toda adesao ou renovacao gera um registro novo e
 * os antigos nunca sao sobrescritos, apenas mudam de situacao.
 * Estrutura escolhida: lista simplesmente encadeada dinamica, com insercao
 * no INICIO. Assim as matriculas mais recentes (que sao as ativas) ficam no
 * comeco e as buscas por matricula ativa param cedo.
 */
#ifndef MATRICULA_H
#define MATRICULA_H

/* ---------- Tamanhos dos campos ---------- */
#ifndef TAM_CPF
#define TAM_CPF   15   // "000.000.000-00" + '\0'
#endif
#ifndef TAM_DATA
#define TAM_DATA  11   // "AAAA-MM-DD" + '\0'. Nesse formato, strcmp
#endif                 // compara datas na ordem cronologica correta.

/* ---------- Situacao da matricula (varchar(50) do banco -> enum) ---------- */
typedef enum {
    MATRICULA_ATIVA = 0,   // aluno pode treinar
    MATRICULA_TRANCADA,    // pausada temporariamente
    MATRICULA_CANCELADA,   // encerrada antes do prazo
    MATRICULA_ENCERRADA    // chegou ao fim do prazo
} SituacaoMatricula;

/* ---------- Codigos de retorno das operacoes ---------- */
typedef enum {
    MATRICULA_OK = 0,              // operacao realizada com sucesso
    MATRICULA_ERRO_PARAMETRO,      // ponteiro nulo ou dado invalido
    MATRICULA_ERRO_NAO_ENCONTRADA, // nenhuma matricula com o id/CPF
    MATRICULA_ERRO_VAZIA,          // a lista nao tem nenhuma matricula
    MATRICULA_ERRO_JA_ATIVA,       // aluno ja possui matricula ativa
    MATRICULA_ERRO_MEMORIA         // falha ao alocar memoria
} ResultadoMatricula;

/* ---------- Dados de uma matricula ---------- */
typedef struct {
    int id;                                // CHAVE: gerado automaticamente
    char cpf_aluno[TAM_CPF];               // aluno (chave estrangeira)
    int id_plano;                          // plano (chave estrangeira)
    char data_inicio[TAM_DATA];            // "AAAA-MM-DD"
    char data_prevista_termino[TAM_DATA];  // "AAAA-MM-DD"
    SituacaoMatricula situacao;            // ativa, trancada, ...
} Matricula;

/* ---------- Estrutura de dados: lista simplesmente encadeada ---------- */
typedef struct NoMatricula {
    Matricula dado;                // a matricula guardada neste no
    struct NoMatricula *proximo;   // proximo no (matricula mais antiga)
} NoMatricula;

typedef struct {
    NoMatricula *inicio;  // matricula mais recente (NULL se vazia)
    int quantidade;       // quantas matriculas estao guardadas (n)
    int proximo_id;       // proximo id a ser gerado (simula auto_increment)
} ListaMatriculas;

/* ---------- Operacoes ---------- */

/* Cria uma lista de matriculas vazia (inicio = NULL, proximo_id = 1).
 * Parametros: nenhum.
 * Retorno: ponteiro para a lista criada, ou NULL se faltar memoria. */
ListaMatriculas *criar_lista_matriculas(void);

/* Libera todos os nos e a propria lista.
 * Parametros: lista - lista a destruir (pode ser NULL).
 * Retorno: nenhum. Depois da chamada, o ponteiro nao deve ser mais usado. */
void liberar_lista_matriculas(ListaMatriculas *lista);

/* Cria uma matricula ATIVA no INICIO da lista e gera o id. Recusa se o
 * aluno ja tiver matricula ativa (a checagem percorre a lista).
 * Parametros: lista - lista de destino; cpf_aluno - CPF do aluno;
 *             id_plano - plano (> 0); data_inicio, data_prevista_termino -
 *             "AAAA-MM-DD" (termino >= inicio);
 *             id_gerado - saida: recebe o id criado (pode ser NULL).
 * Retorno: MATRICULA_OK; MATRICULA_ERRO_PARAMETRO; MATRICULA_ERRO_JA_ATIVA;
 *          MATRICULA_ERRO_MEMORIA. */
ResultadoMatricula inserir_matricula(ListaMatriculas *lista,
                                     const char *cpf_aluno, int id_plano,
                                     const char *data_inicio,
                                     const char *data_prevista_termino,
                                     int *id_gerado);

/* Procura a matricula ATIVA de um aluno, percorrendo a partir do inicio
 * (mais recentes primeiro).
 * Parametros: lista - lista de matriculas; cpf_aluno - CPF procurado.
 * Retorno: ponteiro para a matricula ativa, ou NULL se o aluno nao tiver
 *          matricula ativa, se a lista estiver vazia ou parametro for NULL. */
Matricula *buscar_matricula_ativa(const ListaMatriculas *lista,
                                  const char *cpf_aluno);

/* Procura uma matricula pelo id (chave).
 * Parametros: lista - lista de matriculas; id - id procurado.
 * Retorno: ponteiro para a matricula, ou NULL se nao existir. */
Matricula *buscar_matricula_por_id(const ListaMatriculas *lista, int id);

/* Muda a situacao de uma matricula (trancar, cancelar, encerrar, reativar).
 * Ao reativar, confere se o aluno ja tem outra matricula ativa.
 * Parametros: lista - lista de matriculas; id - matricula a alterar;
 *             nova_situacao - valor de SituacaoMatricula.
 * Retorno: MATRICULA_OK; MATRICULA_ERRO_PARAMETRO; MATRICULA_ERRO_VAZIA;
 *          MATRICULA_ERRO_NAO_ENCONTRADA; MATRICULA_ERRO_JA_ATIVA. */
ResultadoMatricula atualizar_situacao_matricula(ListaMatriculas *lista, int id,
                                                SituacaoMatricula nova_situacao);

/* Imprime o historico de matriculas de um aluno (mais recente primeiro).
 * Parametros: lista - lista de matriculas; cpf_aluno - CPF do aluno.
 * Retorno: quantidade de matriculas impressas, ou -1 se parametro for NULL. */
int listar_matriculas_do_aluno(const ListaMatriculas *lista,
                               const char *cpf_aluno);

/* Imprime as matriculas ATIVAS cuja data_prevista_termino e menor ou igual
 * a data_limite (relatorio de renovacoes da recepcao).
 * Parametros: lista - lista de matriculas; data_limite - "AAAA-MM-DD".
 * Retorno: quantidade de matriculas impressas, ou -1 se parametro for NULL. */
int listar_matriculas_vencendo(const ListaMatriculas *lista,
                               const char *data_limite);

/* Imprime todas as matriculas (mais recente primeiro).
 * Parametros: lista - lista de matriculas.
 * Retorno: nenhum (imprime aviso se a lista for NULL ou estiver vazia). */
void listar_matriculas(const ListaMatriculas *lista);

/* Remove de vez uma matricula (so para corrigir lancamento errado; o
 * normal e cancelar). Religa o no anterior ao seguinte e libera o no.
 * Parametros: lista - lista de matriculas; id - matricula a remover.
 * Retorno: MATRICULA_OK; MATRICULA_ERRO_PARAMETRO; MATRICULA_ERRO_VAZIA;
 *          MATRICULA_ERRO_NAO_ENCONTRADA. */
ResultadoMatricula remover_matricula(ListaMatriculas *lista, int id);

#endif /* MATRICULA_H */
