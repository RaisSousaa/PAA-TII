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
    if (!s) exit(1);
    memcpy(s, buffer, len + 1);
    return s;
}

static CasoLCS *ler_casos(const char *arquivo, int *qtd_out) {
    FILE *f = fopen(arquivo, "r");
    if (!f) exit(1);

    int k;
    fscanf(f, "%d", &k);
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF) {}

    CasoLCS *casos = (CasoLCS*)calloc((size_t)k, sizeof(CasoLCS));

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

/* ===== LCS DP ===== */
static int lcs_dp(const char *a, const char *b, int n, int m, size_t *mem_bytes) {
    size_t linhas = (size_t)n + 1;
    size_t cols   = (size_t)m + 1;

    int *dp = (int*)calloc(linhas * cols, sizeof(int));
    if (!dp) exit(1);

    if (mem_bytes) *mem_bytes = linhas * cols * sizeof(int);

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int acima    = dp[(i - 1) * cols + j];
            int esquerda = dp[i * cols + (j - 1)];
            int diag     = dp[(i - 1) * cols + (j - 1)];

            dp[i * cols + j] =
                (a[i - 1] == b[j - 1]) ? (diag + 1)
                                       : ((acima > esquerda) ? acima : esquerda);
        }
    }

    int ans = dp[n * cols + m];
    free(dp);
    return ans;
}

int main(int argc, char **argv) {
    const char *arquivo = (argc >= 2) ? argv[1] : "testes_lcs.txt";

    int qtd = 0;
    CasoLCS *casos = ler_casos(arquivo, &qtd);

    for (int i = 0; i < qtd; i++) 
    {
        size_t mem = 0;

        double t0 = agora_segundos();
        int lcs = lcs_dp(casos[i].a, casos[i].b, casos[i].n, casos[i].m, &mem);
        double t1 = agora_segundos();

        printf("Caso: %s\n", casos[i].nome);
        printf("Tamanho da entrada: n = %d, m = %d\n\n", casos[i].n, casos[i].m);

        printf("Versao: Programacao Dinamica (Iterativo)\n");
        printf("Tempo de execucao (s): %.9f\n", t1 - t0);
        printf("Memoria dinamica (bytes): %zu\n", mem);
        printf("Resultado LCS: %d\n", lcs);
        printf("---------------------------------------\n\n");
    }


    liberar_casos(casos, qtd);
    return 0;
}
