#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "misc.h"
#include "nmea.h"
#include "nmea-cb.h"

OPTIONS o;

extern NMEA_CALLBACK callbacks[];

static void usage();
static void populate_conninfo(CONNINFO *, char *);

/**
 * A program belépési pontja
 * @param argc A parancssori argumentumok száma
 * @param argv A parancssori argumentumok tömbje
 */
int main(int argc, char **argv)
{
	int c, status;
	
	struct addrinfo hints;
	struct addrinfo *ai;
	
	int errno;

	opterr = 0;
	
	while ((c = getopt(argc, argv, "dhvt:s:m:")) != -1)
	{
		switch(c)		
		{
			case 'm':
				if(!strcmp(optarg, "=track")) o.display_mode = TRACK;
				else if(!strcmp(optarg, "=sats")) o.display_mode = SATS;
			break;
			
			case 's':
				if(*optarg != '=')
					die("Az -s kapcsoló argumentuma érvénytelen.");

				optarg++;

				o.im_scale = atof(optarg);

				if(o.im_scale == 0)
					die("Az -s kapcsoló argumentuma érvénytelen.");
				
			break;

			case 'd':
				o.using_gpsd = TRUE;
			break;
			
			case 'h':
				usage();
			break;
			
			case 'v':
				o.verbose = TRUE;
			break;
			
			case 't':
				if(*optarg != '=')
					die("A -t kapcsoló argumentuma érvénytelen.");

				o.runtime = atoi(optarg + 1);

				if(o.runtime == 0)
					die("A -t kapcsoló argumentuma érvénytelen.");

			break;
			
			case '?':
				if(optopt == 't')
				{
					die("Érvénytelen futási idõtartam.");
				}
				else
				{
					die("Ismeretlen kapcsoló: -%c.", optopt);
				}
			break;
		}
		
	}

	if(optind + 1 >= argc)
		usage();
		
	// Parse hostname/port info
	
	populate_conninfo(&o.conninfo, argv[optind++]);

verbose_msg("Kimeneti fájl megnyitása...");
	
	if(NULL == (o.outfile = fopen(argv[optind], "w")))
		die("Nem sikerült létrehozni a kimeneti fájlt (%s).", argv[optind]);

verbose_msg("Hosztnév: %s, port: %s.", o.conninfo.hostname, o.conninfo.port);
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if (0 != (status = getaddrinfo
		(o.conninfo.hostname, o.conninfo.port, &hints, &ai)))
			die("Hálózati hiba: %s\n", gai_strerror(status));
		
verbose_msg("A getaddrinfo() sikeres.")
	
	// Get a socket...
	
	o.sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	
verbose_msg("Socket-leíró száma: %d", o.sockfd);
	
	// ...and here we go.
	
verbose_msg("Kísérlet a csatlakozásra...");
	
	if(-1 == connect(o.sockfd, ai->ai_addr, ai->ai_addrlen))
		die("Sikertelen csatlakozás.");

	// If we use GPSD get it up and running
	if(o.using_gpsd == TRUE)
	{
		write(o.sockfd, "r\n", 2);
	}
		
verbose_msg("OK.");

	nmea_parse();
	finalize();
	
	return 0;
}

/**
 * Az adatgyûjtés abbahagyása és az SVG-generálás kikényszerítése a futási idõ lejárta elõtt
 */
void finalize()
{
	callbacks[o.display_mode].fc(o.outfile);
	fclose(o.outfile);
	exit(0);
}

/**
 * A kapcsolat adatait tartalmazó struktúra feltöltése adatokkal
 * @param c A CONNINFO struktúra
 * @param arg A kiszolgálónevet és portszámot tartalmazó sztring
 */
static void populate_conninfo(CONNINFO *c, char *arg)
{
	char *cln, *port;

	if(NULL == (cln = strchr(arg, ':')))
	{
		/* Assume the whole argument is a port number */
		if(0 == atoi(arg))
		{
			die("Érvénytelen portszám.")
		}
		else
		{
			c->hostname = "127.0.0.1";
			c->port = arg;
			
			return;
		}
	}
	else
	{
		if(cln == (arg + strlen(arg) - 1))
			die("Érvénytelen hosztnév.")
		else
		{
			if(0 == atoi(cln + 1))
				die("Érvénytelen portszám.")
			
			*cln = '\0';
				
			c->hostname = arg;
			c->port = cln + 1;
			
			return;
		}
	}
}

/**
 * Rövid használati útmutató kiírása
 */
static void usage()
{
	printf(
		"\nHasználat: nmea2svg "
			"[-hv] [t=<time>] [s=<scale>] [-m[={track|alt|sats}]] [<host>:]<port> <outfile>\n\n"		
		BOLD_ON "\t-h\t " BOLD_OFF "Súgó megjelenítése\n"
		BOLD_ON "\t-v\t " BOLD_OFF "Hibakeresési információ megjelenítése\n"
		BOLD_ON "\t-t\t " BOLD_OFF "Futási idõ beállítása <time> másodpercre\n"
		BOLD_ON "\t-s\t " BOLD_OFF "Az SVG-fájl földrajzi méretarányának beállítása 1:(<scale>*1000) -re\n"
		BOLD_ON "\t-m\t " BOLD_OFF "Megjelenítési mód beállítása:\n"
		BOLD_ON "\t\t\ttrack: " BOLD_OFF "Az megtett útvonal kirajzolása;\n"
		BOLD_ON "\t\t\tsats: " BOLD_OFF "Látható mûholdak kirajzolása.\n"
		BOLD_ON "\thost\t " BOLD_OFF "A kiszolgáló neve\n"
		BOLD_ON "\tport\t " BOLD_OFF "A kiszolgálónévhez tartozó portszám\n"
		BOLD_ON "\toutfile\t " BOLD_OFF "A kimeneti fájl neve\n\n"
	);
	
	exit(-1);
}
