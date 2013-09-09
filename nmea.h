#ifndef __NMEA_H
#define __NMEA_H

#include <stdio.h>

typedef
	enum { GGA = 1, RMC = 2, GLL = 4, HDG = 8, GSV = 16, RMZ = 32 }
	NMEA_SENTENCE_TYPE;
	
typedef
	enum { GP = 1, HC = 2, PG = 4 }
	NMEA_TALKER_TYPE;

typedef void (* NMEA_MESSAGE_CALLBACK)
	(NMEA_TALKER_TYPE, NMEA_SENTENCE_TYPE, char **);
typedef void (* NMEA_FINALIZATION_CALLBACK)
	(FILE *);

#define NMEA_CALLBACK_DECLARATION(f) \
	void f(NMEA_TALKER_TYPE, NMEA_SENTENCE_TYPE, char **);
#define NMEA_FINALIZATION_DECLARATION(f) \
	void f(FILE *);
	

/**
 * Egy megjelenítési módhoz tartozó visszahívási függvényeket tartalmazó struktúra
 */
typedef struct
{
	NMEA_MESSAGE_CALLBACK mc;				///< Az adatok feldolgozásáért felelõs függvény
	NMEA_FINALIZATION_CALLBACK fc;			///< Az SVG-ábra megrajzolásáért felelõs függvény
	unsigned short int talker_type_flags;	///< A megjelenítés szempontjából releváns üzenetküldõ-eszközöket kiválasztó flagek
	unsigned short int sentence_type_flags;	///< A meglejenítés szempontjából releváns üzenettípusokat kiválasztó flag
} NMEA_CALLBACK;

void nmea_parse();
static void nmea_parse_sentence(char *, NMEA_CALLBACK);
static NMEA_SENTENCE_TYPE nmea_get_sentence_type(char *);
static NMEA_TALKER_TYPE nmea_get_talker_type(char *);
static char **nmea_get_sentence_args(char *);
void nmea_free_sentence_args(char **);
#endif
