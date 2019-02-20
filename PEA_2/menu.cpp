#include "pch.h"
using namespace std;



void menu()
{
	#pragma region ustawienia
	// Kadencja tabu search
	int kadencja = 40;
	// Kryterium dywersyfikacji, liczba iteracji bez poprawy
	int iteracjeDD = 10000;

	// Domyslny czas zatrzymania algorytmu tabu search [s]
	float czasStopu = 10;

	// Domyslny stan dywersyfikacji
	bool dywersyfikacja = true;

	// Szczegolowe wyswietlanie wynikow dzialania algorytmu
	bool szczegoly = false;
	
	// dlugosc epoki, liczba iteracji do ponownego losowania rozwiazania
	int dlugoscEpoki = 100000;

	// poczatkowa temperatura algorytmu wyzarzania
	float temperaturaPoczatkowa = 1000000;

	// minimalna temperatura algorytmu wyzarzania
	const float temperaturaMinimalna = 0.000001;

	// rodzaj s¹siedztwa (true=zamiana wierzcho³ków, false=2-otp)
	bool vertexEx = true;
	#pragma endregion
	Zegar zegar;                // czasomierz
	Graf *graf = NULL;			// wskaznik na aktualny graf

	char wybor = 0;
	do
	{
		#pragma region wypisanie menu
		cout << "1 - Wczytaj dane" << endl;
		cout << "2 - Dywersyfikacja TS";
		if (dywersyfikacja)
		{
			cout << " (wlaczona)" << endl;
		}
		else
		{
			cout << " (wylaczona)" << endl;
		}
		cout << "3 - Czas zatrzymania (" << czasStopu << "s)" << endl;
		cout << "4 - Wybor sasiedztwa(";
		if (vertexEx)
		{
			cout << "Zamiana wierzcholkow)" << endl;
		}
		else
		{
			cout << "2-opt)" << endl;
		}
		cout << "5 - Szczegoly";
		if (szczegoly)
		{
			cout << " (wlaczone)" << endl;
		}
		else
		{
			cout << " (wylaczone)" << endl;
		}
		cout << "6 - Tabu search" << endl;
		cout << "7 - Symulowane Wyzarzanie" << endl;
		cout << "Wyjdz: 0" << endl;
		cout << "Wybierz: ";
		cin >> wybor;
		cout << endl;
		#pragma endregion
		switch (wybor)
		{
		case '1':
		{
			string nazwaPliku;
			string odczyt;
			ifstream plik;
			cout << "Podaj nazwe pliku: ";
			cin >> nazwaPliku;

			plik.open(nazwaPliku.c_str());
			if (plik.is_open())
			{
				do
				{
					plik >> odczyt;
				}
				while (odczyt != "DIMENSION:");

				plik >> odczyt;
				int vertex = atoi(odczyt.c_str());

				do
				{
					plik >> odczyt;
				}
				while (odczyt != "EDGE_WEIGHT_FORMAT:");

				plik >> odczyt;
				if (odczyt == "FULL_MATRIX")
				{
					if (graf != NULL)
					{
						delete graf;
					}

					graf = new Graf(vertex);

					do
					{
						plik >> odczyt;
					}
					while (odczyt != "EDGE_WEIGHT_SECTION");

					for (int i = 0; i < vertex; i++)
					{
						for (int j = 0; j < vertex; j++)
						{
							plik >> odczyt;
							int koszt = atoi(odczyt.c_str());

							if (i != j)
							{
								graf->dodajWierzcholek(i, j, koszt);
							}

						}
					}

					cout << "Wczytano - liczba wierzcholkow: " << vertex << endl;
					cout << endl;
				}
				else
				{
					cout << "Nieobslugiwany format " << odczyt << endl;
					cout << endl;
				}

				plik.close();
			}
			else
			{
				cout << "Brak pliku " << nazwaPliku << endl;
				cout << endl;
			}
		}
		break;

		case '2':
		{
			dywersyfikacja = !dywersyfikacja;
		}
		break;

		case '3':
		{
			cout << "Poprzedni czas pracy: " << czasStopu << endl;
			cout << "Podaj nowy czas: ";
			cin >> czasStopu;
			cout << endl;
		}
		break;

		case '4':
		{
			vertexEx = !vertexEx;
		}
		break;

		case '5':
		{
			szczegoly = !szczegoly;

		}
		break;

		case '6':
		{
			if (graf != NULL)
			{
				int n = graf->zwrocLiczbeWierzcholkow();
				kadencja = 40;
				iteracjeDD= 10*n;
				cout << "Kadencja: " << kadencja << endl;
				cout << "Czas zatrzymania algorytmu [s]: " << czasStopu << endl;

				if (dywersyfikacja == true)
					cout << "Dywersyfikacja wlaczona, kryterium: " << iteracjeDD << " iteracji" << endl;
				else
					cout << "Dywersyfikacja wylaczona" << endl;

				cout << endl;

				zegar.start();
				vector<unsigned> droga = Graf::tabuSearch(*graf, kadencja, dywersyfikacja, iteracjeDD, czasStopu,vertexEx);
				zegar.stop();

				// Wyswietlenie trasy
				unsigned drogaOdPoczatku = 0;
				unsigned dlugosc = 0;
				if (szczegoly)
				{
					cout << "Wierzcholek \t odleglosc od poprzednika \t dl. cyklu" << endl;
					cout << droga.at(0) << '\t' << dlugosc << '\t' << drogaOdPoczatku << endl;
				}
				for (int i = 1; i < droga.size(); i++)
				{
					dlugosc = graf->zwrocKoszt(droga.at(i - 1), droga.at(i));
					drogaOdPoczatku += dlugosc;
					if (szczegoly)
					{
						cout << droga.at(i) << '\t' << dlugosc << '\t' << drogaOdPoczatku << endl;
					}
				}
				cout << "Dlugosc trasy: " << drogaOdPoczatku << endl;
				cout << endl;
				cout << "Czas wykonania algorytmu [s]: " << zegar.odczyt() << endl;
			}
			else
				cout << "Brak zaladowanych danych" << endl;
			cout << endl;
		}
		break;

		case '7':
		{
			if (graf != NULL)
			{
				int n = graf->zwrocLiczbeWierzcholkow();
				dlugoscEpoki = 10*n;
				cout << "Epoka: " << dlugoscEpoki << endl;
				cout << "Czas zatrzymania algorytmu [s]: " << czasStopu << endl;
				cout << "Temperatura poczatkowa: " << temperaturaPoczatkowa << endl;
				cout << "Temperatura minimalna: " << temperaturaMinimalna << endl;
				cout << endl;

				zegar.start();
				vector<unsigned> droga = Graf::symulowaneWyrzazanie(*graf, temperaturaPoczatkowa, temperaturaMinimalna, czasStopu, dlugoscEpoki, vertexEx);
				zegar.stop();

				// Wyswietlenie trasy
				unsigned drogaOdPoczatku = 0;
				unsigned dlugosc = 0;
				if (szczegoly)
				{
					cout << "Wierzcholek \t odleglosc od poprzednika \t dl. cyklu" << endl;
					cout << droga.at(0) << '\t' << dlugosc << '\t' << drogaOdPoczatku << endl;
				}
				for (int i = 1; i < droga.size(); i++)
				{
					dlugosc = graf->zwrocKoszt(droga.at(i - 1), droga.at(i));
					drogaOdPoczatku += dlugosc;
					if (szczegoly)
					{
						cout << droga.at(i) << '\t' << dlugosc << '\t' << drogaOdPoczatku << endl;
					}
				}
				cout << "Dlugosc trasy: " << drogaOdPoczatku << endl;
				cout << endl;
				cout << "Czas wykonania algorytmu [s]: " << zegar.odczyt() << endl;
			}
			else
				cout << "Brak zaladowanych danych" << endl;
			cout << endl;
		}
		break;

		case '8':
		{
			if (graf != NULL)
			{
				graf->wyswietl();
			}

		}

		case '0':
		{
			if (graf != NULL)
			{
				delete graf;
			}
			return;
		}
		break;

		default:
		{
			cout << "Nieprawidlowy wybor" << endl;
			cout << endl;
		}
		}
	}
	while (true);
}