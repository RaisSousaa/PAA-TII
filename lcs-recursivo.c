#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct {
    char nome[64];
    int n, m;
    char *a;
    char *b;
} CasoLCS;

/* ===== Timer (Windows) ===== */
static double agora_segundos(void) {
    static LARGE_INTEGER freq;
    static int init = 0;
    LARGE_INTEGER counter;

    if (!init) {
        QueryPerformanceFrequency(&freq);
        init = 1;
    }
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
}

/* ===== Leitura ===== */
static char *ler_linha(FILE *f) {
    char buffer[300000];
    if (!fgets(buffer, (int)sizeof(buffer), f)) return NULL;

    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[--len] = '\0';
    }

    char *s = (char*)malloc(len + 1);
    if (!s) { fprintf(stderr, "malloc falhou\n"); exit(1); }
    memcpy(s, buffer, len + 1);
    return s;
}

static CasoLCS *ler_casos(const char *arquivo, int *qtd_out) {
    FILE *f = fopen(arquivo, "r");
    if (!f) { fprintf(stderr, "Erro ao abrir %s\n", arquivo); exit(1); }

    int k = 0;
    if (fscanf(f, "%d", &k) != 1 || k <= 0) {
        fprintf(stderr, "Arquivo invalido\n");
        exit(1);
    }

    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF) {}

    CasoLCS *casos = (CasoLCS*)calloc((size_t)k, sizeof(CasoLCS));
    if (!casos) exit(1);

    for (int i = 0; i < k; i++) {
        fscanf(f, "%63s %d %d", casos[i].nome, &casos[i].n, &casos[i].m);
        while ((c = fgetc(f)) != '\n' && c != EOF) {}

        casos[i].a = ler_linha(f);
        casos[i].b = ler_linha(f);

        casos[i].n = (int)strlen(casos[i].a);
        casos[i].m = (int)strlen(casos[i].b);
    }

    fclose(f);
    *qtd_out = k;
    return casos;
}

static void liberar_casos(CasoLCS *casos, int qtd) {
    for (int i = 0; i < qtd; i++) {
        free(casos[i].a);
        free(casos[i].b);
    }
    free(casos);
}

/* ===== LCS Recursivo ===== */
static int lcs_rec(const char *a, const char *b, int i, int j) {
    if (i == 0 || j == 0) return 0;
    if (a[i - 1] == b[j - 1]) return 1 + lcs_rec(a, b, i - 1, j - 1);
    int x = lcs_rec(a, b, i - 1, j);
    int y = lcs_rec(a, b, i, j - 1);
    return (x > y) ? x : y;
}

int main(int argc, char **argv) {
    const char *arquivo = (argc >= 2) ? argv[1] : "testes_lcs.txt";

    int qtd = 0;
    CasoLCS *casos = ler_casos(arquivo, &qtd);

    for (int i = 0; i < qtd; i++) {
        double t0 = agora_segundos();
        int lcs = lcs_rec(casos[i].a, casos[i].b, casos[i].n, casos[i].m);
        double t1 = agora_segundos();

        printf("Caso: %s\n", casos[i].nome);
        printf("Tamanho da entrada: n = %d, m = %d\n\n", casos[i].n, casos[i].m);

        printf("Versao: Recursiva\n");
        printf("Tempo de execucao (s): %.9f\n", t1 - t0);
        printf("Memoria dinamica (bytes): 0\n");
        printf("Resultado LCS: %d\n\n", lcs);
    }

    liberar_casos(casos, qtd);
    return 0;
}
