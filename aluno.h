/*
 * aluno.h - TAD Aluno
 *
 * Representa os alunos cadastrados na academia (tabela Aluno do banco).
 * Estrutura escolhida: array dinamico (lista sequencial) mantido ORDENADO
 * pelo CPF, para permitir busca binaria na operacao mais frequente
 * (buscar_aluno_por_cpf). Quando o array enche, a capacidade e dobrada.
 */
#ifndef ALUNO_H
#define ALUNO_H

// Tamanhos dos campos (varchar(n) do banco -> char[n+1])
// TAM_CPF e TAM_DATA tambem sao usados em matricula.h e turma.h;
// o #ifndef evita definir duas vezes quando os tres .h sao incluidos.
#ifndef TAM_CPF
#define TAM_CPF       15   // "000.000.000-00" (14 caracteres) + '\0'
#endif
#ifndef TAM_DATA
#define TAM_DATA      11   // "AAAA-MM-DD" (mesmo formato do MySQL) + '\0'
#endif
#define TAM_NOME     256   // varchar(255) + '\0'
#define TAM_TELEFONE  15   // "(31)94002-8922" (14 caracteres) + '\0'
#define TAM_EMAIL    256   // varchar(255) + '\0'
#define TAM_ENDERECO 256   // varchar(255) + '\0'

#define CAPACIDADE_INICIAL_ALUNOS 16  /* tamanho inicial sugerido do array */

/* ---------- Codigos de retorno das operacoes ---------- */
typedef enum {
    ALUNO_OK = 0,              // operacao realizada com sucesso
    ALUNO_ERRO_PARAMETRO,      // ponteiro nulo ou dado invalido
    ALUNO_ERRO_DUPLICADO,      // ja existe aluno com o mesmo CPF
    ALUNO_ERRO_NAO_ENCONTRADO, // nenhum aluno com o CPF informado
    ALUNO_ERRO_VAZIA,          // a lista nao tem nenhum aluno
    ALUNO_ERRO_MEMORIA         // falha ao alocar/realocar memoria
} ResultadoAluno;

/* ---------- Dados de um aluno ---------- */
typedef struct {
    char cpf[TAM_CPF];                 // CHAVE: identifica o aluno
    char nome_completo[TAM_NOME];      // nome completo
    char data_nascimento[TAM_DATA];    // "AAAA-MM-DD"
    char telefone[TAM_TELEFONE];       // telefone de contato
    char email[TAM_EMAIL];             // e-mail de contato
    char endereco[TAM_ENDERECO];       // endereco residencial
} Aluno;

/* ---------- Estrutura de dados: array dinamico ordenado por CPF ---------- */
typedef struct {
    Aluno *dados;     // vetor alocado com malloc/realloc, ordenado por cpf
    int quantidade;   // quantos alunos estao guardados (n)
    int capacidade;   // quantas posicoes o vetor tem alocadas
} ListaAlunos;

/* ---------- Operacoes ---------- */

/* Cria uma lista de alunos vazia.
 * Parametros: capacidade_inicial - posicoes a reservar (se <= 0, usa
 *             CAPACIDADE_INICIAL_ALUNOS).
 * Retorno: ponteiro para a lista criada, ou NULL se faltar memoria. */
ListaAlunos *criar_lista_alunos(int capacidade_inicial);

/* Libera toda a memoria da lista (o vetor e a propria struct).
 * Parametros: lista - lista a destruir (pode ser NULL).
 * Retorno: nenhum. Depois da chamada, o ponteiro nao deve ser mais usado. */
void liberar_lista_alunos(ListaAlunos *lista);

/* Insere um aluno na posicao correta para manter a ordem por CPF.
 * Se o vetor estiver cheio, dobra a capacidade com realloc.
 * Parametros: lista - lista de destino; novo - dados do aluno (copiados).
 * Retorno: ALUNO_OK; ALUNO_ERRO_PARAMETRO se algum ponteiro for NULL ou o
 *          CPF estiver vazio; ALUNO_ERRO_DUPLICADO se o CPF ja existir;
 *          ALUNO_ERRO_MEMORIA se o realloc falhar. */
ResultadoAluno inserir_aluno(ListaAlunos *lista, const Aluno *novo);

/* Procura um aluno pelo CPF usando busca binaria.
 * Parametros: lista - lista de alunos; cpf - CPF procurado.
 * Retorno: ponteiro para o aluno dentro da lista (pode ser alterado, exceto
 *          o CPF), ou NULL se nao existir, se a lista estiver vazia ou se
 *          algum parametro for NULL. O ponteiro deixa de valer apos uma
 *          insercao ou remocao. */
Aluno *buscar_aluno_por_cpf(const ListaAlunos *lista, const char *cpf);

/* Lista (imprime) todos os alunos cujo nome contem o trecho informado,
 * sem diferenciar maiusculas de minusculas. Percorre a lista inteira.
 * Parametros: lista - lista de alunos; trecho - parte do nome procurada.
 * Retorno: quantidade de alunos encontrados (0 se nenhum), ou -1 se algum
 *          parametro for NULL. */
int buscar_alunos_por_nome(const ListaAlunos *lista, const char *trecho);

/* Atualiza os dados de contato de um aluno (nome, nascimento, telefone,
 * e-mail e endereco). O CPF NAO muda, para nao quebrar a ordenacao nem as
 * matriculas e participacoes que referenciam o aluno.
 * Parametros: lista - lista de alunos; cpf - CPF do aluno a alterar;
 *             novos_dados - valores novos (o campo cpf e ignorado).
 * Retorno: ALUNO_OK; ALUNO_ERRO_PARAMETRO; ALUNO_ERRO_VAZIA;
 *          ALUNO_ERRO_NAO_ENCONTRADO. */
ResultadoAluno atualizar_aluno(ListaAlunos *lista, const char *cpf,
                               const Aluno *novos_dados);

/* Remove o aluno com o CPF informado, deslocando os seguintes uma posicao
 * para a esquerda (mantem a ordem).
 * Parametros: lista - lista de alunos; cpf - CPF do aluno a remover.
 * Retorno: ALUNO_OK; ALUNO_ERRO_PARAMETRO; ALUNO_ERRO_VAZIA;
 *          ALUNO_ERRO_NAO_ENCONTRADO. */
ResultadoAluno remover_aluno(ListaAlunos *lista, const char *cpf);

/* Imprime todos os alunos em ordem de CPF.
 * Parametros: lista - lista de alunos.
 * Retorno: nenhum (imprime aviso se a lista for NULL ou estiver vazia). */
void listar_alunos(const ListaAlunos *lista);

/* Informa quantos alunos estao cadastrados.
 * Parametros: lista - lista de alunos.
 * Retorno: quantidade de alunos (0 se a lista for NULL). */
int quantidade_alunos(const ListaAlunos *lista);

#endif /* ALUNO_H */
