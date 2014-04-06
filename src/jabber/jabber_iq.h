#ifndef JABBER_IQ_H
#define JABBER_IQ_H

#include "jabber_strofa.h"
#include "parser/parser_xml.h"

namespace Jabber
{
	class IQ : public Strofa
	{
		public:
            enum TYP {SET1, GET2, RESULT3, ERROR4};
			IQ(const char* nadawca, const char* odbiorca, TYP nowy_typ);
			IQ(const char* nadawca, const char* odbiorca, char* xml);
			IQ(const IQ& inny);
			virtual ~IQ();

			virtual char* Zamien_na_XML() const;
			virtual Wezel* Pobierz_korzen();

			virtual IQ& operator=(const IQ& inny);
			

		private:
			TYP typ;
			Wezel* korzen;
	};
}
			

#endif
