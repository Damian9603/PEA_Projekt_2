#ifndef GRAF_H
#define GRAF_H
#include "pch.h"
#include <vector>

class Graf
{
    public:
        Graf(int liczbaWierzcholkow);
        virtual ~Graf();
        bool dodajWierzcholek(unsigned v, unsigned w, unsigned koszt);
        unsigned zwrocKoszt(unsigned v, unsigned w);
		unsigned zwrocLiczbeWierzcholkow();
        static std::vector<unsigned> tabuSearch(Graf &graf, unsigned kadencja, bool dywersyfikacja, int maxIteracji, float czasStopu, bool vertexEx);
		static std::vector<unsigned>symulowaneWyrzazanie(Graf &graf, float tMin, float tKonc, float czasStopu, unsigned epoka, bool vertexEx);
		void wyswietl();

    protected:
        unsigned liczbaWierzcholkow;

    private:
		unsigned **macierzGrafu;
		unsigned *tablicaGrafu;
};

#endif // GRAF_H
