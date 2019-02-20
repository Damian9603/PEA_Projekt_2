#include "pch.h"
#include "Zegar.h"

Zegar::Zegar()
{

}

void Zegar::start()
{
    poczatek = clock();
    pomiar = 0;
}

void Zegar::stop()
{
    koniec = clock();
    pomiar = ((double)(koniec - poczatek))/CLOCKS_PER_SEC;
}

double Zegar::odczyt()
{
    return pomiar;
}
