#include <stdio.h>
#include <string.h>
#include <windows.h>

char *base_pilha;
long pico_pilha;

static double agora_ms(void)
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
    return ((double)counter.QuadPart / (double)freq.QuadPart) * 1000.0;
}

int maior(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

int lcs_rec(char *s1, char *s2, int m, int n)
{
    char local;
    long uso = (long)(base_pilha - &local);
    if (uso > pico_pilha)
        pico_pilha = uso;

    if (m == 0 || n == 0)
        return 0;
    if (s1[m - 1] == s2[n - 1])
        return 1 + lcs_rec(s1, s2, m - 1, n - 1);
    else
        return maior(lcs_rec(s1, s2, m, n - 1), lcs_rec(s1, s2, m - 1, n));
}

int lcs_ite(char *s1, char *s2, int m, int n, unsigned long *mem)
{
    int mt[m + 1][n + 1];
    *mem = (unsigned long)(sizeof(int) * (m + 1) * (n + 1));
    for (int i = 0; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
                mt[i][j] = 0;
            else if (s1[i - 1] == s2[j - 1])
                mt[i][j] = mt[i - 1][j - 1] + 1;
            else
                mt[i][j] = maior(mt[i - 1][j], mt[i][j - 1]);
        }
    }
    return mt[m][n];
}

void rodar(char *label, char *s1, char *s2)
{
    int m = strlen(s1), n = strlen(s2);
    unsigned long m_it;
    char b;
    base_pilha = &b;
    pico_pilha = 0;

    double t1 = agora_ms();
    int r1 = lcs_rec(s1, s2, m, n);
    double t2 = agora_ms();

    double t3 = agora_ms();
    int r2 = lcs_ite(s1, s2, m, n, &m_it);
    double t4 = agora_ms();

    printf("[%s]\nRec: %d | %.4f ms | Pilha: %ld B\n", label, r1, t2 - t1, pico_pilha);
    printf("Ite: %d | %.4f ms | Matriz: %lu B\n---\n", r2, t4 - t3, m_it);
}

int main()
{
    rodar("IGUAIS", "ABCDEFGH", "ABCDEFGH");
    rodar("SEM COMUM ", "AAAAAAAA", "BBBBBBBB");
    rodar("ALTERNADO", "ABABABAB", "BABAABAA");
    return 0;
}