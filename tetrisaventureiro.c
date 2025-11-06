#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// -----------------------------------------------------------------
// 1. DEFINIÇÕES E ESTRUTURAS
// -----------------------------------------------------------------

// Capacidades máximas das estruturas
#define MAX_FILA 5
#define MAX_PILHA 3

/**
 * @brief Estrutura da Peca (sem alterações)
 */
typedef struct {
    char nome;
    int id;
} Peca;

/**
 * @brief Estrutura da Fila Circular (sem alterações)
 */
typedef struct {
    Peca itens[MAX_FILA];
    int front; 
    int rear;  
    int count; 
} FilaCircular;

/**
 * @brief Estrutura da Pilha Linear (NOVA)
 */
typedef struct {
    Peca itens[MAX_PILHA];
    int topo; // Índice do elemento no topo
} PilhaLinear;

// ID global para peças únicas
int g_proximoId = 0;


// -----------------------------------------------------------------
// 2. FUNÇÕES AUXILIARES E GERAIS
// -----------------------------------------------------------------

/**
 * @brief Gera uma nova peça aleatória (sem alterações)
 */
Peca gerarPeca() {
    Peca p;
    char tipos[] = "IOTLSJZ";
    p.nome = tipos[rand() % 7];
    p.id = g_proximoId++;
    return p;
}


// -----------------------------------------------------------------
// 3. FUNÇÕES DA FILA CIRCULAR
// (Leves modificações para retornar/receber Peças)
// -----------------------------------------------------------------

/**
 * @brief Inicializa a fila.
 */
void inicializarFila(FilaCircular *fila) {
    fila->front = 0;
    fila->rear = 0;
    fila->count = 0;
}

int filaEstaVazia(FilaCircular *fila) {
    return (fila->count == 0);
}

int filaEstaCheia(FilaCircular *fila) {
    return (fila->count == MAX_FILA);
}

/**
 * @brief Insere (enqueue) uma Peca específica na fila.
 * (Modificado para receber a Peca, ao invés de gerá-la internamente)
 */
void enqueue(FilaCircular *fila, Peca p) {
    if (filaEstaCheia(fila)) {
        // Isso não deve acontecer na lógica principal, mas é uma boa prática
        printf(">> ERRO: Tentativa de inserir em fila cheia!\n");
        return;
    }
    fila->itens[fila->rear] = p;
    fila->rear = (fila->rear + 1) % MAX_FILA;
    fila->count++;
}

/**
 * @brief Remove (dequeue) a peça da frente e a RETORNA.
 * (Modificado para retornar a Peca)
 */
Peca dequeue(FilaCircular *fila) {
    // A lógica principal deve checar se está vazia antes de chamar
    Peca pecaJogada = fila->itens[fila->front];
    fila->front = (fila->front + 1) % MAX_FILA;
    fila->count--;
    return pecaJogada;
}


// -----------------------------------------------------------------
// 4. FUNÇÕES DA PILHA LINEAR (NOVAS)
// -----------------------------------------------------------------

/**
 * @brief Inicializa a pilha.
 */
void inicializarPilha(PilhaLinear *pilha) {
    pilha->topo = -1; // -1 indica que a pilha está vazia
}

int pilhaEstaVazia(PilhaLinear *pilha) {
    return (pilha->topo == -1);
}

int pilhaEstaCheia(PilhaLinear *pilha) {
    return (pilha->topo == MAX_PILHA - 1);
}

/**
 * @brief Insere (push) uma Peca no topo da pilha.
 */
void push(PilhaLinear *pilha, Peca p) {
    // A lógica principal deve checar se está cheia antes
    pilha->topo++;
    pilha->itens[pilha->topo] = p;
}

/**
 * @brief Remove (pop) a Peca do topo da pilha e a RETORNA.
 */
Peca pop(PilhaLinear *pilha) {
    // A lógica principal deve checar se está vazia antes
    Peca pecaRetirada = pilha->itens[pilha->topo];
    pilha->topo--;
    return pecaRetirada;
}


// -----------------------------------------------------------------
// 5. FUNÇÕES DE EXIBIÇÃO E MENU
// -----------------------------------------------------------------

/**
 * @brief Exibe o estado atual da fila (sem alterações)
 */
void visualizarFila(FilaCircular *fila) {
    printf("Fila de Pecas: ");
    if (filaEstaVazia(fila)) {
        printf("[VAZIA]");
    } else {
        int indice = fila->front;
        for (int i = 0; i < fila->count; i++) {
            printf("[%c %d] ", fila->itens[indice].nome, fila->itens[indice].id);
            indice = (indice + 1) % MAX_FILA;
        }
    }
    printf("\n");
}

/**
 * @brief Exibe o estado atual da pilha (NOVA)
 * Formato: (Topo -> Base)
 */
void visualizarPilha(PilhaLinear *pilha) {
    printf("Pilha de Reserva (Topo -> Base): ");
    if (pilhaEstaVazia(pilha)) {
        printf("[VAZIA]");
    } else {
        // Itera do topo (último a entrar) até a base (primeiro)
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[%c %d] ", pilha->itens[i].nome, pilha->itens[i].id);
        }
    }
    printf("\n");
}

/**
 * @brief Exibe o novo menu (Modificado)
 */
void exibirMenu() {
    printf("\n1 - Jogar Peca\n");
    printf("2 - Reservar Peca\n");
    printf("3 - Usar Peca Reservada\n");
    printf("0 - Sair\n");
    printf("Opcao: ");
}

/**
 * @brief Função de conveniência para repor a fila.
 * Sempre que uma peça sai (dequeue), esta função é chamada
 * para gerar uma nova e manter a fila com 5 peças.
 */
void reporPecaFila(FilaCircular *fila) {
    Peca novaPeca = gerarPeca();
    enqueue(fila, novaPeca);
    printf(">> Nova Peca entrou na fila: [%c %d]\n", novaPeca.nome, novaPeca.id);
}

// -----------------------------------------------------------------
// 6. FUNÇÃO PRINCIPAL (MAIN)
// -----------------------------------------------------------------

int main() {
    srand(time(NULL));

    FilaCircular filaDePecas;
    PilhaLinear pilhaDeReserva;

    // Inicializa as estruturas
    inicializarFila(&filaDePecas);
    inicializarPilha(&pilhaDeReserva);

    // Preenche a fila inicial com 5 peças
    printf("Inicializando fila com %d pecas...\n", MAX_FILA);
    for (int i = 0; i < MAX_FILA; i++) {
        enqueue(&filaDePecas, gerarPeca());
    }

    int opcao = -1;

    do {
        // 1. Mostra o estado atual
        printf("\n----------------------------------------\n");
        visualizarFila(&filaDePecas);
        visualizarPilha(&pilhaDeReserva);
        
        // 2. Mostra o menu
        exibirMenu();

        // 3. Lê a opção
        if (scanf("%d", &opcao) != 1) {
            while(getchar() != '\n'); // Limpa buffer
            opcao = -1;
        }

        // 4. Executa a ação
        switch (opcao) {
            case 1: // Jogar Peça
                // 1.1 Remove da fila
                // (Não precisamos verificar se está vazia, pois ela sempre é reposta)
                {
                    Peca jogada = dequeue(&filaDePecas);
                    printf("\n>> Peca Jogada: [%c %d]\n", jogada.nome, jogada.id);

                    // 1.2 Repõe na fila (para manter 5)
                    reporPecaFila(&filaDePecas);
                }
                break;

            case 2: // Reservar Peça
                // 2.1 Verifica se a pilha NÃO está cheia
                if (pilhaEstaCheia(&pilhaDeReserva)) {
                    printf("\n>> ERRO: Pilha de reserva esta cheia! (Max: %d)\n", MAX_PILHA);
                } else {
                    // 2.2 Tira da fila
                    Peca reservada = dequeue(&filaDePecas);

                    // 2.3 Coloca na pilha
                    push(&pilhaDeReserva, reservada);
                    printf("\n>> Peca Reservada: [%c %d]\n", reservada.nome, reservada.id);

                    // 2.4 Repõe na fila (para manter 5)
                    reporPecaFila(&filaDePecas);
                }
                break;

            case 3: // Usar Peça Reservada
                // 3.1 Verifica se a pilha NÃO está vazia
                if (pilhaEstaVazia(&pilhaDeReserva)) {
                    printf("\n>> ERRO: Pilha de reserva esta vazia!\n");
                } else {
                    // 3.2 Tira da pilha (e "usa" = descarta)
                    Peca usada = pop(&pilhaDeReserva);
                    printf("\n>> Peca Usada da Reserva: [%c %d]\n", usada.nome, usada.id);
                    // (A fila não é afetada nesta operação)
                }
                break;

            case 0:
                printf("\nSaindo do programa...\n");
                break;

            default:
                printf("\n>> Opcao invalida. Tente novamente.\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}