#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace Jabber
{

	/*!
	*   @brief Funkcja sluzy do kopiwania podanej ilosci liter ze zrodla
	*   @param zrodlo Lancuch zrodlowy, z ktorego kopiowany jest podciag
	*   @param ilosc Ilosc znakow, ktore chcemy skopiowac
	*   @return Nowo zaalokowany lancuch znkow, ktory nalezy zwolnic za pomoca free()
	*/
	char* Kopiuj(const char* const zrodlo, int ilosc);
	void Dodaj_znak(char* cel, char znak);
	void Utnij_znak_nowej_lini(char* tekst);
	char* Wyciagnij_nazwe_ze_sciezki_linux(const char* sciezka);
	void Polacz(char** cel, const char* tekst);
	int my_strlen(const char* tekst);
}

