#ifndef __NMEA_CB_H
#define __NMEA_CB_H

#include <stdio.h>

NMEA_CALLBACK_DECLARATION(nmea_cb_track);
NMEA_FINALIZATION_DECLARATION(nmea_fn_track);

NMEA_CALLBACK_DECLARATION(nmea_cb_sats);
NMEA_FINALIZATION_DECLARATION(nmea_fn_sats);

/**
 * Egy k�t dimenzi�s t�rben l�v� pontot le�r� strukt�ra
 */
typedef struct
{
	double x;				///< X-koordin�ta
	double y;				///< Y-koordin�ta
} POINT;

/**
 * Egy �tvonal r�szleteit tartalmaz� strukt�ra
 */
typedef struct
{
	POINT   center;			///< Az els� be�rkez� koordin�ta
	POINT   min;			///< A legalacsonyabb �rt�keket tartalmaz� pont
	POINT   max;			///< A legmagasabb �rt�keket tartalmaz� pont
	POINT   last;			///< Az el�z� p�r koordin�ta
	char   *path_data;		///< A k�sz�l� adathalmaz a "path" SVG-elem form�tum�ban
	char   *pdc;			///< A path_data els� szabad karakter�re mutat� pointer
	size_t  l;				///< A path_data sz�m�ra lefoglalt mem�ria m�rete
} TRACK_INFO;

/**
 *	Egy m�hold aktu�lis helyzet�t tartalmaz� l�ncolt strukt�ra
 */
typedef struct _SAT_INFO
{
	char *n;				///< A m�hold azonos�t�ja sz�vegk�nt
	signed azimuth;			///< A m�hold poz�ci�j�nak tet�ponti sz�ge
	signed elevation;		///< A m�hold magass�ga
	struct _SAT_INFO *next;	///< A lista k�vetkez� eleme
} SAT_INFO;

#endif
