#ifndef PARSER_WIADOMOSCI_H
#define PARSER_WIADOMOSCI_H

namespace Jabber
{
	class Klient;
	template <class T>
	class Parser_wiadomosci
	{
		public:
			enum TYP_WIADOMOSCI{MESSAGE, PRESENCE, IQ};
			
			Parser_wiadomosci(TYP_WIADOMOSCI nowy_typ)
			{
				typ = nowy_typ;
				priorytet = 0;
			}
			Parser_wiadomosci(const Parser_wiadomosci& inny)
			{
				typ = inny.typ;
				priorytet = inny.priorytet;	
			}
			virtual ~Parser_wiadomosci() {};

			virtual bool Parsuj(Klient* klient, T& wiadomosc) = 0;

			TYP_WIADOMOSCI Pobierz_typ() const
			{
				return typ;
			}
		private:
			TYP_WIADOMOSCI typ;
			int priorytet;
	};
}

#endif
