#ifndef __MISC_H
#define __MISC_H

#define verbose_msg(s, ...) \
if(o.verbose) \
	printf("nmea2svg: " s "\n", ##__VA_ARGS__);
#define die(s, ...) \
{ \
	fprintf(stderr, "nmea2svg: " s "\n", ##__VA_ARGS__); \
	exit(-1); \
}

#define BOLD_ON 	"\x1b[1m"
#define BOLD_OFF	"\x1b[22m"

typedef enum { FALSE, TRUE } BOOL;

typedef struct _conninfo CONNINFO;

/**
 * A program be�ll�t�sait tartalmaz� strukt�ra
 */
typedef struct
{
// 1. Display settings
	enum { TRACK, SATS } display_mode;	///< Megjelen�t�si m�d
	unsigned long im_scale;				///< A k�p m�retar�nya

// 2. Connection-related settings
	struct _conninfo					///< A h�l�zati kapcsolat r�szletei
	{
		char *hostname;					///< Kiszolg�l�n�v
		char *port;						///< Port sz�m
	} conninfo;
	int sockfd;							///< A socket-le�r� sz�ma

// 3. Other stuff
	BOOL using_gpsd;					///< GPSD haszn�lata?
	BOOL verbose;						///< R�szletes kimenet gener�l�sa
	unsigned long runtime;				///< Maxim�lis fut�si id�
	FILE *outfile;						///< A kimeneti f�jlra mutat� le�r�

} OPTIONS;

void finalize();

#endif
