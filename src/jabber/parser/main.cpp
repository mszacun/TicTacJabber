#include "parser_xml.h"

int main()
{
	Parser_XML parser;
	const Wezel* korzen = NULL;

	parser.Parsuj("<xml><marcin>Jest mistrzem</marcin><kupa/><siku><mocz/></siku></xml>");
	korzen = parser.Pobierz_korzen();

	if (parser.Czy_poprawny())
	{
		printf("Pomyslnie sparsowano plik!\n");
		printf("Proba:\n%s\n", parser.Zamien_na_XML().c_str());		
	}	
	else
		printf("Uwaga! Wystapil blad w czasie parsowania\n");
	printf("Tresc elementu siku: %s\n", parser.Pobierz_tresc_elementu("siku"));
	printf("Tresc elementu marcin: %s\n", parser.Pobierz_tresc_elementu("marcin"));
	parser.Parsuj("<polecenia><ping/></polecenia>");
	if (parser.Czy_poprawny())
		printf("Sparsowano\n");
	return 0;
}
