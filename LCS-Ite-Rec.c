#include <stdio.h>
#include <string.h>
#include <windows.h>

// Função de alta precisão para Windows
static double agora_segundos(void)
{
    static LARGE_INTEGER freq;
    static int init = 0;
    LARGE_INTEGER counter;

    if (!init)
    {
        QueryPerformanceFrequency(&freq);
        init = 1;
    }
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
}

// Função modular para encontrar o maior (sem ternários)
int encontrar_maior(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

// --- VERSÃO RECURSIVA ---
int lcs_recursivo(char *s1, char *s2, int m, int n)
{
    if (m == 0 || n == 0)
    {
        return 0;
    }
    if (s1[m - 1] == s2[n - 1])
    {
        return 1 + lcs_recursivo(s1, s2, m - 1, n - 1);
    }
    else
    {
        return encontrar_maior(lcs_recursivo(s1, s2, m, n - 1),
                               lcs_recursivo(s1, s2, m - 1, n));
    }
}

// --- VERSÃO ITERATIVA (Programação Dinâmica) ---
int lcs_iterativo(char *s1, char *s2, int m, int n)
{
    int matriz[m + 1][n + 1];
    int i, j;

    // Inicialização da borda de zeros (Ø)
    for (i = 0; i <= m; i++)
    {
        for (j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
            {
                matriz[i][j] = 0;
            }
            else if (s1[i - 1] == s2[j - 1])
            {
                // Match: Diagonal + 1
                matriz[i][j] = matriz[i - 1][j - 1] + 1;
            }
            else
            {
                // Diferente: Maior entre Cima ou Esquerda
                matriz[i][j] = encontrar_maior(matriz[i - 1][j], matriz[i][j - 1]);
            }
        }
    }
    return matriz[m][n];
}

void comparar_metodos(char *nome, char *s1, char *s2)
{
    int m = (int)strlen(s1);
    int n = (int)strlen(s2);
    double inicio, fim;
    int res_rec, res_ite;

    printf(">>> TESTE: %s\n", nome);

    // Medindo Recursivo
    inicio = agora_segundos();
    res_rec = lcs_recursivo(s1, s2, m, n);
    fim = agora_segundos();
    printf("RECURSIVO: LCS = %d | Tempo: %.10f s\n", res_rec, fim - inicio);

    // Medindo Iterativo
    inicio = agora_segundos();
    res_ite = lcs_iterativo(s1, s2, m, n);
    fim = agora_segundos();
    printf("ITERATIVO: LCS = %d | Tempo: %.10f s\n", res_ite, fim - inicio);

    printf("--------------------------------------------------\n");
}

int main()
{
    comparar_metodos("IGUAIS (8x8)", "ABCDEFGH", "ABCDEFGH");
    comparar_metodos("SEM COMUM (8x8)", "AAAAAAAA", "BBBBBBBB");
    comparar_metodos("ALTERNADO (8x8)", "ABABABAB", "BABAABAA");

    return 0;
}