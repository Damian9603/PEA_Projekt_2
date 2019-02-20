#include "pch.h"
#include "Graf.h"
#include "Zegar.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <queue>

using namespace std;

Graf::Graf(int liczbaWierzcholkow)
{
	this->liczbaWierzcholkow = liczbaWierzcholkow;
	macierzGrafu = new unsigned*[liczbaWierzcholkow];
	tablicaGrafu = new unsigned[liczbaWierzcholkow*liczbaWierzcholkow];

	for (int i = 0; i < liczbaWierzcholkow; i++)
	{
		macierzGrafu[i] = tablicaGrafu + i * liczbaWierzcholkow;

		for (int j = 0; j < liczbaWierzcholkow; j++)
			macierzGrafu[i][j] = 0;
	}
}

Graf::~Graf()
{
	delete tablicaGrafu;
	delete macierzGrafu;
}


bool Graf::dodajWierzcholek(unsigned v, unsigned w, unsigned koszt)
{
	if (macierzGrafu[v][w] > 0)
	{
		return false;
	}
	else
	{
		macierzGrafu[v][w] = koszt;
		return true;
	}
}

unsigned Graf::zwrocKoszt(unsigned v, unsigned w)
{
	return macierzGrafu[v][w];
}

unsigned Graf::zwrocLiczbeWierzcholkow()
{
	return liczbaWierzcholkow;
}


vector<unsigned> Graf::tabuSearch(Graf &graf, unsigned kadencja, bool dywersyfikacja, int maxIteracji, float czasStopu, bool vertexEx)
{
    Zegar zegar;

    vector<unsigned> optymalnaDroga;
    int dlugoscOptymalnejDrogi = -1;

    vector<unsigned> aktualnaDroga;
    aktualnaDroga.push_back(0);

    #pragma region zachlanny
	//pierwsze rozwiązanie
    for(int i = 0; i < graf.liczbaWierzcholkow - 1; i++) 
	{
        int najmniejszaOdlegosc = -1;
        unsigned nastepnyWierzcholek;
        for(int j = 0; j < graf.liczbaWierzcholkow; j++) 
		{
			if (i == j)
			{
				continue;
			}


            bool uzytyWierzcholek = false;
            for(int k = 0; k < aktualnaDroga.size(); k++) 
			{
                if(j == aktualnaDroga.at(k)) 
				{
                    uzytyWierzcholek = true;
                    break;
                }
            }
			if (uzytyWierzcholek)
			{
				continue;
			}


            unsigned rozwazanaDlugosc = graf.zwrocKoszt(i, j);

            if(najmniejszaOdlegosc == -1|| najmniejszaOdlegosc > rozwazanaDlugosc) 
			{
                najmniejszaOdlegosc = rozwazanaDlugosc;
                nastepnyWierzcholek = j;
            }
        }
        aktualnaDroga.push_back(nastepnyWierzcholek);
    }
    aktualnaDroga.push_back(0);

    #pragma endregion

    vector< vector<unsigned> > tablicaTabu;
    int licznikDywersyfikacji = 0;
    bool czyPozostalCzas = true;
    zegar.start();
	
    while(czyPozostalCzas == true) 
	{
        bool warunekStopu = true;

        while(warunekStopu == true) 
		{
            vector<unsigned> nastepnaDroga;
            int dlugoscNastepnejDrogi = -1;

            vector<unsigned> nastepneTabu(3, 0);
			//ustawienie kadencji tabu na podaną w parametrze kadencja
            nastepneTabu.at(0) = kadencja;

            int start = 1;
			if (vertexEx)
			{
				#pragma region wymiana wierzcholkow (VertexEx)
				for (int i = 1; i < graf.liczbaWierzcholkow - 1; i++)
				{
					for (int j = start; j < graf.liczbaWierzcholkow - 1; j++)
					{
						//zamienianie wierzchołka z samym sobą jest bezcelowe
						if (i == j)
						{
							continue;
						}

						//sąsiednia droga (zamienione dwa wierzchołki w porównaniu do aktualnej drogi)
						vector<unsigned> sasiedniaDroga = aktualnaDroga;

						//zamiana dwóch wierzchołków w drodze (o indeksie i oraz j), czyli tworzenie sąsiedniego rozwiązania
						unsigned tmp = sasiedniaDroga.at(j);
						sasiedniaDroga.at(j) = sasiedniaDroga.at(i);
						sasiedniaDroga.at(i) = tmp;

						unsigned dlugoscSasiedniejDrogi = 0;
						//obliczanie sąsiedniego rozwiązania
						for (int i = 1; i < sasiedniaDroga.size(); i++)
						{
							dlugoscSasiedniejDrogi += graf.zwrocKoszt(sasiedniaDroga.at(i - 1), sasiedniaDroga.at(i));
						}

						//sprawdzanie czy na liście tabu nie ma danego połączenia
						bool tabu = false;
						for (int k = 0; k < tablicaTabu.size(); k++)
						{
							if (tablicaTabu.at(k).at(1) == i && tablicaTabu.at(k).at(2) == j)
							{
								tabu = true;
							}

							if (tablicaTabu.at(k).at(1) == j && tablicaTabu.at(k).at(2) == i)
							{
								tabu = true;
							}

						}

						//kryterium aspiracji
						if (tabu == true && dlugoscSasiedniejDrogi >= dlugoscOptymalnejDrogi)
						{
							continue;
						}
						//jeżeli droga sąsiedniego rozwiązania jest mniejsza od najlepszego rozwiązania w pętli to ona staje się najlepszym rozwiązaniem w pętli
						if (dlugoscNastepnejDrogi == -1 || dlugoscNastepnejDrogi > dlugoscSasiedniejDrogi)
						{
							dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
							nastepnaDroga = sasiedniaDroga;
							nastepneTabu.at(1) = i;
							nastepneTabu.at(2) = j;
						}
					}
					start++;
				}
				#pragma endregion
			}
			else
			{
				#pragma region 2-opt
				for (int i = 1; i < graf.liczbaWierzcholkow - 2; i++)
				{
					for (int j = i; j < graf.liczbaWierzcholkow; j++)
					{

						//sąsiednia droga (zamienione dwa wierzchołki w porównaniu do aktualnej drogi)
						vector<unsigned> sasiedniaDroga=aktualnaDroga;
						//sasiedniaDroga.resize(aktualnaDroga.size());
						if (i != j)
						{
							int dec = 0;
							if (i + 1 == j)
							{
								unsigned pom = sasiedniaDroga[i];
								sasiedniaDroga[i] = sasiedniaDroga[j];
								sasiedniaDroga[j] = pom;
							}
							else if (j == graf.liczbaWierzcholkow - 1)
							{
								dec = 0;
								for (int k = i; k < (j / 2) + 1; k++)
								{
									unsigned pom = sasiedniaDroga[k];
									sasiedniaDroga[k] = sasiedniaDroga[j - dec];
									sasiedniaDroga[j - dec] = pom;
									dec++;
								}
								dec = 0;
							}
							else
							{
								dec = 0;
								for (int k = i; k < (j / 2) + 1; k++)
								{
									unsigned pom = sasiedniaDroga[k];
									sasiedniaDroga[k] = sasiedniaDroga[j - dec];
									sasiedniaDroga[j - dec] = pom;
									dec++;
								}
							}
						}
						
						

						unsigned dlugoscSasiedniejDrogi = 0;
						//obliczanie sąsiedniego rozwiązania
						for (int i = 1; i < sasiedniaDroga.size(); i++)
						{
							dlugoscSasiedniejDrogi += graf.zwrocKoszt(sasiedniaDroga.at(i - 1), sasiedniaDroga.at(i));
						}

						//sprawdzanie czy na liście tabu nie ma danego połączenia
						bool tabu = false;
						for (int k = 0; k < tablicaTabu.size(); k++)
						{
							if (tablicaTabu.at(k).at(1) == i && tablicaTabu.at(k).at(2) == j)
							{
								tabu = true;
							}

							if (tablicaTabu.at(k).at(1) == j && tablicaTabu.at(k).at(2) == i)
							{
								tabu = true;
							}

						}

						//kryterium aspiracji (żadne połączenie nie może być na liście tabu oraz droga sąsiedniego rozwiązania musi być mniejsza od aktualnego optymalnego rozwiązania)
						if (tabu == true && dlugoscSasiedniejDrogi >= dlugoscOptymalnejDrogi)
						{
							continue;
						}
						//jeżeli droga sąsiedniego rozwiązania jest mniejsza od najlepszego rozwiązania w pętli to ona staje się najlepszym rozwiązaniem w pętli
						if (dlugoscNastepnejDrogi == -1 || dlugoscNastepnejDrogi > dlugoscSasiedniejDrogi)
						{
							dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
							nastepnaDroga = sasiedniaDroga;
							nastepneTabu.at(1) = i;
							nastepneTabu.at(2) = j;
						}
					}
				}
				#pragma endregion
			}
			
			//aktualna droga staje się najlepszą drogą z wyłonioną z pętli
            aktualnaDroga = nastepnaDroga;
			//jeżeli aktualna optymalna droga nie istnieje, albo jest większa od najlepszej z pętli to najlepsza z pętli staje się aktualną optymalną drogą
            if(dlugoscOptymalnejDrogi == -1 || dlugoscOptymalnejDrogi > dlugoscNastepnejDrogi)
			{
                dlugoscOptymalnejDrogi = dlugoscNastepnejDrogi;
                optymalnaDroga = nastepnaDroga;
                licznikDywersyfikacji = 0;
            }
			//dekrementacja kadencji połączeń w tablicy tabu
            for(int i = 0; i < tablicaTabu.size(); i++) 
			{
                tablicaTabu.at(i).at(0)--;
            }
			//usuwanie połączeń o zerowej kadencji w tablicy tabu
            for(int i = 0; i < tablicaTabu.size(); i++) 
			{
                if(tablicaTabu.at(i).at(0) == 0)
                    tablicaTabu.erase(tablicaTabu.begin() + i);
            }
			//dodawanie do tablicy tabu nowego połączenia wyłonionego w pętli
            tablicaTabu.push_back(nastepneTabu);
            licznikDywersyfikacji++;

            zegar.stop();
			//sprawdzanie czy zadany czas upłynął 
			if (zegar.odczyt() > czasStopu)
			{
				czyPozostalCzas = false;
			}

			//jeżeli dywersyfikacja jest włączona to warunek stopu jest osiągany przez upłynięcie czasu albo osiągnięcie odpowiedniej ilości iteracji
            if(dywersyfikacja == true) 
			{
				if (licznikDywersyfikacji >= maxIteracji || czyPozostalCzas == false)
				{
					warunekStopu = false;
				}
                    
            }
			//jeżeli dywersyfikacja jest wyłączona to w przypadku upłynięcia czasu warunek stopu jest osiągnięty
            else 
			{
				if (czyPozostalCzas == false)
				{
					warunekStopu = false;
				}
                    
            }
        }

		//jezeli dywersyfikacja jest włączona to następuje ponowne zdefiniowanie pierwszego rozwiązania i poprzednie kroki są powtarzane (powyższa pętla while) 
        if(dywersyfikacja == true) 
		{
            srand(time(NULL));
			//czyszczenie aktualnej drogi
            aktualnaDroga.clear();

            #pragma region losowanie wierzcholkow
			//losowanie 3 losowych wierzchołków w celu uwtorzenia pierwszej ścieżki

            unsigned pierwszy = rand() % graf.liczbaWierzcholkow;
            aktualnaDroga.push_back(pierwszy);

            unsigned drugi;
			do
			{
				drugi = rand() % graf.liczbaWierzcholkow;
			}
            while(drugi == pierwszy);

            aktualnaDroga.push_back(drugi);

            unsigned trzeci;
			do 
			{
				trzeci = rand() % graf.liczbaWierzcholkow;
			}
            while(trzeci == pierwszy || trzeci == drugi);
            aktualnaDroga.push_back(trzeci);
            #pragma endregion

            #pragma region zachlanny
			//reszta drogi jest dopełniana za pomocą algorytmu zachłannego
            for(int i = 0; i < graf.liczbaWierzcholkow - 3; i++) 
			{
                int najmniejszaOdlegosc = -1;
                unsigned nastepnyWierzcholek;
                for(int j = 0; j < graf.liczbaWierzcholkow; j++) 
				{
					if (i == j)
					{
						continue;
					}

                    bool uzytyWierzcholek = false;
                    for(int k = 0; k < aktualnaDroga.size(); k++) 
					{
                        if(j == aktualnaDroga.at(k)) 
						{
                            uzytyWierzcholek = true;
                            break;
                        }
                    }

					if (uzytyWierzcholek)
					{
						continue;
					}

                    unsigned rozwazanaDlugosc = graf.zwrocKoszt(i, j);
					if (najmniejszaOdlegosc == -1 || najmniejszaOdlegosc > rozwazanaDlugosc)
					{
						najmniejszaOdlegosc = rozwazanaDlugosc;
						nastepnyWierzcholek = j;
					}
                }
                aktualnaDroga.push_back(nastepnyWierzcholek);
            }
            aktualnaDroga.push_back(pierwszy);
            #pragma endregion
        }

        licznikDywersyfikacji = 0;
    }

    return optymalnaDroga;
}

vector<unsigned> Graf::symulowaneWyrzazanie(Graf &graf, float tPocz, float tMin, float czasStopu, unsigned epoka, bool vertexEx)
{
	float temperatura = tPocz;
	Zegar zegar;

	vector<unsigned> optymalnaDroga;
	int dlugoscOptymalnejDrogi = -1;

	vector<unsigned> aktualnaDroga;
	int dlugoscAktualnejDrogi = -1;
	aktualnaDroga.push_back(0);
	
	#pragma region zachlanny
	//pierwsze rozwiązanie
	for (int i = 0; i < graf.liczbaWierzcholkow - 1; i++)
	{
		int najmniejszaOdlegosc = -1;
		unsigned nastepnyWierzcholek;
		for (int j = 0; j < graf.liczbaWierzcholkow; j++)
		{
			if (i == j)
			{
				continue;
			}


			bool uzytyWierzcholek = false;
			for (int k = 0; k < aktualnaDroga.size(); k++)
			{
				if (j == aktualnaDroga.at(k))
				{
					uzytyWierzcholek = true;
					break;
				}
			}
			if (uzytyWierzcholek)
			{
				continue;
			}


			unsigned rozwazanaDlugosc = graf.zwrocKoszt(i, j);

			if (najmniejszaOdlegosc == -1 || najmniejszaOdlegosc > rozwazanaDlugosc)
			{
				najmniejszaOdlegosc = rozwazanaDlugosc;
				nastepnyWierzcholek = j;
			}
		}
		aktualnaDroga.push_back(nastepnyWierzcholek);
	}
	aktualnaDroga.push_back(0);

	#pragma endregion

	bool czyPozostalCzas = true;
	bool zaZimno = false;
	zegar.start();
	float czas = 0;
	int licznik = 0;
	while (czyPozostalCzas==true)
	{
		while (zaZimno==false)
		{
			int licznikEpoki = 0;
			vector<unsigned> nastepnaDroga;
			int dlugoscNastepnejDrogi = -1;
			int start = 1;

			while (epoka>licznikEpoki)
			{
				if (vertexEx)
				{
					#pragma region wymiana wierzcholkow (VertexEx)
					for (int i = 1; i < graf.liczbaWierzcholkow - 1; i++)
					{
						for (int j = start; j < graf.liczbaWierzcholkow - 1; j++)
						{
							//zamienianie wierzchołka z samym sobą jest bezcelowe
							if (i == j)
							{
								continue;
							}

							//sąsiednia droga (zamienione dwa wierzchołki w porównaniu do aktualnej drogi)
							vector<unsigned> sasiedniaDroga = aktualnaDroga;

							//zamiana dwóch wierzchołków w drodze (o indeksie i oraz j), czyli tworzenie sąsiedniego rozwiązania
							unsigned tmp = sasiedniaDroga.at(j);
							sasiedniaDroga.at(j) = sasiedniaDroga.at(i);
							sasiedniaDroga.at(i) = tmp;

							unsigned dlugoscSasiedniejDrogi = 0;
							//obliczanie sąsiedniego rozwiązania
							for (int i = 1; i < sasiedniaDroga.size(); i++)
							{
								dlugoscSasiedniejDrogi += graf.zwrocKoszt(sasiedniaDroga.at(i - 1), sasiedniaDroga.at(i));
							}

							//jeżeli droga sąsiedniego rozwiązania jest mniejsza od najlepszego rozwiązania w pętli to ona staje się najlepszym rozwiązaniem w pętli
							if (dlugoscOptymalnejDrogi == -1 || dlugoscSasiedniejDrogi < dlugoscOptymalnejDrogi)
							{
								dlugoscOptymalnejDrogi = dlugoscSasiedniejDrogi;
								optymalnaDroga = sasiedniaDroga;
								dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
								aktualnaDroga = sasiedniaDroga;
							}

							if (dlugoscAktualnejDrogi > dlugoscSasiedniejDrogi)
							{
								dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
								aktualnaDroga = sasiedniaDroga;
								dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
								nastepnaDroga = sasiedniaDroga;
							}

							//jeżeli droga sąsiedniego rozwiązania jest mniejsza od najlepszego rozwiązania w pętli to ona staje się najlepszym rozwiązaniem w pętli
							if (dlugoscNastepnejDrogi == -1 || dlugoscNastepnejDrogi > dlugoscSasiedniejDrogi)
							{
								dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
								aktualnaDroga = sasiedniaDroga;
								dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
								nastepnaDroga = sasiedniaDroga;
							}
							else
							{
								int delta = dlugoscSasiedniejDrogi - dlugoscNastepnejDrogi;
								float los = ((float)rand() / (RAND_MAX));
								float funkcja = exp(((float)delta*(-1)) / (float)temperatura);
								if (los > funkcja)
								{
									dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
									aktualnaDroga = sasiedniaDroga;
									dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
									nastepnaDroga = sasiedniaDroga;
								}
							}
							licznikEpoki++;
							licznik++;
						}
					}
					#pragma endregion
				}
				else
				{
					#pragma region 2-opt
					for (int i = 1; i < graf.liczbaWierzcholkow - 2; i++)
					{
						for (int j = i; j < graf.liczbaWierzcholkow - 1; j++)
						{

							//sąsiednia droga (zamienione dwa wierzchołki w porównaniu do aktualnej drogi)
							vector<unsigned> sasiedniaDroga = aktualnaDroga;

							if (i != j)
							{
								if (i != j)
								{
									int dec = 0;
									if (i + 1 == j)
									{
										unsigned pom = sasiedniaDroga[i];
										sasiedniaDroga[i] = sasiedniaDroga[j];
										sasiedniaDroga[j] = pom;
									}
									else if (j == graf.liczbaWierzcholkow - 1)
									{
										dec = 0;
										for (int k = i; k < (j / 2) + 1; k++)
										{
											unsigned pom = sasiedniaDroga[k];
											sasiedniaDroga[k] = sasiedniaDroga[j - dec];
											sasiedniaDroga[j - dec] = pom;
											dec++;
										}
										dec = 0;
									}
									else
									{
										dec = 0;
										for (int k = i; k < (j / 2) + 1; k++)
										{
											unsigned pom = sasiedniaDroga[k];
											sasiedniaDroga[k] = sasiedniaDroga[j - dec];
											sasiedniaDroga[j - dec] = pom;
											dec++;
										}
									}
								}
							}

							unsigned dlugoscSasiedniejDrogi = 0;
							//obliczanie sąsiedniego rozwiązania
							for (int i = 1; i < sasiedniaDroga.size(); i++)
							{
								dlugoscSasiedniejDrogi += graf.zwrocKoszt(sasiedniaDroga.at(i - 1), sasiedniaDroga.at(i));
							}

							//jeżeli droga sąsiedniego rozwiązania jest mniejsza od najlepszego rozwiązania w pętli to ona staje się najlepszym rozwiązaniem w pętli
							if (dlugoscOptymalnejDrogi == -1 || dlugoscSasiedniejDrogi < dlugoscOptymalnejDrogi)
							{
								dlugoscOptymalnejDrogi = dlugoscSasiedniejDrogi;
								optymalnaDroga = sasiedniaDroga;
								dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
								aktualnaDroga = sasiedniaDroga;
							}

							if (dlugoscAktualnejDrogi > dlugoscSasiedniejDrogi)
							{
								dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
								aktualnaDroga = sasiedniaDroga;
								dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
								nastepnaDroga = sasiedniaDroga;
							}

							//jeżeli droga sąsiedniego rozwiązania jest mniejsza od najlepszego rozwiązania w pętli to ona staje się najlepszym rozwiązaniem w pętli
							if (dlugoscNastepnejDrogi == -1 || dlugoscNastepnejDrogi > dlugoscSasiedniejDrogi)
							{
								dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
								aktualnaDroga = sasiedniaDroga;
								dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
								nastepnaDroga = sasiedniaDroga;
							}
							else
							{
								int delta = dlugoscSasiedniejDrogi - dlugoscNastepnejDrogi;
								float los = ((float)rand() / (RAND_MAX));
								float funkcja = exp(((float)delta*(-1)) / (float)temperatura);
								if (los > funkcja)
								{
									dlugoscAktualnejDrogi = dlugoscSasiedniejDrogi;
									aktualnaDroga = sasiedniaDroga;
									dlugoscNastepnejDrogi = dlugoscSasiedniejDrogi;
									nastepnaDroga = sasiedniaDroga;
								}
							}
							licznikEpoki++;
							licznik++;
						}
					}
					#pragma endregion
				}

				aktualnaDroga = nastepnaDroga;
				//jeżeli aktualna optymalna droga nie istnieje, albo jest większa od najlepszej z pętli to najlepsza z pętli staje się aktualną optymalną drogą
				if (dlugoscOptymalnejDrogi == -1 || dlugoscOptymalnejDrogi > dlugoscNastepnejDrogi)
				{
					dlugoscOptymalnejDrogi = dlugoscNastepnejDrogi;
					optymalnaDroga = nastepnaDroga;
				}

				zegar.stop();
				czas = zegar.odczyt();
				//sprawdzanie czy zadany czas upłynął 
				if (czas > czasStopu)
				{
					czyPozostalCzas = false;
					zaZimno = true;
				}
				else if (licznikEpoki > 10000)
				{
					czyPozostalCzas = false;
					zaZimno = true;
				}
			}

			temperatura *= 0.99;
			if (temperatura < tMin)
			{
				zaZimno = true;
				czyPozostalCzas = false;
			}
			
			srand(time(NULL));
			//czyszczenie aktualnej drogi
			aktualnaDroga.clear();

			#pragma region losowanie wierzcholkow
			//losowanie 3 losowych wierzchołków w celu uwtorzenia pierwszej ścieżki

			unsigned pierwszy = rand() % graf.liczbaWierzcholkow;
			aktualnaDroga.push_back(pierwszy);

			unsigned drugi;
			do
			{
				drugi = rand() % graf.liczbaWierzcholkow;
			} while (drugi == pierwszy);

			aktualnaDroga.push_back(drugi);

			unsigned trzeci;
			do
			{
				trzeci = rand() % graf.liczbaWierzcholkow;
			} while (trzeci == pierwszy || trzeci == drugi);
			aktualnaDroga.push_back(trzeci);
			#pragma endregion

			#pragma region zachlanny
			//reszta drogi jest dopełniana za pomocą algorytmu zachłannego
			for (int i = 0; i < graf.liczbaWierzcholkow - 3; i++)
			{
				int najmniejszaOdlegosc = -1;
				unsigned nastepnyWierzcholek;
				for (int j = 0; j < graf.liczbaWierzcholkow; j++)
				{
					if (i == j)
					{
						continue;
					}

					bool uzytyWierzcholek = false;
					for (int k = 0; k < aktualnaDroga.size(); k++)
					{
						if (j == aktualnaDroga.at(k))
						{
							uzytyWierzcholek = true;
							break;
						}
					}

					if (uzytyWierzcholek)
					{
						continue;
					}

					unsigned rozwazanaDlugosc = graf.zwrocKoszt(i, j);
					if (najmniejszaOdlegosc == -1 || najmniejszaOdlegosc > rozwazanaDlugosc)
					{
						najmniejszaOdlegosc = rozwazanaDlugosc;
						nastepnyWierzcholek = j;
					}
				}
				aktualnaDroga.push_back(nastepnyWierzcholek);
			}
			aktualnaDroga.push_back(pierwszy);
			#pragma endregion

		}
	}
	return optymalnaDroga;
}

void Graf::wyswietl()
{
	for (int i = 0; i < liczbaWierzcholkow; i++)
	{
		for (int j = 0; j < liczbaWierzcholkow; j++)
		{
			cout << macierzGrafu[i][j] << " ";
		}
		cout << endl;
	}
}