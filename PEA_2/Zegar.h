#ifndef ZEGAR_H
#define ZEGAR_H
#include "pch.h"


class Zegar
{
    public:
        Zegar();
        void start();
        void stop();
        double odczyt();
        
    protected:
    private:
        clock_t poczatek;
        clock_t koniec;
		double pomiar;
};

#endif // STOPWATCH_H
