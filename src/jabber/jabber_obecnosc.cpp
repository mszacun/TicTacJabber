#include "jabber_obecnosc.h"


/* Konstryktor uzywany do tworzenie strof sluzacych do zmiany statusu */
Jabber::Obecnosc::Obecnosc(const char* nowy_nadawca, const char* nowy_odbiorca,
			         STATUS nowy_status, const char* nowy_opis, int nowy_priorytet):
	Strofa(nowy_nadawca, nowy_odbiorca), typ(NIEWAZNE), status(nowy_status),
	priorytet(nowy_priorytet)
{
	if (nowy_opis != NULL)
		opis = strdup(nowy_opis);
	else
		opis = NULL;
}

/* Konstruktor uzywany do tworzenia strof sluzacych do zarzadzania rosterem */
Jabber::Obecnosc::Obecnosc(const char* nowy_nadawca, const char* nowy_odbiorca,
			         TYP nowy_typ, const char* nowy_opis):
					Strofa(nowy_nadawca, nowy_odbiorca), typ(nowy_typ), status(NIEWAZNE2),
					opis(NULL), priorytet(0)
{
	if (nowy_opis != NULL)
		opis = strdup(nowy_opis);
	else
		opis = NULL;
}

Jabber::Obecnosc::Obecnosc(const Obecnosc& inna): Strofa(inna)
{
	typ = inna.typ;
	status = inna.status;
	priorytet = inna.priorytet;
	if (inna.opis != NULL)
		opis = strdup(inna.opis);
	else
		opis = NULL;
}

Jabber::Obecnosc::~Obecnosc()
{
	if (opis != NULL)
		delete[] opis;
}

char* Jabber::Obecnosc::Zamien_na_XML() const
{
	Wezel* wezel = new Wezel("presence", NULL, NULL, NULL, NULL, NULL);
	
	if (typ != NIEWAZNE && status == NIEWAZNE2)
	{
		wezel->Dodaj_atrybut("to", odbiorca);
		wezel->Dodaj_atrybut("type", Typ_na_tekst(typ));
		if (opis != NULL)
		{
			Wezel* temp = new Wezel("status", opis, NULL, NULL, NULL, NULL);
			wezel->Dodaj_dziecko(temp);
			delete temp;
		}
		return strdup(wezel->Zamien_na_XML(0).c_str());
	}
	if (status != NIEWAZNE2 && typ == NIEWAZNE)
	{
		Wezel* temp = new Wezel("show", Status_na_tekst(status), NULL, NULL, NULL, NULL);
		wezel->Dodaj_dziecko(temp);
		delete temp;

		char priorytet_tekst[10];
		sprintf(priorytet_tekst, "%d", priorytet);		        
		temp = new Wezel("priority", priorytet_tekst, NULL, NULL, NULL, NULL);
		wezel->Dodaj_dziecko(temp);
		delete temp;
		
		if (opis != NULL)
		{
			temp = new Wezel("status", opis, NULL, NULL, NULL, NULL);
			wezel->Dodaj_dziecko(temp);
			delete temp;
		}
		return strdup(wezel->Zamien_na_XML(0).c_str());
	}
	return NULL;
}

const char* Jabber::Obecnosc::Typ_na_tekst(TYP typ_do_zamiany)
{
	switch (typ_do_zamiany)
	{
		case NIEDOSTEPNY:
			return "unavailable";
		case PROSBA_O_SUBSKRYPCJE:
			return "subscribe";
		case ZASUBSKRYBOWANY:
			return "subscribed";
		case USUN_SUBSKRYPCJE:
			return "unsubscribe";
		case KONIEC_SUBSKRYPCJI:
			return "unsubscribed";
		case PROBA:
			return "probe";
		case BLAD:
			return "error";
		default:
			return NULL;
	}
	return NULL;
}

Jabber::Obecnosc::TYP Jabber::Obecnosc::Tekst_na_typ(const char* tekst)
{
	if (tekst == NULL)
		return Jabber::Obecnosc::BLAD;
	if (strcmp(tekst, "unavailable") == 0)
		return Jabber::Obecnosc::NIEDOSTEPNY;
	if (strcmp(tekst, "subscribe") == 0)
		return Jabber::Obecnosc::PROSBA_O_SUBSKRYPCJE;
	if (strcmp(tekst, "subscribed") == 0)
		return Jabber::Obecnosc::ZASUBSKRYBOWANY;
	if (strcmp(tekst, "unsubscribe") == 0)
		return Jabber::Obecnosc::USUN_SUBSKRYPCJE;
	if (strcmp(tekst, "unsubscribed") == 0)
		return Jabber::Obecnosc::KONIEC_SUBSKRYPCJI;
	if (strcmp(tekst, "probe") == 0)
	    return Jabber::Obecnosc::PROBA;
	return Jabber::Obecnosc::BLAD;
}

const char* Jabber::Obecnosc::Status_na_tekst(STATUS status_do_zamiany)
{
	switch (status_do_zamiany)
	{
		case DOSTEPNY:
			return "chat";
		case NIE_PRZESZKADZAC:
			return "dnd";
		case ZARAZ_WRACAM:
			return "away";
		default:
			return NULL;
	}
}

Jabber::Obecnosc::STATUS Jabber::Obecnosc::Tekst_na_status(const char* tekst)
{
	if (tekst == NULL)
		return Jabber::Obecnosc::DOSTEPNY;
	if (strcmp(tekst, "dnd") == 0)
		return Jabber::Obecnosc::NIE_PRZESZKADZAC;
	if (strcmp(tekst, "away") == 0)
		return Jabber::Obecnosc::ZARAZ_WRACAM;
    return Jabber::Obecnosc::DOSTEPNY;
}

void Jabber::Obecnosc::Zmien_typ(TYP nowy_typ)
{
	typ = nowy_typ;
}

void Jabber::Obecnosc::Zmien_status(STATUS nowy_status)
{
	status = nowy_status;
}

void Jabber::Obecnosc::Zmien_priorytet(int nowy_priorytet)
{
	priorytet = nowy_priorytet;
}

void Jabber::Obecnosc::Zmien_opis(const char* nowy_opis)
{
	if (opis != NULL)
		delete[] opis;
	if (nowy_opis != NULL)
		opis = strdup(nowy_opis);
	else
		opis = NULL;
}

Jabber::Obecnosc::TYP Jabber::Obecnosc::Pobierz_typ() const
{
	return typ;
}

Jabber::Obecnosc::STATUS Jabber::Obecnosc::Pobierz_status() const
{
	return status;
}

int Jabber::Obecnosc::Pobierz_priorytet() const
{
	return priorytet;
}

const char* Jabber::Obecnosc::Pobierz_opis() const
{
	return opis;
}

const Jabber::Obecnosc& Jabber::Obecnosc::operator==(const Obecnosc& inna)
{
	Strofa::operator=(inna);
	
	typ = inna.typ;
	status = inna.status;
	priorytet = inna.priorytet;

	if (opis != NULL)
		delete[] opis;
	if (inna.opis != NULL)
		opis = strdup(inna.opis);
	else
		opis = NULL;
	return *this;
}
