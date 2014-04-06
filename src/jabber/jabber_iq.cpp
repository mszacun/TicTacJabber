#include "jabber_iq.h"

Jabber::IQ::IQ(const char* nadawca, const char* odbiorca, TYP nowy_typ) : Strofa(nadawca, odbiorca)
{
	typ = nowy_typ;
	korzen = new Wezel("iq", NULL, NULL, NULL, NULL, NULL);
	korzen->Dodaj_atrybut("to", this->odbiorca);
	korzen->Dodaj_atrybut("id", this->id);
	if (typ == SET1)
		korzen->Dodaj_atrybut("type", "set");
	if (typ == GET2)
		korzen->Dodaj_atrybut("type", "get");
	if (typ == RESULT3)
		korzen->Dodaj_atrybut("type", "result");
	if (typ == ERROR4)
		korzen->Dodaj_atrybut("type", "error");
}

Jabber::IQ::IQ(const char* nadawca, const char* odbiorca, char* xml) : Strofa(nadawca, odbiorca)
{
	try
	{
		printf("Parsuje: %s\n", xml);
		korzen = new Wezel(xml, NULL, NULL);
		if (strcmp(korzen->Pobierz_atrybut("type"), "set") == 0)    
		    typ = SET1;
		if (strcmp(korzen->Pobierz_atrybut("type"), "get") == 0)
		    typ = GET2;
		if (strcmp(korzen->Pobierz_atrybut("type"), "result") == 0)
		    typ = RESULT3;
		if (strcmp(korzen->Pobierz_atrybut("type"), "error") == 0)
		    typ = ERROR4;
		/* podmieniamy id na to ze strofy */
		if (id != NULL)
			delete[] id;
		id = strdup(korzen->Pobierz_atrybut("id"));
	}
	catch (int)
	{
		puts("OSTRZEZENIE: Wystapil blad podczas parsowania tekstu IQ\n");
	}
}

Jabber::IQ::IQ(const IQ& inny) : Strofa(inny)
{
	typ = inny.typ;
	if (korzen != NULL)
		delete korzen;
	korzen = inny.korzen->Klonuj(NULL, NULL);
}

Jabber::IQ::~IQ()
{
	if (korzen != NULL)
		delete korzen;
}

char* Jabber::IQ::Zamien_na_XML() const
{
	return strdup(korzen->Zamien_na_XML(0).c_str());
}

Wezel* Jabber::IQ::Pobierz_korzen()
{
	return korzen;
}

Jabber::IQ& Jabber::IQ::operator=(const IQ& inny)
{
	Strofa::operator=(inny);

	if (korzen != NULL)
		delete korzen;
	typ = inny.typ;
	if (inny.korzen != NULL)
		korzen = inny.korzen->Klonuj(NULL, NULL);
	else
		korzen = NULL;

	return *this;
}
