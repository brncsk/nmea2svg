#ifndef __NMEA_CB_H
#define __NMEA_CB_H

#include <stdio.h>

NMEA_CALLBACK_DECLARATION(nmea_cb_track);
NMEA_FINALIZATION_DECLARATION(nmea_fn_track);

NMEA_CALLBACK_DECLARATION(nmea_cb_sats);
NMEA_FINALIZATION_DECLARATION(nmea_fn_sats);

/**
 * Egy két dimenziós térben lévõ pontot leíró struktúra
 */
typedef struct
{
	double x;				///< X-koordináta
	double y;				///< Y-koordináta
} POINT;

/**
 * Egy útvonal részleteit tartalmazó struktúra
 */
typedef struct
{
	POINT   center;			///< Az elsõ beérkezõ koordináta
	POINT   min;			///< A legalacsonyabb értékeket tartalmazó pont
	POINT   max;			///< A legmagasabb értékeket tartalmazó pont
	POINT   last;			///< Az elõzõ pár koordináta
	char   *path_data;		///< A készülõ adathalmaz a "path" SVG-elem formátumában
	char   *pdc;			///< A path_data elsõ szabad karakterére mutató pointer
	size_t  l;				///< A path_data számára lefoglalt memória mérete
} TRACK_INFO;

/**
 *	Egy mûhold aktuális helyzetét tartalmazó láncolt struktúra
 */
typedef struct _SAT_INFO
{
	char *n;				///< A mûhold azonosítója szövegként
	signed azimuth;			///< A mûhold pozíciójának tetõponti szöge
	signed elevation;		///< A mûhold magassága
	struct _SAT_INFO *next;	///< A lista következõ eleme
} SAT_INFO;

#endif
