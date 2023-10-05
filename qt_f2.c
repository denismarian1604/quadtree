/*VLADULESCU Denis - Marian - 314CBb*/
#include "header.h"

TArb *c3_cit(TArb *v_grid, FILE *read, unsigned int *k)
{
    unsigned char tip = '0';
    while (!feof(read))
    {
        // citim tipul nodului
        fread(&tip, sizeof(unsigned char), 1, read);

        // daca am citit NULL, inseamna ca fisierul s-a terminat
        if (feof(read))
            break;

        // daca tipul este 0, adica este NOD
        if (tip == 0)
        {
            // nodurile care nu sunt frunze le identificam
            // cu o conventie similara ca la construirea arborelui
            // pentru primele 2 cerinte

            // alocam nodul curent
            v_grid[*k] = (TArb)calloc(1, sizeof(TCArb));
            if (!v_grid[*k])
            {
                for (int j = 0; j < *k; j++)
                    free(v_grid[j]);
                return NULL;
            }

            // punem informatiile
            v_grid[*k]->r = v_grid[*k]->g = v_grid[*k]->b = -1;
            v_grid[*k]->tip = NOD;

            // contorizam elementele
            (*k)++;
        }
        else
        {
            // daca tipul este 1, adica este FRUNZA
            unsigned int r, g, b;
            r = g = b = 0;
            fread(&r, sizeof(unsigned char), 1, read);
            fread(&g, sizeof(unsigned char), 1, read);
            fread(&b, sizeof(unsigned char), 1, read);

            // alocam nodul curent
            v_grid[*k] = (TArb)calloc(1, sizeof(TCArb));
            if (!v_grid[*k])
            {
                for (int j = 0; j < *k; j++)
                    free(v_grid[j]);
                return NULL;
            }

            // punem informatiile
            v_grid[*k]->r = r;
            v_grid[*k]->g = g;
            v_grid[*k]->b = b;
            v_grid[*k]->tip = FRUNZA;

            // contorizam elementele
            (*k)++;
        }
    }
    return v_grid;
}
void c3_rezolva(TArb a, TArb *v_grid, int k)
{
    // ant este folosit pentru eficientizarea parcurgerii
    // deoarece ultima pozitie valida este garantat
    // incepand de la ultima folosita + 1

    int ant = 0;
    for (int i = 0; i < k; i++)
    {
        // in cazul in care nodul curent este de tip NOD, adica
        // se divide, ii cautam fiii mai departe in vector
        // in cazul in care este frunza nu mai sunt legaturi de facut
        if (v_grid[i]->tip == NOD)
        {
            int j = ant + 1;
            // determinam prima pozitia disponibila pentru fiecare fiu
            // al nodului curent

            v_grid[i]->sus_stg = v_grid[j];
            ant = j;
            j++;

            v_grid[i]->sus_dr = v_grid[j];
            ant = j;
            j++;

            v_grid[i]->jos_dr = v_grid[j];
            ant = j;
            j++;

            v_grid[i]->jos_stg = v_grid[j];
            ant = j;
            j++;
        }
    }
}
TArb c3_constr_arb(TArb *v_grid, int k)
{
    // initializam arborele cu primul element din vectorul de noduri
    // deci cu radacina.
    TArb a = v_grid[0];

    // in cazul in care avem mai mult de un nod, continuam sa facem legaturi
    if (k > 1)
        c3_rezolva(a, v_grid, k);

    // returnam arborele creat
    return a;
}
void c3_rezolva_grid(TPixel **grid, TArb a, int x, int y, int n)
{
    // daca nodul curent este NOD, inseamna ca se divide
    // si atunci divizam blocul curent in 4 si repetam procesul
    // pana cand dam de frunze pe fiecare apel
    if (a->tip == NOD)
    {
        c3_rezolva_grid(grid, a->sus_stg, x, y, n / 2);
        c3_rezolva_grid(grid, a->sus_dr, x, y + n / 2, n / 2);
        c3_rezolva_grid(grid, a->jos_dr, x + n / 2, y + n / 2, n / 2);
        c3_rezolva_grid(grid, a->jos_stg, x + n / 2, y, n / 2);
    }
    else
    {
        // e frunza, iar in acest caz copiem culorile in matrice
        for (int i = x; i < x + n; i++)
            for (int j = y; j < y + n; j++)
            {
                grid[i][j].r = a->r;
                grid[i][j].g = a->g;
                grid[i][j].b = a->b;
            }
    }
}
TPixel **c3_constr_grid(TArb a, int n)
{
    int x = 0, y = 0;
    // alocam matricei de pixeli
    TPixel **grid = (TPixel **)calloc(n, sizeof(TPixel *));
    if (!grid)
        return NULL;

    for (int i = 0; i < n; i++)
    {
        grid[i] = (TPixel *)calloc(n, sizeof(TPixel));
        if (!grid[i])
        {
            for (int j = 0; j < i; j++)
                free(grid[j]);
            free(grid);
            return NULL;
        }
    }
    // construim matricea de pixeli
    c3_rezolva_grid(grid, a, x, y, n);
    return grid;
}
void afis_grid(TPixel **grid, unsigned int n, FILE *write)
{
    // subprogram de afisare al matricii de pixeli grid
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            unsigned int r = grid[i][j].r;
            unsigned int g = grid[i][j].g;
            unsigned int b = grid[i][j].b;

            fwrite(&r, sizeof(unsigned char), 1, write);
            fwrite(&g, sizeof(unsigned char), 1, write);
            fwrite(&b, sizeof(unsigned char), 1, write);
        }
    }
}
void c3(FILE *read, FILE *write)
{
    unsigned int n = 0;
    fread(&n, sizeof(unsigned int), 1, read);
    // numaram nodurile din fisier
    int nr_elem = 0;
    while (!feof(read))
    {
        unsigned char tip = '\0';
        fread(&tip, sizeof(unsigned char), 1, read);

        // daca am citit NULL, inseamna ca fisierul s-a terminat
        if (feof(read))
            break;

        nr_elem++;
    }

    // repozitionam cursorul la inceput
    fseek(read, SEEK_SET, 0);

    // vector in care retinem elementele arborelui de construit
    TArb *v_grid_aux = (TArb *)calloc(nr_elem, sizeof(TArb));

    // in cazul in care alocarea nu a reusit sarim direct la eliberari
    if (!v_grid_aux)
        goto elib2;

    unsigned int k = 0;
    // citim latura din fisier
    fread(&n, sizeof(unsigned int), 1, read);

    // citim in vectorul de blocuri, elementele arborelui
    // ok = 1 => eroare
    TArb *v_grid = c3_cit(v_grid_aux, read, &k);

    // in cazul in care alocarea nu a reusit sarim direct la eliberari
    if (!v_grid)
        goto elib2;

    TArb a;
    // construim arborele pe baza vectorului de blocuri
    a = c3_constr_arb(v_grid, k);

    // construim matricea de reprezentare a imaginii pe baza arborelui
    TPixel **grid = c3_constr_grid(a, n);

    // facem afisarea conformul fisierului .ppm
    fprintf(write, "P6\n%u %u\n255\n", n, n);
    afis_grid(grid, n, write);

// eliberarile de memorie
elib2:

    for (int i = 0; i < n; i++)
        free(grid[i]);
    free(grid);
    if (v_grid)
        free(v_grid);
    if (a)
        DistrArb(a);
    fclose(read);
    fclose(write);
}
