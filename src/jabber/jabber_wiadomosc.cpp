#include "jabber_wiadomosc.h"

/*!
*	 @brief Konstruktor klasy Jabber::Wiadomosc
*	 @param nadawca Uzytkownik, ktory wysyla wiadmosc, jesli to ty wysylasz wiadomosc, tutaj powinien byc twoj JID, inaczej serwer nie wysle wiadomosci i rozlaczy cie
*	 @param odbiorca Uzytkownik, do ktorego kierowana jest wiadomosc
*	 @param tytul Tytul wysylanej wiadomosc(nie wymagany, moze byc NULL i nic sie nie stanie)
*	 @param tresc Tresc wysylanej wiadomosc
*	 @return Nowo zaalokowana wiadomosc, nalezy zwolnic ja funkcja Jabber_wiadomosc_zniszcz()
*	 Ta funkcja sluzy do tworzenia wiadomosci, ktora potem mozna wyslac. Jesli to ty wysylasz wiadomosc, jako nadawce powinienes podac NULL, wtedy automatycznie zostanie ustawiony twoj jid
*/
	
Jabber::Wiadomosc::Wiadomosc(const char* nowy_nadawca, const char* nowy_odbiorca,
                             const char* nowy_tytul, const char* nowa_tresc) :
							 Strofa(nowy_nadawca, nowy_odbiorca)
{
	
	if (nowy_tytul != NULL)
		tytul = strdup(nowy_tytul);
	else
		tytul = NULL;
	
	if (nowa_tresc != NULL)
		tresc = strdup(nowa_tresc);
	else
		tresc = NULL;
}

Jabber::Wiadomosc::Wiadomosc(const Wiadomosc& inna): Strofa(inna)
{
	if (inna.tytul != NULL)
		tytul = strdup(inna.tytul);
	else
		tytul = NULL;

	if (inna.tresc != NULL)
		tresc = strdup(inna.tresc);
	else
		tresc = NULL;
}

/*!
* @brief Destruktor klasy Jabber::Wiadomosc
*
*/
Jabber::Wiadomosc::~Wiadomosc()
{
	if (tresc != NULL)
		delete[] tresc;
	if (tytul != NULL)
		delete[] tytul;
}

/*!
* @brief Zmienia tresc wiadomosci
* @param nowa_tresc Nowa tresc wiadomosci
*/
void Jabber::Wiadomosc::Zmien_tresc(const char* nowa_tresc)
{
	if (nowa_tresc == NULL)
		return;
	if (tresc != NULL)
		delete[] tresc;
	tresc = strdup(nowa_tresc);
}

/*!
* @brief Zwraca tresc wiadomosci
* @return Tresc wiadomosci
*/
const char* Jabber::Wiadomosc::Pobierz_tresc() const
{
	return tresc;
}

/*!
* @brief Sprawdza, czy podana wiadomosc jest poprawna
*
* Ta funkcja, czy podana wiadomosc zawiera poprawnego adresata, nadawce i tresc
* @return True, jesli wiadomosc jest poprawna, False jesli nie jest poprawna
*/
bool Jabber::Wiadomosc::Czy_poprawna() const
{
	if (tresc == NULL)
		return false;
	if (nadawca == NULL)
	    return false;
	if (odbiorca == NULL)
		return false;

	return true;
}

char* Jabber::Wiadomosc::Zamien_na_XML() const
{
	int dlugosc = 0;
	char* wynik = NULL;

	if (odbiorca == NULL || tresc == NULL)
		return NULL;

	dlugosc = strlen(odbiorca) + strlen(tresc) + 70; /* ilosc znakow w wiadomosci */		
	wynik = new char[dlugosc];
	sprintf(wynik, "<message to=\'%s\' type=\'chat\' xml:lang=\'en\'><body>%s</body></message>",
        odbiorca, tresc);

	return wynik;
}

const Jabber::Wiadomosc& Jabber::Wiadomosc::operator=(const Jabber::Wiadomosc& inna)
{
	Strofa::operator=(inna);

	if (tresc != NULL)
		delete[] tresc;
	if (inna.tresc != NULL)
		tresc = strdup(inna.tresc);
	else
		tresc = NULL;
	if (tytul != NULL)
		delete[] tytul;
	if (inna.tytul != NULL)
		tytul = strdup(inna.tytul);
	else
		tytul = NULL;
	return *this;
}
