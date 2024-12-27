#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>
#include <cmath>

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

Node* createNode(int ver) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = ver;
    newNode->next = NULL;
    return newNode;
}

int** generate_matrix(int size) {
    int** G = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        G[i] = (int*)malloc(size * sizeof(int));
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            G[i][j] = rand() % 2;
            if (i == j) G[i][j] = 0;
            G[j][i] = G[i][j];
        }
    }

    return G;
}

void printG(int** G, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", G[i][j]);
        }
        printf("\n");
    }
}

void free_matrix(int** G, int size) {
    for (int i = 0; i < size; i++) {
        free(G[i]);
    }
    free(G);
}

Node** matrixToList(int** G, int size) {
    Node** adjList = (Node**)malloc(size * sizeof(Node*));

    for (int i = 0; i < size; i++) {
        adjList[i] = NULL;
        for (int j = 0; j < size; j++) {
            if (G[i][j] == 1) {
                Node* newNode = createNode(j);
                // Добавляем в начало списка
                newNode->next = adjList[i];
                adjList[i] = newNode;
            }
        }
    }
    return adjList;
}

void printAdjacencyList(Node** adjList, int size) {
    for (int i = 0; i < size; i++) {
        printf("Vertex %d:", i);
        Node* temp = adjList[i];
        while (temp) {
            printf(" -> %d", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}

// удаление вершины из графа
int** delG(int** G, int size, int v) {
    int newSize = size - 1;
    int** Gtemp = (int**)malloc(newSize * sizeof(int*));
    for (int i = 0; i < newSize; i++) {
        Gtemp[i] = (int*)malloc(newSize * sizeof(int));
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i < v && j < v) {
                Gtemp[i][j] = G[i][j];
            }
            else if (i > v && j > v) {
                Gtemp[i - 1][j - 1] = G[i][j];
            }
            else if (i > v && j < v) {
                Gtemp[i - 1][j] = G[i][j];
            }
            else if (i < v && j > v) {
                Gtemp[i][j - 1] = G[i][j];
            }
        }
    }
    free_matrix(G, size);
    return Gtemp;
}


// отождествление вершин (объединение двух вершин в графе)
int** unionV(int** G, int size, int v1, int v2) {
    for (int i = 0; i < size; i++) {
        if (G[v2][i] == 1) {
            G[v1][i] = G[v2][i];
            G[i][v1] = G[i][v2];
        }
    }
    if (G[v1][v2] == 1) {
        G[v1][v1] = 1;
    }
    G = delG(G, size, v2);
    return G;
}

// Расщепление ребра между двумя вершинами
int** contrE(int** G, int size, int v1, int v2) {
    if (G[v1][v2] != 1) {
        printf("Ребра нет\n");
        return G;
    }
    else if (G[v1][v2] == 1) {
        G[v1][v2] = 0;
        G[v2][v1] = 0;

        for (int i = 0; i < size; i++) {
            if (G[v2][i] == 1) {
                G[v1][i] = G[v2][i];
                G[i][v1] = G[i][v2];
            }
        }
        G = delG(G, size, v2);
        return G;
    }
}

// разделение указанной вершины v в графе
int** SplitV(int** G, int size, int v) {
    int newSize = size + 1;
    int** Gtemp = (int**)malloc(newSize * sizeof(int*));
    for (int i = 0; i < newSize; i++) {
        Gtemp[i] = (int*)malloc(newSize * sizeof(int));
    }

    for (int i = 0; i < size; i++) {
        Gtemp[i][newSize - 1] = G[i][v];
        Gtemp[newSize - 1][i] = G[v][i];
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            Gtemp[i][j] = G[i][j];
        }
    }

    Gtemp[v][v] = 0;
    Gtemp[newSize - 1][newSize - 1] = 0;

    free_matrix(G, size);
    return Gtemp;
}

// объединение двух графов
int** unicoG(int** G1, int** G2, int size1, int size2) {
    int newSize = (size1 > size2) ? size1 : size2;
    int** Gtemp = (int**)malloc(newSize * sizeof(int*));
    for (int i = 0; i < newSize; i++) {
        Gtemp[i] = (int*)malloc(newSize * sizeof(int));
    }

    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size1; j++) {
            Gtemp[i][j] = G1[i][j];
        }
    }

    for (int i = 0; i < size2; i++) {
        for (int j = 0; j < size2; j++) {
            Gtemp[i][j] |= G2[i][j];
        }
    }

    return Gtemp;
}

// матрица смежности имеющая связи более крупного графа, которые присутствуют в меньшем
int** xorG(int** G1, int size1, int** G2, int size2) {
    int maxSize = (size1 > size2) ? size1 : size2;
    int** Gtemp = (int**)malloc(maxSize * sizeof(int*));
    for (int i = 0; i < maxSize; i++) {
        Gtemp[i] = (int*)malloc(maxSize * sizeof(int));
        for (int j = 0; j < maxSize; j++) {
            Gtemp[i][j] = 0;
        }
    }

    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size1; j++) {
            Gtemp[i][j] = G1[i][j];
        }
    }

    for (int i = 0; i < size2; i++) {
        for (int j = 0; j < size2; j++) {
            //XOR
            Gtemp[i][j] ^= G2[i][j];
        }
    }

    return Gtemp;
}

// кольцевая сумма двух графов
int** ringSum(int** G1, int size1, int** G2, int size2) {
    int newSize = size1 + size2;

    int** Gnew = (int**)malloc(newSize * sizeof(int*));
    for (int i = 0; i < newSize; i++) {
        Gnew[i] = (int*)malloc(newSize * sizeof(int));
        for (int j = 0; j < newSize; j++) {
            Gnew[i][j] = 0;
        }
    }

    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size1; j++) {
            Gnew[i][j] = G1[i][j];
        }
    }

    for (int i = 0; i < size2; i++) {
        for (int j = 0; j < size2; j++) {
            Gnew[size1 + i][size1 + j] = G2[i][j];
        }
    }

    Gnew[size1 - 1][size1] = 1;
    Gnew[size1][size1 - 1] = 1;
    Gnew[0][newSize - 1] = 1;
    Gnew[newSize - 1][0] = 1;

    return Gnew;
}

void showMenu() {
    printf("\nВыберите операцию:\n");
    printf("1. Удаление вершины\n");
    printf("2. Объединение вершин\n");
    printf("3. Расщепление ребра между вершинами\n");
    printf("4. Разделение вершины\n");
    printf("5. Объединение двух графов\n");
    printf("6. Кольцевая сумма двух графов\n");
    printf("7. XOR двух матриц\n");
    printf("8. Выход\n");
    printf("Ваш выбор: ");
}

int main() {
    srand(time(NULL));
    setlocale(LC_ALL, "Rus");
    int size1 = 0, size2 = 0, verticle1 = 0, verticle2 = 0;
    int choice, v1, v2;
    printf("Введите размер первой матрицы: ");
    scanf("%d", &size1);
    printf("Введите размер второй матрицы: ");
    scanf("%d", &size2);

    int** G1 = generate_matrix(size1);
    int** G2 = generate_matrix(size2);

    printf("\nПервая матрица:\n");
    printG(G1, size1);

    printf("\nВторая матрица:\n");
    printG(G2, size2);

    Node** adjList1 = matrixToList(G1, size1);
    Node** adjList2 = matrixToList(G2, size2);

    printf("\nСписок смежности первой матрицы:\n");
    printAdjacencyList(adjList1, size1);

    printf("\nСписок смежности второй матрицы:\n");
    printAdjacencyList(adjList2, size2);
    while (1) {
        showMenu();
        scanf("%d", &choice);

        switch (choice) {
        case 1: {
            printf("\nВведите вершину, которую хотите удалить из первой матрицы: ");
            scanf("%d", &verticle1);
            G1 = delG(G1, size1, verticle1);
            size1--;
            printG(G1, size1);

            printf("\nВведите вершину, которую хотите удалить из второй матрицы: ");
            scanf("%d", &verticle2);
            G2 = delG(G2, size2, verticle2);
            size2--;
            printG(G2, size2);
            break;
        }

        case 2: {
            printf("\nВведите две вершины для объединения в первой матрице: ");
            scanf("%d %d", &v1, &v2);
            G1 = unionV(G1, size1, v1, v2);
            size1--;
            printG(G1, size1);

            printf("\nВведите две вершины для объединения во второй матрице: ");
            scanf("%d %d", &v1, &v2);
            G2 = unionV(G2, size2, v1, v2);
            size2--;
            printG(G2, size2);
            break;
        }

        case 3: {
            printf("\nВведите две вершины для удаления ребра между вершинами в первой матрице: ");
            scanf("%d %d", &v1, &v2);
            G1 = contrE(G1, size1, v1, v2);
            printG(G1, size1);
            printf("\nВведите две вершины для удаления ребра между вершинами во второй матрице: ");
            scanf("%d %d", &v1, &v2);
            G2 = contrE(G2, size2, v1, v2);
            printG(G2, size2);
            break;
        }

        case 4: {
            printf("\nВведите вершину для разделения в первой матрице: ");
            scanf("%d", &v1);
            G1 = SplitV(G1, size1, v1);
            size1++;
            printG(G1, size1);

            printf("\nВведите вершину для разделения во второй матрице: ");
            scanf("%d", &v1);
            G2 = SplitV(G2, size2, v1);
            size2++;
            printG(G2, size2);
            break;
        }

        case 5: {
            printf("\nОбъединение двух графов:\n");
            G1 = unicoG(G1, G2, size1, size2);
            printG(G1, size1 > size2 ? size1 : size2);
            break;
        }

        case 6: {
            printf("\nКольцевая сумма двух графов:\n");
            int** G_ring = ringSum(G1, size1, G2, size2);
            printG(G_ring, size1 + size2 - 2);
            free_matrix(G_ring, size1 + size2 - 2);
            break;
        }

        case 7: {
            printf("\nXOR двух графов:\n");
            int** G_xor = xorG(G1, size1, G2, size2);
            printG(G_xor, (size1 > size2) ? size1 : size2);
            free_matrix(G_xor, (size1 > size2) ? size1 : size2);
            break;
        }

        case 8:
            printf("Выход из программы\n");
            free_matrix(G1, size1);
            free_matrix(G2, size2);
            for (int i = 0; i < size1; i++) {
                Node* temp = adjList1[i];
                while (temp) {
                    Node* toFree = temp;
                    temp = temp->next;
                    free(toFree);
                }
            }
            free(adjList1);

            for (int i = 0; i < size2; i++) {
                Node* temp = adjList2[i];
                while (temp) {
                    Node* toFree = temp;
                    temp = temp->next;
                    free(toFree);
                }
            }
            free(adjList2);

            return 0;

        default:
            printf("Неверный выбор. Попробуйте снова.\n");
            break;
        }
    }

    return 0;
}