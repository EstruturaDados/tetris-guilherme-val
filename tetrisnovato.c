#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// -----------------------------------------------------------------
// 1. DEFINIÇÕES E ESTRUTURAS
// -----------------------------------------------------------------

// Define a capacidade máxima da fila, conforme o requisito.
#define MAX_FILA 5

/**
 * @brief Estrutura que representa uma peça do jogo.
 * Possui um nome (tipo) e um id numérico.
 */
typedef struct {
    char nome;
    int id;
} Peca;

/**
 * @brief Estrutura da Fila Circular.
 * Contém o array de itens, os índices 'front' e 'rear'
 * e um 'count' para facilitar a verificação de cheia/vazia.
 */
typedef struct {
    Peca itens[MAX_FILA];
    int front; // Índice do primeiro elemento
    int rear;  // Índice do ÚLTIMO elemento (onde o próximo será inserido)
    int count; // Número atual de elementos na fila
} FilaCircular;

// ID global para garantir que cada peça seja única.
// (Simplificação para o Nível Novato)
int g_proximoId = 0;


// -----------------------------------------------------------------
// 2. FUNÇÕES AUXILIARES E DA FILA
// -----------------------------------------------------------------

/**
 * @brief Gera uma nova peça aleatória com um ID único.
 * Usa um contador global (g_proximoId) para o ID.
 * Sorteia um tipo de peça (I, O, T, L, S, Z, J).
 */
Peca gerarPeca() {
    Peca p;
    char tipos[] = "IOTLSJZ"; // 7 tipos de peça
    
    // Sorteia um tipo
    p.nome = tipos[rand() % 7];
    
    // Atribui o ID único e incrementa o contador
    p.id = g_proximoId++;
    
    return p;
}

/**
 * @brief Verifica se a fila está vazia.
 */
int estaVazia(FilaCircular *fila) {
    return (fila->count == 0);
}

/**
 * @brief Verifica se a fila está cheia.
 */
int estaCheia(FilaCircular *fila) {
    return (fila->count == MAX_FILA);
}

/**
 * @brief Insere (enqueue) uma nova peça no fim da fila.
 * A peça é criada automaticamente pela função gerarPeca().
 */
void enqueue(FilaCircular *fila) {
    if (estaCheia(fila)) {
        printf("\n>> ERRO: A fila esta cheia! Nao e possivel inserir.\n");
        return;
    }

    // Gera uma nova peça
    Peca novaPeca = gerarPeca();

    // Insere a peça na posição 'rear'
    fila->itens[fila->rear] = novaPeca;
    
    // Atualiza o índice 'rear' de forma circular
    fila->rear = (fila->rear + 1) % MAX_FILA;
    
    // Incrementa a contagem
    fila->count++;

    printf("\n>> Peca Inserida: [%c %d]\n", novaPeca.nome, novaPeca.id);
}

/**
 * @brief Remove (dequeue) a peça da frente da fila ("Jogar").
 */
void dequeue(FilaCircular *fila) {
    if (estaVazia(fila)) {
        printf("\n>> ERRO: A fila esta vazia! Nao e possivel jogar.\n");
        return;
    }

    // Pega a peça que está na frente
    Peca pecaJogada = fila->itens[fila->front];

    // Atualiza o índice 'front' de forma circular
    fila->front = (fila->front + 1) % MAX_FILA;
    
    // Decrementa a contagem
    fila->count--;

    printf("\n>> Peca Jogada: [%c %d]\n", pecaJogada.nome, pecaJogada.id);
}

// -----------------------------------------------------------------
// 3. FUNÇÕES DE EXIBIÇÃO E INICIALIZAÇÃO
// -----------------------------------------------------------------

/**
 * @brief Exibe o estado atual da fila, conforme formato solicitado.
 */
void visualizarFila(FilaCircular *fila) {
    printf("\nFila de Pecas: ");

    if (estaVazia(fila)) {
        printf("[VAZIA]");
    } else {
        // Itera 'count' vezes a partir do 'front'
        int indice = fila->front;
        for (int i = 0; i < fila->count; i++) {
            printf("[%c %d] ", fila->itens[indice].nome, fila->itens[indice].id);
            // Avança o índice de forma circular
            indice = (indice + 1) % MAX_FILA;
        }
    }
    printf("\n");
}

/**
 * @brief Exibe o menu de opções.
 */
void exibirMenu() {
    printf("\n1 - Jogar Peca (dequeue)\n");
    printf("2 - Inserir Nova Peca (enqueue)\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
}

/**
 * @brief Inicializa a fila com 5 peças automáticas, conforme requisito.
 */
void inicializarFila(FilaCircular *fila) {
    fila->front = 0;
    fila->rear = 0;
    fila->count = 0;

    printf("Inicializando fila com 5 pecas...\n");
    for (int i = 0; i < MAX_FILA; i++) {
        // Usamos a função enqueue interna, mas sem as verificações
        // pois sabemos que a fila está vazia.
        Peca p = gerarPeca();
        fila->itens[fila->rear] = p;
        fila->rear = (fila->rear + 1) % MAX_FILA;
        fila->count++;
    }
}

// -----------------------------------------------------------------
// 4. FUNÇÃO PRINCIPAL (MAIN)
// -----------------------------------------------------------------

int main() {
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    FilaCircular filaDePecas;
    inicializarFila(&filaDePecas);

    int opcao = -1;

    do {
        // 1. Mostra o estado atual da fila
        visualizarFila(&filaDePecas);
        
        // 2. Mostra o menu
        exibirMenu();

        // 3. Lê a opção do usuário
        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer de entrada em caso de erro (ex: digitou uma letra)
            while(getchar() != '\n');
            opcao = -1; // Força uma opção inválida
        }

        // 4. Executa a ação
        switch (opcao) {
            case 1:
                dequeue(&filaDePecas);
                break;
            case 2:
                enqueue(&filaDePecas);
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