/*VLADULESCU Denis - Marian - 314CBb*/
#include "header.h"
void DistrArb(TArb a)
{
    if (!a)
        return;
    if (a->sus_stg)
        DistrArb(a->sus_stg);
    if (a->sus_dr)
        DistrArb(a->sus_dr);
    if (a->jos_dr)
        DistrArb(a->jos_dr);
    if (a->jos_stg)
        DistrArb(a->jos_stg);
    if (a)
        free(a);
}
int main(int argc, const char *argv[])
{
    FILE *write, *read;

    // in functie de tipul cerintei, determin pozitia fisierelor,
    // iar ulterior le deschid
    if (strcmp(argv[1], "-c1") == 0 || strcmp(argv[1], "-c2") == 0)
    {
        write = fopen(argv[4], "wb");
        if (!write)
            return 1;

        read = fopen(argv[3], "rb");
        if (!read)
            return 1;
    }
    else
    {
        write = fopen(argv[3], "wb");
        if (!write)
            return 1;

        read = fopen(argv[2], "rb");
        if (!read)
            return 1;
    }

    // apelez functiile corespunzatoare tipurilor de cerinta
    // functia c1_c2 primeste ca si argument si argv
    // pentru a putea sa determin factorul de compresie
    if (strcmp(argv[1], "-c1") == 0 || strcmp(argv[1], "-c2") == 0)
    {
        c1_c2(argv, read, write);
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        c3(read, write);
    }

    return 0;
}