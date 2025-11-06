#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// -----------------------------------------------------------------
// 1. DEFINIÇÕES E ESTRUTURAS
// -----------------------------------------------------------------

#define MAX_FILA 5
#define MAX_PILHA 3

/**
 * @brief Estrutura da Peca
 */
typedef struct {
    char nome;
    int id;
} Peca;

/**
 * @brief Estrutura da Fila Circular
 */
typedef struct {
    Peca itens[MAX_FILA];
    int front; 
    int rear;  
    int count; 
} FilaCircular;

/**
 * @brief Estrutura da Pilha Linear
 */
typedef struct {
    Peca itens[MAX_PILHA];
    int topo; // -1 = vazia
} PilhaLinear;

/**
 * @brief Estrutura para salvar o estado do jogo (para o UNDO)
 * Contém cópias completas da fila, da pilha e do contador de ID.
 */
typedef struct {
    FilaCircular fila;
    PilhaLinear pilha;
    int proximoId; // Essencial para o Undo funcionar corretamente
} EstadoJogo;


// -----------------------------------------------------------------
// 2. FUNÇÕES DAS ESTRUTURAS (FILA E PILHA)
// -----------------------------------------------------------------

// --- Funções da Fila ---
void inicializarFila(FilaCircular *fila) {
    fila->front = 0;
    fila->rear = 0;
    fila->count = 0;
}
int filaEstaVazia(FilaCircular *fila) { return (fila->count == 0); }
int filaEstaCheia(FilaCircular *fila) { return (fila->count == MAX_FILA); }

void enqueue(FilaCircular *fila, Peca p) {
    if (filaEstaCheia(fila)) return; // Guarda de segurança
    fila->itens[fila->rear] = p;
    fila->rear = (fila->rear + 1) % MAX_FILA;
    fila->count++;
}
Peca dequeue(FilaCircular *fila) {
    Peca pecaJogada = fila->itens[fila->front];
    fila->front = (fila->front + 1) % MAX_FILA;
    fila->count--;
    return pecaJogada;
}

// --- Funções da Pilha ---
void inicializarPilha(PilhaLinear *pilha) {
    pilha->topo = -1;
}
int pilhaEstaVazia(PilhaLinear *pilha) { return (pilha->topo == -1); }
int pilhaEstaCheia(PilhaLinear *pilha) { return (pilha->topo == MAX_PILHA - 1); }

void push(PilhaLinear *pilha, Peca p) {
    if (pilhaEstaCheia(pilha)) return; // Guarda de segurança
    pilha->topo++;
    pilha->itens[pilha->topo] = p;
}
Peca pop(PilhaLinear *pilha) {
    Peca pecaRetirada = pilha->itens[pilha->topo];
    pilha->topo--;
    return pecaRetirada;
}


// -----------------------------------------------------------------
// 3. FUNÇÕES DE GERENCIAMENTO DO JOGO
// -----------------------------------------------------------------

/**
 * @brief Gera uma nova peça.
 * Agora recebe o ponteiro do contador de ID do estado do jogo.
 */
Peca gerarPeca(int *idCounter) {
    Peca p;
    char tipos[] = "IOTLSJZ";
    p.nome = tipos[rand() % 7];
    p.id = (*idCounter)++; // Usa e incrementa o ID do estado
    return p;
}

/**
 * @brief Salva um estado (origem) em outro (destino).
 * Isso é o coração da função "Desfazer".
 */
void salvarEstado(EstadoJogo *destino, EstadoJogo *origem) {
    // A atribuição de structs em C faz uma cópia byte-a-byte
    // Isso funciona perfeitamente para nossas estruturas.
    *destino = *origem;
}

/**
 * @brief Função de conveniência para repor a fila.
 * Mantém a fila com 5 peças, como no Nível Aventureiro.
 */
void reporPecaFila(EstadoJogo *estado) {
    if (!filaEstaCheia(&estado->fila)) {
        Peca novaPeca = gerarPeca(&estado->proximoId);
        enqueue(&estado->fila, novaPeca);
        printf(">> Nova Peca [%c %d] entrou na fila.\n", novaPeca.nome, novaPeca.id);
    }
}

// -----------------------------------------------------------------
// 4. FUNÇÕES DE EXIBIÇÃO
// -----------------------------------------------------------------

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

void visualizarPilha(PilhaLinear *pilha) {
    printf("Pilha de Reserva (Topo -> Base): ");
    if (pilhaEstaVazia(pilha)) {
        printf("[VAZIA]");
    } else {
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[%c %d] ", pilha->itens[i].nome, pilha->itens[i].id);
        }
    }
    printf("\n");
}

void exibirMenu() {
    printf("\nOpcoes:\n");
    printf("1 - Jogar peca da frente da fila\n");
    printf("2 - Enviar peca da fila para reserva (pilha)\n");
    printf("3 - Usar peca da reserva (pilha)\n");
    printf("4 - Trocar peca da frente da fila com o topo da pilha\n");
    printf("5 - Desfazer ultima jogada\n");
    printf("6 - Inverter fila com pilha (troca 3x3)\n");
    printf("0 - Sair\n");
    printf("Opcao: ");
}

// -----------------------------------------------------------------
// 5. FUNÇÕES DAS AÇÕES PRINCIPAIS
// -----------------------------------------------------------------

// Ação 1: Jogar Peça (Dequeue + Reposição)
void acaoJogar(EstadoJogo *estado) {
    if (filaEstaVazia(&estado->fila)) {
        printf("\n>> ERRO: Fila vazia!\n");
        return;
    }
    Peca jogada = dequeue(&estado->fila);
    printf("\n>> Peca Jogada: [%c %d]\n", jogada.nome, jogada.id);
    reporPecaFila(estado);
}

// Ação 2: Reservar Peça (Dequeue -> Push + Reposição)
void acaoReservar(EstadoJogo *estado) {
    if (pilhaEstaCheia(&estado->pilha)) {
        printf("\n>> ERRO: Pilha de reserva esta cheia!\n");
        return;
    }
    if (filaEstaVazia(&estado->fila)) {
        printf("\n>> ERRO: Fila vazia!\n");
        return;
    }
    Peca reservada = dequeue(&estado->fila);
    push(&estado->pilha, reservada);
    printf("\n>> Peca Reservada: [%c %d]\n", reservada.nome, reservada.id);
    reporPecaFila(estado);
}

// Ação 3: Usar Peça Reservada (Pop)
void acaoUsarReserva(EstadoJogo *estado) {
    if (pilhaEstaVazia(&estado->pilha)) {
        printf("\n>> ERRO: Pilha de reserva esta vazia!\n");
        return;
    }
    Peca usada = pop(&estado->pilha);
    printf("\n>> Peca Usada da Reserva: [%c %d]\n", usada.nome, usada.id);
}

// Ação 4: Troca Topo-Frente (Swap)
void acaoTrocarTopoFrente(EstadoJogo *estado) {
    if (filaEstaVazia(&estado->fila)) {
        printf("\n>> ERRO: Fila vazia!\n");
        return;
    }
    if (pilhaEstaVazia(&estado->pilha)) {
        printf("\n>> ERRO: Pilha vazia!\n");
        return;
    }

    // Troca direta de itens nos arrays
    // Pega o índice real do item da frente da fila
    int indiceFrenteFila = estado->fila.front;
    int indiceTopoPilha = estado->pilha.topo;

    Peca temp = estado->fila.itens[indiceFrenteFila];
    estado->fila.itens[indiceFrenteFila] = estado->pilha.itens[indiceTopoPilha];
    estado->pilha.itens[indiceTopoPilha] = temp;
    
    printf("\n>> Troca Topo/Frente realizada.\n");
}

// Ação 6: Trocar 3x3 (Complexo)
void acaoInverter3x3(EstadoJogo *estado) {
    // Esta operação só funciona se ambas estiverem cheias (3 na pilha, 5 na fila)
    if (estado->fila.count < 3 || estado->pilha.topo < 2) {
        printf("\n>> ERRO: Acao requer 3 pecas na pilha e pelo menos 3 na fila.\n");
        return;
    }

    // 1. Armazena peças temporárias
    Peca tempPilha[3];
    Peca tempFila[3];
    int i;

    // 2. Salva o resto da fila (se houver)
    int restoCount = estado->fila.count - 3;
    Peca tempRestoFila[MAX_FILA - 3];

    // 3. Esvazia 3 da Pilha (Topo -> Base)
    // Ex: [Z, Y, X] -> tempPilha = {Z, Y, X}
    for (i = 0; i < 3; i++) {
        tempPilha[i] = pop(&estado->pilha);
    }

    // 4. Esvazia 3 da Fila (Frente -> Tras)
    // Ex: [A, B, C] D E -> tempFila = {A, B, C}
    for (i = 0; i < 3; i++) {
        tempFila[i] = dequeue(&estado->fila);
    }
    
    // 5. Salva o resto da fila
    // Ex: D E -> tempRestoFila = {D, E}
    for (i = 0; i < restoCount; i++) {
        tempRestoFila[i] = dequeue(&estado->fila);
    }
    // Fila agora está vazia

    // 6. Empilha as peças da Fila na Pilha (na ordem inversa)
    // Ex: tempFila = {A, B, C} -> push(C), push(B), push(A)
    // Nova Pilha (Topo -> Base): [A, B, C] (Invertido da imagem, mas consistente)
    // Correção para BATER com a imagem: [T 2][L 1][I 0]
    // tempFila[0]=I0, tempFila[1]=L1, tempFila[2]=T2
    // push(tempFila[2]) -> T2
    // push(tempFila[1]) -> L1
    // push(tempFila[0]) -> I0
    for (i = 2; i >= 0; i--) {
        push(&estado->pilha, tempFila[i]);
    }

    // 7. Enfileira as peças da Pilha na Fila (na ordem)
    // Ex: tempPilha = {Z, Y, X} -> enqueue(Z), enqueue(Y), enqueue(X)
    // Correção para BATER com a imagem: [O 8][L 7][T 6]
    // tempPilha[0]=O8, tempPilha[1]=L7, tempPilha[2]=T6
    // enqueue(tempPilha[0]) -> O8
    // enqueue(tempPilha[1]) -> L7
    // enqueue(tempPilha[2]) -> T6
    for (i = 0; i < 3; i++) {
        enqueue(&estado->fila, tempPilha[i]);
    }

    // 8. Devolve o resto para a fila
    // Ex: enqueue(D), enqueue(E)
    for (i = 0; i < restoCount; i++) {
        enqueue(&estado->fila, tempRestoFila[i]);
    }

    printf("\n>> Troca 3x3 realizada.\n");
}


// -----------------------------------------------------------------
// 6. FUNÇÃO PRINCIPAL (MAIN)
// -----------------------------------------------------------------

int main() {
    srand(time(NULL));

    EstadoJogo estadoAtual;
    EstadoJogo estadoAnterior;
    int opcao = -1;

    // --- Inicialização do Estado Atual ---
    inicializarFila(&estadoAtual.fila);
    inicializarPilha(&estadoAtual.pilha);
    estadoAtual.proximoId = 0;

    // Preenche a fila inicial com 5 peças
    printf("Inicializando fila com %d pecas...\n", MAX_FILA);
    for (int i = 0; i < MAX_FILA; i++) {
        // Gera peças usando o contador de ID do estado
        enqueue(&estadoAtual.fila, gerarPeca(&estadoAtual.proximoId));
    }
    
    // Salva o estado inicial caso o usuário queira desfazer a primeira jogada
    salvarEstado(&estadoAnterior, &estadoAtual);

    // --- Loop Principal ---
    do {
        // 1. Mostra o estado atual
        printf("\n=== Estado Atual ===\n");
        visualizarFila(&estadoAtual.fila);
        visualizarPilha(&estadoAtual.pilha);
        
        // 2. Mostra o menu
        exibirMenu();

        // 3. Lê a opção
        if (scanf("%d", &opcao) != 1) {
            while(getchar() != '\n'); // Limpa buffer
            opcao = -1;
        }

        // 4. Salva o estado ANTES de executar a ação
        // Não salva se a ação for Sair (0) ou Desfazer (5)
        if (opcao != 0 && opcao != 5) {
            salvarEstado(&estadoAnterior, &estadoAtual);
        }

        // 5. Executa a ação
        switch (opcao) {
            case 1:
                acaoJogar(&estadoAtual);
                break;
            case 2:
                acaoReservar(&estadoAtual);
                break;
            case 3:
                acaoUsarReserva(&estadoAtual);
                break;
            case 4:
                acaoTrocarTopoFrente(&estadoAtual);
                break;
            case 5: // Desfazer
                salvarEstado(&estadoAtual, &estadoAnterior);
                printf("\n>> Ultima acao desfeita.\n");
                break;
            case 6:
                acaoInverter3x3(&estadoAtual);
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