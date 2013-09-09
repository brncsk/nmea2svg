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
 * Egy megjelen�t�si m�dhoz tartoz� visszah�v�si f�ggv�nyeket tartalmaz� strukt�ra
 */
typedef struct
{
	NMEA_MESSAGE_CALLBACK mc;				///< Az adatok feldolgoz�s��rt felel�s f�ggv�ny
	NMEA_FINALIZATION_CALLBACK fc;			///< Az SVG-�bra megrajzol�s��rt felel�s f�ggv�ny
	unsigned short int talker_type_flags;	///< A megjelen�t�s szempontj�b�l relev�ns �zenetk�ld�-eszk�z�ket kiv�laszt� flagek
	unsigned short int sentence_type_flags;	///< A meglejen�t�s szempontj�b�l relev�ns �zenett�pusokat kiv�laszt� flag
} NMEA_CALLBACK;

void nmea_parse();
static void nmea_parse_sentence(char *, NMEA_CALLBACK);
static NMEA_SENTENCE_TYPE nmea_get_sentence_type(char *);
static NMEA_TALKER_TYPE nmea_get_talker_type(char *);
static char **nmea_get_sentence_args(char *);
void nmea_free_sentence_args(char **);
#endif
