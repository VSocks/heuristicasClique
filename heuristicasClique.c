#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NODES 100
#define POPULATION_SIZE 50
#define GENERATIONS 100
#define MUTATION_RATE 0.1

// Função para ler a matriz de adjacência de um arquivo
void read_graph(const char *filename, int graph[MAX_NODES][MAX_NODES], int *n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    fscanf(file, "%d", n);  // Lê o número de nós
    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *n; j++) {
            fscanf(file, "%d", &graph[i][j]);
        }
    }

    fclose(file);
}

// Função para verificar se um conjunto de vértices forma um clique
int is_clique(int graph[MAX_NODES][MAX_NODES], int clique[], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (graph[clique[i]][clique[j]] == 0) {
                return 0;  // Não é um clique
            }
        }
    }
    return 1;  // É um clique
}

// Função para buscar um clique maior com o VNS
int vns(int graph[MAX_NODES][MAX_NODES], int n) {
    int best_clique[MAX_NODES], best_size = 0;
    int clique[MAX_NODES];
    int size = 0;

    // Inicializa com um clique de um único vértice
    clique[size++] = 0;

    // Busca na vizinhança 1 (adicionando vértices)
    for (int i = 1; i < n; i++) {
        clique[size++] = i;

        // Verifica se é um clique
        if (is_clique(graph, clique, size)) {
            if (size > best_size) {
                best_size = size;
                for (int j = 0; j < size; j++) {
                    best_clique[j] = clique[j];
                }
            }
        } else {
            size--;
        }
    }

    return best_size;
}

// Função para avaliar o fitness de um indivíduo
int evaluate_fitness(int graph[MAX_NODES][MAX_NODES], int n, int individual[MAX_NODES]) {
    int clique[MAX_NODES], size = 0;

    // Constrói o conjunto do indivíduo
    for (int i = 0; i < n; i++) {
        if (individual[i]) {
            clique[size++] = i;
        }
    }

    // Verifica se o conjunto é um clique
    return is_clique(graph, clique, size) ? size : 0;
}

// Função para gerar a população inicial
void initialize_population(int population[POPULATION_SIZE][MAX_NODES], int n) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        for (int j = 0; j < n; j++) {
            population[i][j] = rand() % 2;  // 0 ou 1
        }
    }
}

// Função para realizar cruzamento
void crossover(int parent1[MAX_NODES], int parent2[MAX_NODES], int offspring[MAX_NODES], int n) {
    int crossover_point = rand() % n;
    for (int i = 0; i < n; i++) {
        offspring[i] = (i < crossover_point) ? parent1[i] : parent2[i];
    }
}

// Função para realizar mutação
void mutate(int individual[MAX_NODES], int n) {
    for (int i = 0; i < n; i++) {
        if ((double)rand() / RAND_MAX < MUTATION_RATE) {
            individual[i] = 1 - individual[i];  // Inverte o bit
        }
    }
}

// Algoritmo evolutivo para encontrar o maior clique
int evolutionary_algorithm(int graph[MAX_NODES][MAX_NODES], int n) {
    int population[POPULATION_SIZE][MAX_NODES];
    int fitness[POPULATION_SIZE];
    int best_clique_size = 0;

    // Inicializa a população
    initialize_population(population, n);

    for (int generation = 0; generation < GENERATIONS; generation++) {
        // Avalia a população
        for (int i = 0; i < POPULATION_SIZE; i++) {
            fitness[i] = evaluate_fitness(graph, n, population[i]);
            if (fitness[i] > best_clique_size) {
                best_clique_size = fitness[i];
            }
        }

        // Seleção e criação de nova geração
        int new_population[POPULATION_SIZE][MAX_NODES];
        for (int i = 0; i < POPULATION_SIZE; i++) {
            int parent1 = rand() % POPULATION_SIZE;
            int parent2 = rand() % POPULATION_SIZE;

            crossover(population[parent1], population[parent2], new_population[i], n);
            mutate(new_population[i], n);
        }

        // Atualiza a população
        for (int i = 0; i < POPULATION_SIZE; i++) {
            for (int j = 0; j < n; j++) {
                population[i][j] = new_population[i][j];
            }
        }
    }

    return best_clique_size;
}

int main() {
    int graph[MAX_NODES][MAX_NODES];
    int n;
    clock_t start, end;

    for (int i = 1; i <= 10; i++) {
        char filename[20];
        sprintf(filename, "instancia%d.txt", i);
        read_graph(filename, graph, &n);

        // Busca pelo VNS
        start = clock();
        int vns_clique_size = vns(graph, n);
        end = clock();
        double vns_time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

        printf("Instancia %d - Maior clique (VNS): %d, Tempo: %.6f segundos\n", i, vns_clique_size, vns_time_taken);

        // Busca pelo Algoritmo Evolutivo
        start = clock();
        int ea_clique_size = evolutionary_algorithm(graph, n);
        end = clock();
        double ea_time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

        printf("Instancia %d - Maior clique (Evolutivo): %d, Tempo: %.6f segundos\n", i, ea_clique_size, ea_time_taken);
    }

    return 0;
}