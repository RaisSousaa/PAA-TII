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
    // Ajuste se você tiver strings MUITO grandes
    char buffer[300000];

    if (!fgets(buffer, (int)sizeof(buffer), f)) return NULL;

    // remover \r\n
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[len - 1] = '\0';
        len--;
    }

    char *s = (char*)malloc(len + 1);
    if (!s) {
        fprintf(stderr, "Erro: malloc falhou na leitura da linha.\n");
        exit(1);
    }
    memcpy(s, buffer, len + 1);
    return s;
}

static CasoLCS *ler_casos(const char *arquivo, int *qtd_out) {
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo: %s\n", arquivo);
        exit(1);
    }

    int k = 0;
    if (fscanf(f, "%d", &k) != 1 || k <= 0) {
        fprintf(stderr, "Arquivo invalido: primeira linha deve ser um inteiro K > 0.\n");
        fclose(f);
        exit(1);
    }

    // consumir resto da linha
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF) {}

    CasoLCS *casos = (CasoLCS*)calloc((size_t)k, sizeof(CasoLCS));
    if (!casos) {
        fprintf(stderr, "Erro: calloc falhou.\n");
        fclose(f);
        exit(1);
    }

    for (int i = 0; i < k; i++) {
        if (fscanf(f, "%63s %d %d", casos[i].nome, &casos[i].n, &casos[i].m) != 3) {
            fprintf(stderr, "Erro lendo cabecalho do caso %d.\n", i + 1);
            fclose(f);
            exit(1);
        }
        while ((c = fgetc(f)) != '\n' && c != EOF) {}

        casos[i].a = ler_linha(f);
        casos[i].b = ler_linha(f);

        if (!casos[i].a || !casos[i].b) {
            fprintf(stderr, "Erro lendo strings do caso %d.\n", i + 1);
            fclose(f);
            exit(1);
        }

        // Opcional: validar se bate com n/m
        if ((int)strlen(casos[i].a) != casos[i].n || (int)strlen(casos[i].b) != casos[i].m) {
            fprintf(stderr,
                    "Aviso: tamanhos declarados (n=%d,m=%d) diferem do comprimento real (|a|=%zu,|b|=%zu) no caso '%s'.\n",
                    casos[i].n, casos[i].m, strlen(casos[i].a), strlen(casos[i].b), casos[i].nome);
            // Você pode escolher: ajustar n/m para o tamanho real:
            casos[i].n = (int)strlen(casos[i].a);
            casos[i].m = (int)strlen(casos[i].b);
        }
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

/*
  LCS iterativo (DP):
  dp[i][j] = LCS entre prefixos a[0..i-1] e b[0..j-1]
  Memoria: (n+1)*(m+1) ints
*/
static int lcs_iterativo_dp(const char *a, const char *b, int n, int m, size_t *bytes_dp_out) {
    size_t linhas = (size_t)n + 1;
    size_t colunas = (size_t)m + 1;
    size_t total = linhas * colunas;

    int *dp = (int*)calloc(total, sizeof(int));
    if (!dp) {
        fprintf(stderr, "Erro: falha ao alocar tabela dp (%zu celulas).\n", total);
        exit(1);
    }

    if (bytes_dp_out) *bytes_dp_out = total * sizeof(int);

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int acima    = dp[(size_t)(i - 1) * colunas + (size_t)j];
            int esquerda = dp[(size_t)i * colunas + (size_t)(j - 1)];
            int diagonal = dp[(size_t)(i - 1) * colunas + (size_t)(j - 1)];

            if (a[i - 1] == b[j - 1]) {
                dp[(size_t)i * colunas + (size_t)j] = diagonal + 1;
            } else {
                dp[(size_t)i * colunas + (size_t)j] = (acima > esquerda) ? acima : esquerda;
            }
        }
    }

    int ans = dp[(size_t)n * colunas + (size_t)m];
    free(dp);
    return ans;
}

static double medir_tempo_iterativo(const char *a, const char *b, int n, int m,
                                    int repeticoes, int *lcs_out, size_t *bytes_dp_out) {
    double soma = 0.0;
    int ans = 0;
    size_t bytes_dp = 0;

    for (int r = 0; r < repeticoes; r++) {
        size_t bytes_rodada = 0;
        clock_t inicio = clock();
        ans = lcs_iterativo_dp(a, b, n, m, &bytes_rodada);
        clock_t fim = clock();

        soma += (double)(fim - inicio) / (double)CLOCKS_PER_SEC;
        bytes_dp = bytes_rodada;
    }

    if (lcs_out) *lcs_out = ans;
    if (bytes_dp_out) *bytes_dp_out = bytes_dp;

    return soma / repeticoes;
}

int main(int argc, char **argv) {
    const char *arquivo = (argc >= 2) ? argv[1] : "testes_lcs.txt";
    int repeticoes = (argc >= 3) ? atoi(argv[2]) : 5;
    if (repeticoes <= 0) repeticoes = 5;

    int qtd = 0;
    CasoLCS *casos = ler_casos(arquivo, &qtd);

    // CSV
    printf("caso;n;m;versao;tempo_medio_s;bytes_dp;lcs\n");

    for (int i = 0; i < qtd; i++) {
        int lcs_val = 0;
        size_t bytes_dp = 0;

        double tempo_medio = medir_tempo_iterativo(
            casos[i].a, casos[i].b,
            casos[i].n, casos[i].m,
            repeticoes, &lcs_val, &bytes_dp
        );

        printf("%s;%d;%d;dp_iterativo;%.9f;%zu;%d\n",
               casos[i].nome, casos[i].n, casos[i].m,
               tempo_medio, bytes_dp, lcs_val);
    }

    liberar_casos(casos, qtd);
    return 0;
}
