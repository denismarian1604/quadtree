/*VLADULESCU Denis - Marian - 314CBb*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum tip_nod
{
    FRUNZA,
    NOD
};

// tipul de date cu care citim matricea de intrare
typedef struct ccit
{
    unsigned char r, g, b;
} TPixel;

typedef struct c_arb
{
    unsigned char r, g, b;
    unsigned long long mean, latura;
    enum tip_nod tip;
    struct c_arb *sus_stg, *sus_dr, *jos_dr, *jos_stg;
} TCArb, *TArb;

int verif_val(TPixel **grid, int x, int y, unsigned int n, int f, unsigned long long *mean);
TArb rezolva(TPixel **grid, int x, int y, unsigned int n, int f);
TArb constr_arb(TPixel **grid, unsigned int n, int f);
int max4(int a, int b, int c, int d);
int nr_niv(TArb a);
void afis_niv(TArb a, int niv, FILE *w);
void print(TArb a, FILE *w);
void nr_blocuri_sim(TArb a, int f, unsigned long long *nr);
void l_max_bloc(TArb a, unsigned long long *l);
void DistrArb(TArb a);
TArb *c3_cit(TArb *v_grid, FILE *read, unsigned int *k);
void c3_rezolva(TArb a, TArb *v_grid, int k);
TArb c3_constr_arb(TArb *v_grid, int k);
void c3_rezolva_grid(TPixel **grid, TArb a, int x, int y, int n);
TPixel **c3_constr_grid(TArb a, int n);
void c1_c2(const char *argv[], FILE *read, FILE *write);
void afis_grid(TPixel **grid, unsigned int n, FILE *write);
void c3(FILE *read, FILE *write);
