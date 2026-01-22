#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nome[64];
    int n;
    int m;
    char *a;
    char *b;
} CasoLCS;

static char *ler_linha(FILE *f) {
    char buffer[200000];
    if (!fgets(buffer, (int)sizeof(buffer), f)) return NULL;

    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
        buffer[len-1] = '\0';
        len--;
    }

    char *s = (char*)malloc(len + 1);
    if (!s) exit(1);
    memcpy(s, buffer, len + 1);
    return s;
}

static CasoLCS *ler_casos(const char *arquivo, int *qtd) {
    FILE *f = fopen(arquivo, "r");
    if (!f) { fprintf(stderr, "Erro ao abrir %s\n", arquivo); exit(1); }

    int k = 0;
    if (fscanf(f, "%d", &k) != 1 || k <= 0) {
        fprintf(stderr, "Arquivo invalido (primeira linha deve ser K > 0)\n");
        exit(1);
    }

    // consumir resto da linha
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF) {}

    CasoLCS *casos = (CasoLCS*)calloc((size_t)k, sizeof(CasoLCS));
    if (!casos) exit(1);

    for (int i = 0; i < k; i++) {
        if (fscanf(f, "%63s %d %d", casos[i].nome, &casos[i].n, &casos[i].m) != 3) {
            fprintf(stderr, "Erro lendo cabecalho do caso %d\n", i+1);
            exit(1);
        }
        while ((c = fgetc(f)) != '\n' && c != EOF) {}

        casos[i].a = ler_linha(f);
        casos[i].b = ler_linha(f);

        if (!casos[i].a || !casos[i].b) {
            fprintf(stderr, "Erro lendo strings do caso %d\n", i+1);
            exit(1);
        }
    }

    fclose(f);
    *qtd = k;
    return casos;
}

static void liberar_casos(CasoLCS *casos, int qtd) {
    for (int i = 0; i < qtd; i++) {
        free(casos[i].a);
        free(casos[i].b);
    }
    free(casos);
}

/* ===== LCS recursivo naive ===== */
static int lcs_rec(const char *a, const char *b, int i, int j) {
    if (i == 0 || j == 0) return 0;
    if (a[i-1] == b[j-1]) return 1 + lcs_rec(a, b, i-1, j-1);
    int x = lcs_rec(a, b, i-1, j);
    int y = lcs_rec(a, b, i, j-1);
    return (x > y) ? x : y;
}

static double medir_tempo_rec(const char *a, const char *b, int n, int m, int repeticoes, int *lcs_out) {
    double soma = 0.0;
    int ans = 0;

    for (int r = 0; r < repeticoes; r++) {
        clock_t inicio = clock();
        ans = lcs_rec(a, b, n, m);
        clock_t fim = clock();
        soma += (double)(fim - inicio) / (double)CLOCKS_PER_SEC;
    }

    if (lcs_out) *lcs_out = ans;
    return soma / repeticoes;
}

int main(int argc, char **argv) {
    const char *arquivo = (argc >= 2) ? argv[1] : "testes_lcs.txt";
    int repeticoes = (argc >= 3) ? atoi(argv[2]) : 5;
    if (repeticoes <= 0) repeticoes = 5;

    int qtd = 0;
    CasoLCS *casos = ler_casos(arquivo, &qtd);

    // CSV
    printf("caso;n;m;versao;tempo_medio_s;lcs\n");

    for (int i = 0; i < qtd; i++) {
        int lcs_val = 0;
        double tempo = medir_tempo_rec(casos[i].a, casos[i].b, casos[i].n, casos[i].m, repeticoes, &lcs_val);
        printf("%s;%d;%d;recursivo;%.9f;%d\n", casos[i].nome, casos[i].n, casos[i].m, tempo, lcs_val);
    }

    liberar_casos(casos, qtd);
    return 0;
}
