/*VLADULESCU Denis - Marian - 314CBb*/
#include "header.h"

int verif_val(TPixel **grid, int x, int y, unsigned int n, int f, unsigned long long *mean)
{
    // in acest subprogram calculam procentul de similaritate "mean"
    // precum a fost furnizat in cerinta

    unsigned long long r = 0, g = 0, b = 0;
    for (int i = x; i < x + n; i++)
        for (int j = y; j < y + n; j++)
        {
            r += grid[i][j].r;
            g += grid[i][j].g;
            b += grid[i][j].b;
        }
    r /= n * n;
    g /= n * n;
    b /= n * n;

    (*mean) = 0;
    for (unsigned long long i = x; i < x + n; i++)
        for (unsigned long long j = y; j < y + n; j++)
        {
            (*mean) += (r - grid[i][j].r) * (r - grid[i][j].r);
            (*mean) += (g - grid[i][j].g) * (g - grid[i][j].g);
            (*mean) += (b - grid[i][j].b) * (b - grid[i][j].b);
        }
    (*mean) /= (3 * n * n);

    // daca procentul este mai mic decat factorul furnizat
    // inseamna ca blocul nu trebuie sa mai fie divizat
    // in caz contrar, continuam sa-l impartim
    if ((*mean) <= f)
        return 1;
    else
        return 0;
}
TArb rezolva(TPixel **grid, int x, int y, unsigned int n, int f)
{
    unsigned long long mean = 0;

    // verificam scorul similaritatii
    if (verif_val(grid, x, y, n, f, &mean))
    {
        // in acest caz mean <= factor pentru blocul curent
        // deci divizunea blocului s-a finalizat

        // alocam un nou nod
        TArb nod = (TArb)calloc(1, sizeof(TCArb));
        if (!nod)
            return NULL;

        // calculam culorile nodului
        unsigned long long r = 0, g = 0, b = 0;
        for (int i = x; i < x + n; i++)
            for (int j = y; j < y + n; j++)
            {
                r += grid[i][j].r;
                g += grid[i][j].g;
                b += grid[i][j].b;
            }
        r /= n * n;
        g /= n * n;
        b /= n * n;
        // completam campurile nodului

        // scorul de similaritate
        nod->mean = mean;
        // latura blocului
        nod->latura = n;
        // punem valoarea medie a pixelilor
        nod->r = r;
        nod->g = g;
        nod->b = b;
        // tipul nodului este, asadar, FRUNZA, deci nu va mai avea copii.
        nod->tip = FRUNZA;
        nod->sus_stg = nod->sus_dr = NULL;
        nod->jos_dr = nod->jos_stg = NULL;

        return nod;
    }
    else
    {
        // in acest caz mean > factor pentru blocul curent
        // deci divizunea blocului nu s-a finalizat

        // alocam un nou nod
        TArb nod = (TArb)calloc(1, sizeof(TCArb));
        if (!nod)
            return NULL;

        // scorul de similaritate
        nod->mean = mean;
        // latura blocului
        nod->tip = NOD;
        // conventie : daca nodul nu este frunza
        // atunci are -1 din toate culorile pentru a putea fi usor identificat
        nod->r = nod->g = nod->b = -1;
        nod->latura = 0;
        // tipul nodului este, asadar, NOD, deci facem autoapelurile
        // pentru copii.
        nod->sus_stg = rezolva(grid, x, y, n / 2, f);
        nod->sus_dr = rezolva(grid, x, y + n / 2, n / 2, f);
        nod->jos_dr = rezolva(grid, x + n / 2, y + n / 2, n / 2, f);
        nod->jos_stg = rezolva(grid, x + n / 2, y, n / 2, f);

        return nod;
    }
}
TArb constr_arb(TPixel **grid, unsigned int n, int f)
{
    // n = una din dimensiunile matricei
    int x = 0, y = 0;
    // returnam arborele generat prin apelurile recursive
    // ale functiei rezolva.
    return rezolva(grid, x, y, n, f);
}
int max4(int a, int b, int c, int d)
{
    // functie care calculeaza maximul intre
    // 4 valorile date
    int maxim = a;
    if (maxim < b)
        maxim = b;
    if (maxim < c)
        maxim = c;
    if (maxim < d)
        maxim = d;

    return maxim;
}
int nr_niv(TArb a)
{
    // functie care calculeaza numar de niveluri ale arborelui
    if (!a)
        return 0;
    int ss = 1 + nr_niv(a->sus_stg);
    int sd = 1 + nr_niv(a->sus_dr);
    int jd = 1 + nr_niv(a->jos_stg);
    int js = 1 + nr_niv(a->jos_dr);

    return max4(ss, sd, jd, js);
}
void afis_niv(TArb a, int niv, FILE *w)
{
    // daca am ajuns pe nivelul curent si nodul exista
    if (a && niv == 0)
    {
        // facem afisarile conform cerintei
        if (a->tip == FRUNZA)
        {
            unsigned char tip_nod = 1;
            fwrite(&tip_nod, sizeof(unsigned char), 1, w);
            fwrite(&a->r, sizeof(unsigned char), 1, w);
            fwrite(&a->g, sizeof(unsigned char), 1, w);
            fwrite(&a->b, sizeof(unsigned char), 1, w);
        }
        else
        {
            unsigned char tip_nod = 0;
            fwrite(&tip_nod, sizeof(unsigned char), 1, w);
        }
    }
    else if (a)
    {
        // cazul in care nodul exista, dar nu este pe nivelul dorit
        // in acest caz se continua coborarea in nivel
        afis_niv(a->sus_stg, niv - 1, w);
        afis_niv(a->sus_dr, niv - 1, w);
        afis_niv(a->jos_dr, niv - 1, w);
        afis_niv(a->jos_stg, niv - 1, w);
    }
}
void print(TArb a, FILE *w)
{
    // determinam numarul de niveluri
    int nr_nivele = nr_niv(a);
    // afisam nodurile de pe fiecare nivel
    for (int i = 0; i < nr_nivele; i++)
    {
        afis_niv(a, i, w);
    }
}
void nr_blocuri_sim(TArb a, int f, unsigned long long *nr)
{
    // functie care calculeaza numarul de blocuri cu factorul mean
    // <= factorul furnizat
    if (!a)
        return;
    else
    {
        if (a->mean <= f)
            (*nr)++;

        nr_blocuri_sim(a->sus_stg, f, nr);
        nr_blocuri_sim(a->sus_dr, f, nr);
        nr_blocuri_sim(a->jos_dr, f, nr);
        nr_blocuri_sim(a->jos_stg, f, nr);
    }
}
void l_max_bloc(TArb a, unsigned long long *l)
{
    // functie care calculeaza latura maxima a unui bloc
    if (!a)
        return;
    else
    {
        if (a->latura > (*l) && a->latura != 0)
            (*l) = a->latura;

        l_max_bloc(a->sus_stg, l);
        l_max_bloc(a->sus_dr, l);
        l_max_bloc(a->jos_dr, l);
        l_max_bloc(a->jos_stg, l);
    }
}
void c1_c2(const char *argv[], FILE *read, FILE *write)
{
    int factor = 0;

    // determinam factorul impus
    for (int i = 0; i < strlen(argv[2]); i++)
        factor = factor * 10 + (argv[2][i] - '0');

    char *tip_fisier = (char *)calloc(3, sizeof(char));
    if (!tip_fisier)
        goto elib;

    // facem citirile necesare
    fscanf(read, "%s", tip_fisier);

    unsigned int nr_c = 0, nr_l = 0, vmax = 0;
    fscanf(read, "%u %u", &nr_l, &nr_c);
    fscanf(read, "%u", &vmax);

    // alocam matricea de pixeli a imaginii
    TPixel **grid = (TPixel **)calloc(nr_l, sizeof(TPixel *));
    // in cazul in care alocarea nu a reusit, mergem la label-ul elib
    // dupa care se vor face si restul de eliberari reusite pana in acel punct
    if (!grid)
        goto elib;

    for (int i = 0; i < nr_l; i++)
    {
        grid[i] = (TPixel *)calloc(nr_c, sizeof(TPixel));
        // daca alocarea unui element nu reuseste, le eliberam pe cele alocate,
        // iar apoi mergem la final pentru restul eliberarilor
        if (!grid[i])
        {
            for (int j = 0; j < i; j++)
                free(grid[j]);
            free(grid);
            goto elib;
        }
    }

    // citim sfarsitul de linie(nefolositor)
    unsigned char junk = '\0';
    fread(&junk, sizeof(unsigned char), 1, read);

    // citim matricea imaginii
    for (int i = 0; i < nr_l; i++)
    {
        for (int j = 0; j < nr_c; j++)
        {
            fread(&grid[i][j].r, sizeof(unsigned char), 1, read);
            fread(&grid[i][j].g, sizeof(unsigned char), 1, read);
            fread(&grid[i][j].b, sizeof(unsigned char), 1, read);
        }
    }

    // constuim arborele cuaternar pe baza matricei imagine
    TArb a = constr_arb(grid, nr_l, factor);
    // in cazul in care alocarea nu a reusit, mergem la label-ul elib_grid
    // dupa care se vor face si restul de eliberari reusite pana in acel punct
    if (!a)
        goto elib_grid;

    // in functie de cerinta, apelam functiile specifice
    if (strcmp(argv[1], "-c2") == 0)
    {
        // scriem dimensiunea laturii imaginii
        fwrite(&nr_l, sizeof(unsigned int), 1, write);
        // facem afisarea pe nivel a arborelui
        print(a, write);
    }
    else
    {
        unsigned long long nr = 0, nr_b = 0, l_max = 0;

        unsigned int nrfr = 0;

        // determinam numarul de niveluri ale arborelui
        nr = nr_niv(a);

        // afisam numarul de niveluri
        fprintf(write, "%llu\n", nr);

        // determinam numarul de blocuri cu mean < factorul furnizat
        nr_blocuri_sim(a, factor, &nr_b);

        // determinam blocul cu latura maxima
        l_max_bloc(a, &l_max);

        // afisam informatiile procurate
        fprintf(write, "%llu\n%llu\n", nr_b, l_max);
    }

// realizam eliberarile de memorie
elib_grid:
    for (int i = 0; i < nr_l; i++)
        free(grid[i]);
    free(grid);

elib:
    DistrArb(a);
    free(tip_fisier);
    fclose(read);
    fclose(write);
}