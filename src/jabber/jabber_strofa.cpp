#include "jabber_strofa.h"

Jabber::Strofa::Strofa(const char* nowy_nadawca, const char* nowy_odbiorca) : id(NULL)
{
	if (nowy_odbiorca != NULL)
		odbiorca = strdup(nowy_odbiorca);
	else
		odbiorca = NULL;
	if (nowy_nadawca != NULL)
		nadawca = strdup(nowy_nadawca);
	else
		nadawca = NULL;
	Wygeneruj_id();
}

Jabber::Strofa::Strofa(const Strofa& inna)
{
	if (inna.odbiorca != NULL)
		odbiorca = strdup(inna.odbiorca);
	else
		odbiorca = NULL;
	if (inna.nadawca != NULL)
		nadawca = strdup(inna.nadawca);
	else
		nadawca = NULL;
	if (inna.id != NULL)
		id = strdup(inna.id);
	else
		id = NULL;
}

Jabber::Strofa::~Strofa()
{
	if (odbiorca != NULL)
		delete[] odbiorca;
	if (nadawca != NULL)
		delete[] nadawca;
	if (id != NULL)
		delete[] id;
}

const char* Jabber::Strofa::Pobierz_nadawce() const
{
	return nadawca;
}

const char* Jabber::Strofa::Pobierz_odbiorce() const
{
	return odbiorca;
}

const char* Jabber::Strofa::Pobierz_id() const
{
	return id;
}

/*!
* @brief Funkcja, ktora sluzy do zmieniania nadawcy strofy
*
* @param nowy_nadawca Nadawca, jakiego chcemy ustawic 
*/
void Jabber::Strofa::Zmien_nadawce(const char* nowy_nadawca)
{
	if (nadawca != NULL)
		delete[] nadawca;
	if (nowy_nadawca != NULL)
		nadawca = strdup(nowy_nadawca);
	else
		nadawca = NULL;
}

/*!
* @brief Zmienia odbiorce wiadomosci
*
* Ta funkcja sluzy do zmiana odbiorcy wiadomosci.
* @param nowy_odbiorca Nowy odbiorca wiadomosci
*/
void Jabber::Strofa::Zmien_odbiorce(const char* nowy_odbiorca)
{
	if (odbiorca != NULL)
		delete[] odbiorca;
	if (nowy_odbiorca != NULL)
		odbiorca = strdup(nowy_odbiorca);
	else
		odbiorca = NULL;
}

const Jabber::Strofa& Jabber::Strofa::operator=(const Jabber::Strofa& inna)
{
	if (odbiorca != NULL)
		delete[] odbiorca;
	if (nadawca != NULL)
		delete[] nadawca;
	if (id != NULL)
		delete[] id;

	if (inna.nadawca != NULL)
		nadawca = strdup(inna.nadawca);
	else
		nadawca = NULL;
	if (inna.odbiorca != NULL)
		odbiorca = strdup(inna.odbiorca);
	else
		odbiorca = NULL;
	if (inna.id != NULL)
		id = strdup(inna.id);
	else
		id = NULL;

	return *this;
}

void Jabber::Strofa::Wygeneruj_id()
{
	const char* alfabet = "1234567890qwertyuiopasdfghjklzxcvbmn";
	int i;

	srand(time(NULL));
	if (id != NULL)
		delete[] id;
	id = new char[DLUGOSC_ID + 1];
	for (i = 0; i < DLUGOSC_ID; i++)
		id[i] = alfabet[rand() % strlen(alfabet)];
	id[10] = '\0';
}
