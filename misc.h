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
 * A program beállításait tartalmazó struktúra
 */
typedef struct
{
// 1. Display settings
	enum { TRACK, SATS } display_mode;	///< Megjelenítési mód
	unsigned long im_scale;				///< A kép méretaránya

// 2. Connection-related settings
	struct _conninfo					///< A hálózati kapcsolat részletei
	{
		char *hostname;					///< Kiszolgálónév
		char *port;						///< Port szám
	} conninfo;
	int sockfd;							///< A socket-leíró száma

// 3. Other stuff
	BOOL using_gpsd;					///< GPSD használata?
	BOOL verbose;						///< Részletes kimenet generálása
	unsigned long runtime;				///< Maximális futási idõ
	FILE *outfile;						///< A kimeneti fájlra mutató leíró

} OPTIONS;

void finalize();

#endif
