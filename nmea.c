#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/time.h>

#include "misc.h"
#include "nmea.h"
#include "nmea-cb.h"

NMEA_CALLBACK callbacks[2] =
{
	{ &nmea_cb_track, &nmea_fn_track, GP, RMC },
	{ &nmea_cb_sats,  &nmea_fn_sats,  GP, GSV }
};

extern OPTIONS o;

/**
 * A socket-bõl érkezõ NMEA-üzenetek értelmezéséért felelõs függvény
 */
void nmea_parse()
{
	char rxbuf[129];
	char sentence[84]; /* 80 + "$" + <CR><LF> + '\0' */
	char *sp = sentence;
	char *t, *rp;
	
	fd_set fds;
	struct timeval tv;
	
	FD_ZERO(&fds);
	FD_SET(o.sockfd, &fds);
	time_t endrun = time(NULL) + o.runtime;

	verbose_msg("%d", o.runtime);

	verbose_msg("nmea_parse_socket() - runtime: %lu secs", o.runtime);

	while(time(NULL) < endrun)
	{
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		
		select(o.sockfd + 1, &fds, NULL, NULL, &tv);
		
		if(FD_ISSET(o.sockfd, &fds))
		{		
			int status = recv(o.sockfd, rxbuf, 128, 0);
			
			if(status == -1)
			{
				verbose_msg("Sikertelen olvasás.");
			}
			else if(status == 0)
			{
				die("A kiszolgáló megszakította a kapcsolatot.");
			}
			else
			{
				rxbuf[status] = '\0';
				rp = rxbuf;
				
				/* Loop through end-of-sentence markers and parse sentences */
				while(NULL != (t = strstr(rp, "\r\n")))
				{
					/* Copy (portion of) the sentence into the temp. storage */
					memcpy(sp, rp, t - rp);
					sp[t - rp] = '\0';
					
					/* Parse the sentence and reset the pointer */
					if(*sentence == '$')
						nmea_parse_sentence
							(sentence, callbacks[o.display_mode]);
					memset(sentence, 0, 84);
					sp = sentence;
					
					rp = t + 2;
				}
				
				/* Store what remains */
				
				if(strlen(rp) > 0)
				{
					memcpy(sp, rp, strlen(rp));
					sp += strlen(rp);
				}
			}
		}
	}
}

/**
 * Az NMEA-üzenetsztringeket értelmezõ függvény
 * @param s Az üzenetet tartalmazó karakterlánc
 * @param c Az aktuális visszahívási függvényeket tartalmazó struktúra
 */
static void nmea_parse_sentence(char *s, NMEA_CALLBACK c)
{
// Cancel if the callback doesn't want the sentence
	if(!(
		(nmea_get_talker_type(s) & c.talker_type_flags) &&
		(nmea_get_sentence_type(s) & c.sentence_type_flags)
	)) return;

	verbose_msg("%s", s);

// Do the callback
	c.mc
	(
		nmea_get_talker_type(s),
		nmea_get_sentence_type(s),
		nmea_get_sentence_args(s)
	);	
}

/**
 * Az NMEA-üzenetek paramétereit értelmezõ függvény
 * @param m Az üzenetet tartalmazó sztring
 * @return Az üzenetek paraméterei
 */
static char **nmea_get_sentence_args(char *m)
{
	char **args, **targs;
	char *t, *a;
	int i = 0, cpty = 10, j;
	
	/* We allocate ten pointers and grow later as needed */
	args = (char **) malloc(sizeof(char *) * cpty);
	
	/* Then we strip the checksum after the '*' */
	*(strchr(m, '*')) = '\0';
	
	/* Ready to tokenize */
	t = strtok(m, ",");
	while(t != NULL)
	{
		/* Allocate some more space if needed */
		if(i == cpty - 1)
		{
			targs = (char **) malloc((i + 1 + cpty) * sizeof(char *));
			for(j = 0; j <= i; targs[j] = args[j], j++);
			free(args);
			args = targs;
		}
		
		a = malloc(strlen(t) + 1);
		strcpy(a, t);
		args[i++] = a;
		t = strtok(NULL, ",");
	}
	
	/* Create a sentinel */
	if(i == cpty - 1)
	{
		targs = (char **) malloc((i + 2) * sizeof(char *));
		for(j = 0; j <= i; targs[j] = args[j], j++);
		free(args);
		args = targs;
	}
	
	args[i++] = NULL;
	
	return args;
}

/**
 * Az NMEA-üzenet típusát meghatározó függvény
 * @param m Az üzenetet tartalmazó karakterlánc
 * @return Az üzenet típusa
 */
static NMEA_SENTENCE_TYPE nmea_get_sentence_type(char *m)
{
	if(!strncmp(m + 3, "GGA", 3))
		return GGA;
	if(!strncmp(m + 3, "RMC", 3))
		return RMC;
	if(!strncmp(m + 3, "GLL", 3))
		return GLL;
	if(!strncmp(m + 3, "HDG", 3))
		return HDG;
	if(!strncmp(m + 3, "GSV", 3))
		return GSV;
	if(!strncmp(m + 3, "RMZ", 3))
		return RMZ;
	
	return -1;
}

/**
 * Az NMEA-üzenetet küldõ eszköz típusát meghatározó függvény
 * @param m Az üzenetet tartalmazó karakterlánc
 * @return Az üzenetet küldõ eszköz típusa
 */
static NMEA_TALKER_TYPE nmea_get_talker_type(char *m)
{
	if(!strncmp(m + 1, "GP", 2))
		return GP;
	if(!strncmp(m + 1, "HC", 2))
		return HC;
	if(!strncmp(m + 1, "PG", 2))
		return PG;
	
	return -1;
}
