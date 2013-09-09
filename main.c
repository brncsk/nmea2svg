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
 * A program bel�p�si pontja
 * @param argc A parancssori argumentumok sz�ma
 * @param argv A parancssori argumentumok t�mbje
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
					die("Az -s kapcsol� argumentuma �rv�nytelen.");

				optarg++;

				o.im_scale = atof(optarg);

				if(o.im_scale == 0)
					die("Az -s kapcsol� argumentuma �rv�nytelen.");
				
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
					die("A -t kapcsol� argumentuma �rv�nytelen.");

				o.runtime = atoi(optarg + 1);

				if(o.runtime == 0)
					die("A -t kapcsol� argumentuma �rv�nytelen.");

			break;
			
			case '?':
				if(optopt == 't')
				{
					die("�rv�nytelen fut�si id�tartam.");
				}
				else
				{
					die("Ismeretlen kapcsol�: -%c.", optopt);
				}
			break;
		}
		
	}

	if(optind + 1 >= argc)
		usage();
		
	// Parse hostname/port info
	
	populate_conninfo(&o.conninfo, argv[optind++]);

verbose_msg("Kimeneti f�jl megnyit�sa...");
	
	if(NULL == (o.outfile = fopen(argv[optind], "w")))
		die("Nem siker�lt l�trehozni a kimeneti f�jlt (%s).", argv[optind]);

verbose_msg("Hosztn�v: %s, port: %s.", o.conninfo.hostname, o.conninfo.port);
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if (0 != (status = getaddrinfo
		(o.conninfo.hostname, o.conninfo.port, &hints, &ai)))
			die("H�l�zati hiba: %s\n", gai_strerror(status));
		
verbose_msg("A getaddrinfo() sikeres.")
	
	// Get a socket...
	
	o.sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	
verbose_msg("Socket-le�r� sz�ma: %d", o.sockfd);
	
	// ...and here we go.
	
verbose_msg("K�s�rlet a csatlakoz�sra...");
	
	if(-1 == connect(o.sockfd, ai->ai_addr, ai->ai_addrlen))
		die("Sikertelen csatlakoz�s.");

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
 * Az adatgy�jt�s abbahagy�sa �s az SVG-gener�l�s kik�nyszer�t�se a fut�si id� lej�rta el�tt
 */
void finalize()
{
	callbacks[o.display_mode].fc(o.outfile);
	fclose(o.outfile);
	exit(0);
}

/**
 * A kapcsolat adatait tartalmaz� strukt�ra felt�lt�se adatokkal
 * @param c A CONNINFO strukt�ra
 * @param arg A kiszolg�l�nevet �s portsz�mot tartalmaz� sztring
 */
static void populate_conninfo(CONNINFO *c, char *arg)
{
	char *cln, *port;

	if(NULL == (cln = strchr(arg, ':')))
	{
		/* Assume the whole argument is a port number */
		if(0 == atoi(arg))
		{
			die("�rv�nytelen portsz�m.")
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
			die("�rv�nytelen hosztn�v.")
		else
		{
			if(0 == atoi(cln + 1))
				die("�rv�nytelen portsz�m.")
			
			*cln = '\0';
				
			c->hostname = arg;
			c->port = cln + 1;
			
			return;
		}
	}
}

/**
 * R�vid haszn�lati �tmutat� ki�r�sa
 */
static void usage()
{
	printf(
		"\nHaszn�lat: nmea2svg "
			"[-hv] [t=<time>] [s=<scale>] [-m[={track|alt|sats}]] [<host>:]<port> <outfile>\n\n"		
		BOLD_ON "\t-h\t " BOLD_OFF "S�g� megjelen�t�se\n"
		BOLD_ON "\t-v\t " BOLD_OFF "Hibakeres�si inform�ci� megjelen�t�se\n"
		BOLD_ON "\t-t\t " BOLD_OFF "Fut�si id� be�ll�t�sa <time> m�sodpercre\n"
		BOLD_ON "\t-s\t " BOLD_OFF "Az SVG-f�jl f�ldrajzi m�retar�ny�nak be�ll�t�sa 1:(<scale>*1000) -re\n"
		BOLD_ON "\t-m\t " BOLD_OFF "Megjelen�t�si m�d be�ll�t�sa:\n"
		BOLD_ON "\t\t\ttrack: " BOLD_OFF "Az megtett �tvonal kirajzol�sa;\n"
		BOLD_ON "\t\t\tsats: " BOLD_OFF "L�that� m�holdak kirajzol�sa.\n"
		BOLD_ON "\thost\t " BOLD_OFF "A kiszolg�l� neve\n"
		BOLD_ON "\tport\t " BOLD_OFF "A kiszolg�l�n�vhez tartoz� portsz�m\n"
		BOLD_ON "\toutfile\t " BOLD_OFF "A kimeneti f�jl neve\n\n"
	);
	
	exit(-1);
}
