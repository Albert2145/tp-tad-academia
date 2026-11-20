/*
 * turma.h - TAD Turma (inclui a Participacao dos alunos)
 *
 * Representa as aulas coletivas da academia (tabela Turma do banco) e os
 * alunos inscritos nelas (tabela Participacao).
 * Estruturas escolhidas:
 *   - GradeTurmas: array ESTATICO de turmas (a grade horaria e pequena e
 *     tem tamanho conhecido).
 *   - Participantes de cada turma: array ESTATICO com no maximo
 *     MAX_PARTICIPANTES posicoes (a capacidade da sala e conhecida).
 *   - Lista de espera de cada turma: FILA dinamica encadeada (quem chega
 *     primeiro ganha a primeira vaga que abrir; o tamanho e imprevisivel).
 */
#ifndef TURMA_H
#define TURMA_H

/* ---------- Tamanhos e limites ---------- */
#ifndef TAM_CPF
#define TAM_CPF   15   // "000.000.000-00" + '\0'
#endif
#ifndef TAM_DATA
#define TAM_DATA  11   // "AAAA-MM-DD" + '\0'
#endif
#define TAM_HORARIO        6   // "HH:MM" + '\0' (time do banco)
#define MAX_TURMAS        50   // turmas que cabem na grade horaria
#define MAX_PARTICIPANTES 30   // limite fisico de alunos por sala

/* ---------- Situacao da turma (varchar(50) do banco -> enum) ---------- */
typedef enum {
    TURMA_ATIVA = 0,   // aceitando inscricoes e com aulas acontecendo
    TURMA_SUSPENSA,    // aulas paradas temporariamente
    TURMA_ENCERRADA    // turma fechada
} SituacaoTurma;

/* ---------- Resultado da inscricao de um aluno ---------- */
typedef enum {
    INSCRICAO_CONFIRMADA = 0,   // havia vaga: aluno entrou na turma
    INSCRICAO_EM_ESPERA         // turma cheia: aluno entrou na fila
} StatusInscricao;

/* ---------- Codigos de retorno das operacoes ---------- */
typedef enum {
    TURMA_OK = 0,              // operacao realizada com sucesso
    TURMA_ERRO_PARAMETRO,      // ponteiro nulo ou dado invalido
    TURMA_ERRO_DUPLICADA,      // id de turma ou aluno ja cadastrado
    TURMA_ERRO_NAO_ENCONTRADA, // turma ou aluno nao encontrado
    TURMA_ERRO_VAZIA,          // grade/turma/fila sem elementos
    TURMA_ERRO_CHEIA,          // grade horaria sem espaco
    TURMA_ERRO_INATIVA,        // turma suspensa ou encerrada
    TURMA_ERRO_MEMORIA         // falha ao alocar memoria
} ResultadoTurma;

/* ---------- Participante ativo (linha de Participacao "ativa") ---------- */
typedef struct {
    char cpf_aluno[TAM_CPF];     // CHAVE dentro da turma
    char data_inicio[TAM_DATA];  // quando comecou a frequentar a turma
} Participante;

/* ---------- No da fila de espera (Participacao "em espera") ---------- */
typedef struct NoEspera {
    char cpf_aluno[TAM_CPF];      // aluno aguardando vaga
    char data_pedido[TAM_DATA];   // quando entrou na fila
    struct NoEspera *proximo;     // quem esta atras na fila
} NoEspera;

/* ---------- Fila de espera (FIFO): entra no fim, sai do inicio ---------- */
typedef struct {
    NoEspera *inicio;   // primeiro da fila: proximo a ganhar vaga
    NoEspera *fim;      // ultimo da fila: onde entra quem chega
    int quantidade;     // quantos alunos estao esperando
} FilaEspera;

/* ---------- Dados de uma turma ---------- */
typedef struct {
    int id;                                // CHAVE da turma
    int id_modalidade;                     // modalidade (chave estrangeira)
    char cpf_professor[TAM_CPF];           // professor (chave estrangeira)
    int dia_da_semana;                     // 1 = domingo ... 7 = sabado
    char horario_inicio[TAM_HORARIO];      // "HH:MM"
    char horario_termino[TAM_HORARIO];     // "HH:MM"
    int capacidade_maxima;                 // 1 .. MAX_PARTICIPANTES
    SituacaoTurma situacao;                // ativa, suspensa, encerrada
    Participante participantes[MAX_PARTICIPANTES]; // array estatico
    int quantidade_participantes;          // quantos estao inscritos
    FilaEspera espera;                     // fila de espera da turma
} Turma;

/* ---------- Grade horaria: array estatico de turmas ---------- */
typedef struct {
    Turma turmas[MAX_TURMAS];   // turmas cadastradas (ordem de cadastro)
    int quantidade;             // quantas turmas existem (n)
} GradeTurmas;

/* ================= Operacoes da grade (conjunto de turmas) ================= */

/* Deixa a grade vazia e pronta para uso (nao aloca memoria).
 * Parametros: grade - grade a inicializar.
 * Retorno: TURMA_OK, ou TURMA_ERRO_PARAMETRO se grade for NULL. */
ResultadoTurma inicializar_grade(GradeTurmas *grade);

/* Libera as filas de espera de todas as turmas e deixa a grade vazia.
 * Parametros: grade - grade a liberar (pode ser NULL).
 * Retorno: nenhum. */
void liberar_grade(GradeTurmas *grade);

/* Cadastra uma turma no fim da grade, sem participantes e com fila vazia.
 * Parametros: grade - grade de destino; nova - dados da turma (participantes
 *             e espera sao ignorados e zerados).
 * Retorno: TURMA_OK; TURMA_ERRO_PARAMETRO (NULL, dia fora de 1..7 ou
 *          capacidade fora de 1..MAX_PARTICIPANTES); TURMA_ERRO_DUPLICADA
 *          (id repetido); TURMA_ERRO_CHEIA (grade lotada). */
ResultadoTurma cadastrar_turma(GradeTurmas *grade, const Turma *nova);

/* Procura uma turma pelo id.
 * Parametros: grade - grade de turmas; id - id procurado.
 * Retorno: ponteiro para a turma, ou NULL se nao existir ou grade for NULL. */
Turma *buscar_turma_por_id(GradeTurmas *grade, int id);

/* Imprime as turmas ativas de um dia da semana, com vagas livres e tamanho
 * da fila de espera.
 * Parametros: grade - grade de turmas; dia_da_semana - 1..7.
 * Retorno: quantidade de turmas impressas, ou -1 se parametro invalido. */
int listar_turmas_do_dia(const GradeTurmas *grade, int dia_da_semana);

/* Muda a situacao de uma turma (suspender, encerrar, reativar).
 * Parametros: grade - grade de turmas; id - turma a alterar;
 *             nova_situacao - valor de SituacaoTurma.
 * Retorno: TURMA_OK; TURMA_ERRO_PARAMETRO; TURMA_ERRO_NAO_ENCONTRADA. */
ResultadoTurma alterar_situacao_turma(GradeTurmas *grade, int id,
                                      SituacaoTurma nova_situacao);

/* ============ Operacoes de participacao (dentro de uma turma) ============ */

/* Inscreve um aluno: com vaga, entra no fim do array de participantes;
 * turma cheia, entra no FIM da fila de espera.
 * Parametros: turma - turma de destino; cpf_aluno - CPF do aluno;
 *             data - "AAAA-MM-DD"; status - saida: INSCRICAO_CONFIRMADA ou
 *             INSCRICAO_EM_ESPERA (pode ser NULL).
 * Retorno: TURMA_OK; TURMA_ERRO_PARAMETRO; TURMA_ERRO_INATIVA;
 *          TURMA_ERRO_DUPLICADA (ja inscrito ou na fila);
 *          TURMA_ERRO_MEMORIA (falha ao criar o no da fila). */
ResultadoTurma inscrever_aluno(Turma *turma, const char *cpf_aluno,
                               const char *data, StatusInscricao *status);

/* Cancela a participacao de um aluno ativo: o ultimo do array ocupa a vaga
 * (sem deslocar) e o PRIMEIRO da fila, se houver, e promovido.
 * Parametros: turma - turma; cpf_aluno - CPF do participante;
 *             data - hoje "AAAA-MM-DD" (inicio do promovido);
 *             cpf_promovido - saida: CPF de quem ganhou a vaga, ou "" se a
 *             fila estava vazia (pode ser NULL; senao, ao menos TAM_CPF).
 * Retorno: TURMA_OK; TURMA_ERRO_PARAMETRO; TURMA_ERRO_VAZIA;
 *          TURMA_ERRO_NAO_ENCONTRADA. */
ResultadoTurma cancelar_participacao(Turma *turma, const char *cpf_aluno,
                                     const char *data, char *cpf_promovido);

/* Retira da fila de espera um aluno que desistiu de esperar (pode estar em
 * qualquer posicao da fila). Religa o no anterior ao seguinte.
 * Parametros: turma - turma; cpf_aluno - CPF de quem desistiu.
 * Retorno: TURMA_OK; TURMA_ERRO_PARAMETRO; TURMA_ERRO_VAZIA;
 *          TURMA_ERRO_NAO_ENCONTRADA. */
ResultadoTurma desistir_da_espera(Turma *turma, const char *cpf_aluno);

/* Verifica se o aluno e participante ativo da turma (check-in na aula).
 * Parametros: turma - turma; cpf_aluno - CPF do aluno.
 * Retorno: 1 se esta inscrito, 0 se nao esta (ou parametro NULL). */
int aluno_esta_inscrito(const Turma *turma, const char *cpf_aluno);

/* Informa a posicao do aluno na fila de espera.
 * Parametros: turma - turma; cpf_aluno - CPF do aluno.
 * Retorno: posicao (1 = proximo a ganhar vaga), 0 se nao esta na fila,
 *          -1 se parametro NULL. */
int posicao_na_espera(const Turma *turma, const char *cpf_aluno);

/* Imprime a lista de chamada: participantes ativos da turma.
 * Parametros: turma - turma.
 * Retorno: quantidade de participantes, ou -1 se turma for NULL. */
int listar_participantes(const Turma *turma);

/* Imprime a fila de espera, do primeiro ao ultimo.
 * Parametros: turma - turma.
 * Retorno: quantidade de alunos na fila, ou -1 se turma for NULL. */
int listar_espera(const Turma *turma);

#endif /* TURMA_H */
