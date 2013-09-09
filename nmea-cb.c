#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <math.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "misc.h"
#include "nmea.h"
#include "nmea-cb.h"
#include "svg.h"

extern OPTIONS o;

static TRACK_INFO tri =
{
	{0, 0},	 // Center
	{0, 0},  // Min
	{0, 0},  // Max
	{0, 0},  // Last
	NULL,	 // Data
	NULL	 // First free char in data
};

static SAT_INFO *sti = NULL,
				*sti_curr = NULL;

/**
 * Az NMEA-üzenetek feldolgozásáért felelõs függvény ("track" mód)
 * @param t A beérkezett üzenetet küldõ eszköz típusa
 * @param s A beérkezett üzenet típusa
 * @param args Az üzenet paraméterei
 */
void nmea_cb_track(NMEA_TALKER_TYPE t, NMEA_SENTENCE_TYPE s, char **args)
{
// 1. Convert the NMEA-formatted coordinates to a less pointless format :@
	double nx_int = (int) (atof(args[3]) / 100);		
	double ny_int = (int) (atof(args[5]) / 100);		

	// This is our current location
	POINT n =
	{
		nx_int + ((atof(args[3]) - (nx_int * 100)) / 60),
		ny_int + ((atof(args[5]) - (ny_int * 100)) / 60)
	};

	if(!strcmp(args[4], "S") == 0)
		n.y = -n.y;
	if(!strcmp(args[6], "W") == 0)
		n.x = -n.x;

// 2. Cancel if we didn't move
	if(((tri.last.x - n.x == 0) && (tri.last.y - n.y == 0)))
		return;

// 3. Record the track

	// If we just started recording - allocate memory
	if(tri.path_data == NULL)
	{
		tri.pdc = tri.path_data = malloc(tri.l = 200);
		memset(tri.path_data, '\0', tri.l);

	// Start the path
		tri.pdc += sprintf(tri.pdc, "M %lf, %lf ", n.x, n.y);

		tri.center = tri.last = tri.min = tri.max = n;
	}
	else
	{
	// If we are to run out of memory - allocate some more		
		if((tri.pdc - tri.path_data) < 60)
		{
			ptrdiff_t diff = tri.pdc - tri.path_data;
			tri.path_data = realloc(tri.path_data, tri.l += 100);
			tri.pdc = tri.path_data + diff;
		}

	// Append the current point to the path
		tri.pdc += sprintf(tri.pdc, "L %lf,%lf ", n.x, n.y);
		tri.last = n;

	// Change max/min if needed
		if(n.x < tri.min.x) tri.min.x = n.x;
		if(n.x > tri.max.x) tri.max.x = n.x;
		if(n.y < tri.min.y) tri.min.y = n.y;
		if(n.y > tri.max.y) tri.max.y = n.y;
	}
}

/**
 * Az SVG-ábra létrehozásáért felelõs függvény ("track" mód)
 * @param f A megnyitott kimeneti fájlra mutató leíró
 */
void nmea_fn_track(FILE *f)
{
// Compute scaling info

	double im_w = ((tri.max.x - tri.min.x) * 111.0) / (o.im_scale / 1000) * 92;
	double im_h = ((tri.max.y - tri.min.y) * 111.0) / (o.im_scale / 1000) * 92;

	double scale_x = im_w / (tri.max.x - tri.min.x);
	double scale_y = im_h / (tri.max.y - tri.min.y);

	verbose_msg("SVG Width in cm's: %lf; SVG height in cm's: %lf", im_w, im_h);


/*	double scale_def_x = ((double) o.image_width) / (tri.max.x - tri.min.x);
	double scale_def_y = ((double) o.image_height) / (tri.max.y - tri.min.y);

	double scale_x = (o.fill_type != FILL_Y) ? scale_def_x : scale_def_y;
	double scale_y = (o.fill_type != FILL_X) ? scale_def_y : scale_def_x;
*/

// Compute translation info
	double tr_x = (-tri.min.x * 111.0) / (o.im_scale / 1000);
	double tr_y = (-tri.min.y * 111.0) / (o.im_scale / 1000);

// Start creating the SVG doc
	xmlDocPtr doc = create_svg_doc();
	xmlNodePtr root = xmlDocGetRootElement(doc);
	xaprintf(root,	"width",	"%lf",				im_w);
	xaprintf(root,	"height",	"%lf",				im_h);
	xaprintf(root,	"viewBox",	"%lf %lf %lf %lf",
		tri.min.x, tri.min.y, tri.max.x - tri.min.x, tri.max.y - tri.min.y);

	// The background pattern
	xmlNodePtr defs = xmlNewNode(NULL, BAD_CAST "defs");
	xmlNodePtr ptrn = xmlNewNode(NULL, BAD_CAST "pattern");
	xaprintf(ptrn, "id", "RectPattern");
	xaprintf(ptrn, "x", "0px");
	xaprintf(ptrn, "y", "0px");
	xaprintf(ptrn, "width",  "20px");
	xaprintf(ptrn, "height", "20px");
	xaprintf(ptrn, "patternUnits", "userSpaceOnUse");
	xaprintf(ptrn, "viewBox", "0 0 20 20");

	xmlAddChild(ptrn, create_rect(0, 0, 20, 20, "fill: #fff"));
	xmlAddChild(ptrn, create_rect(0, 0, 10, 10, "fill: #eee"));
	xmlAddChild(ptrn, create_rect(10, 10, 10, 10, "fill: #eee"));
	
	xmlAddChild(defs, ptrn);
	xmlAddChild(root, defs);

	// The background
	xmlNodePtr bg;
	xmlAddChild(root, bg = create_rect(0, 0, 0, 0, "fill: url(#RectPattern)"));
	xaprintf(bg, "width", "100%%");
	xaprintf(bg, "height", "100%%");

	// The data, itself
	xmlNodePtr path = create_path("track-data", tri.path_data);
	xaprintf(path, "style",
		"fill: none;"
		"stroke: #000;"
		"stroke-width: %lf;"
		"stroke-linejoin: round;",
		2 / scale_x
	);

	xmlAddChild(root, path);

	xmlNodePtr g_ui = create_g("lines");

	xmlDocDump(f, doc);
}

/**
 * Az NMEA-üzenetek feldolgozásáért felelõs függvény ("sats" mód)
 * @param t A beérkezett üzenetet küldõ eszköz típusa
 * @param s A beérkezett üzenet típusa
 * @param args Az üzenet paraméterei
 */

void nmea_cb_sats(NMEA_TALKER_TYPE t, NMEA_SENTENCE_TYPE s, char **args)
{
	// We store this so we can exit if no more messages are to follow
	static char message_count = -1;
	
	// We keep track of which field we are currently in
	unsigned short n = 0;

	if(message_count == -1)
		message_count = atoi(args[1]) - 1;
	else
		message_count--;

	// We skip the first four fields and go directly to the sat data
	for(args += 4; *args != NULL; args++)
	{
		SAT_INFO *temp;
		if(n % 4 == 0) // Sat. no
		{
			// This is the first field, so we create a new record
			if(sti_curr == NULL)
			{
				sti_curr = malloc(sizeof(SAT_INFO));
				sti_curr->n = *args;

				sti = sti_curr;					
			}
			else
			{
				sti_curr->next = malloc(sizeof(SAT_INFO));
				sti_curr->next->n = *args;
				sti_curr = sti_curr->next;
			}
		}
		else if(n % 4 == 1) // Elevation
		{
			sti_curr->elevation = atof(*args);
		}
		else if(n % 4 == 2) // Azimuth
		{
			sti_curr->azimuth = atof(*args);
		}
		// We don't care about the S/N ratio...

		n++;
	}

	if(message_count == 0)
		finalize();
}

/**
 * Az SVG-ábra létrehozásáért felelõs függvény ("sats" mód)
 * @param f A megnyitott kimeneti fájlra mutató leíró
 */
void nmea_fn_sats(FILE *f)
{
	double im_w = 500;
	double im_h = 500;
	
	double xcoeff = im_w * 0.525;
	double ycoeff = im_h * 0.525;

	SAT_INFO *s;

	xmlDocPtr doc = create_svg_doc();
	xmlNodePtr root = xmlDocGetRootElement(doc);

	verbose_msg("%lf %lf", im_w, im_h);

	xaprintf(root,	"width",	"%lf",	im_w);
	xaprintf(root,	"height",	"%lf",	im_h);

	xmlNodePtr defs = xmlNewNode(NULL, BAD_CAST "defs");
	xmlNodePtr fltr = xmlNewNode(NULL, BAD_CAST "filter");
	xmlNodePtr gblr = xmlNewNode(NULL, BAD_CAST "feGaussianBlur");
	
	xmlNodePtr ptrn = xmlNewNode(NULL, BAD_CAST "pattern");

	xaprintf(fltr,	"id",			"blur");
	xaprintf(fltr,	"x",			"-0.27483871");
    xaprintf(fltr,	"y",			"-0.27483871");
    xaprintf(fltr,	"width",		"25.3096774");
    xaprintf(fltr,	"height",		"25.3096774");
	xaprintf(gblr,	"stdDeviation",	"2");
	
	xaprintf(ptrn, "id", "RectPattern");
	xaprintf(ptrn, "x", "0");
	xaprintf(ptrn, "y", "0");
	xaprintf(ptrn, "width",  "20");
	xaprintf(ptrn, "height", "20");
	xaprintf(ptrn, "patternUnits", "userSpaceOnUse");
	xaprintf(ptrn, "viewBox", "0 0 20 20");

	xmlAddChild(ptrn, create_rect(0, 0, 20, 20, "fill: #fff"));
	xmlAddChild(ptrn, create_rect(0, 0, 10, 10, "fill: #eee"));
	xmlAddChild(ptrn, create_rect(10, 10, 10, 10, "fill: #eee"));

	xmlAddChild(fltr, gblr);
	xmlAddChild(defs, fltr);
	xmlAddChild(defs, ptrn);
	xmlAddChild(root, defs);

	xmlAddChild(root, create_rect(0, 0, im_w, im_h,
		"fill: url(#RectPattern); stroke: #000; stroke-width: 1px;"));

	xmlAddChild(root, create_circle(
		im_w / 2,
		im_h / 2,
		im_w / 2,
		"fill: #ccc; stroke: #aaa; stroke-width: 2px; opacity: .5"
	));
	xmlAddChild(root, create_circle(
		im_w / 2,
		im_h / 2,
		im_w / 2 * .66,
		"fill: #ccc; stroke: #aaa; stroke-width: 2px; opacity: .5"
	));
	xmlAddChild(root, create_circle(
		im_w / 2,
		im_h / 2,
		im_w / 2 * .33,
		"fill: #ccc; stroke: #aaa; stroke-width: 2px; opacity: .5"
	));

	xmlNodePtr cpath = xmlNewNode(NULL, BAD_CAST "clipPath");	
	xaprintf(cpath, "id", "ZenithClip");

	xmlAddChild(cpath, create_circle(
		im_w / 2,
		im_h / 2,
		im_w / 2,
		""
	));

	xmlAddChild(root, cpath);

	xmlAddChild(root, create_line(
		im_w / 2, 0, im_w / 2, im_h,
		"stroke: #444; stroke-width: .5px;"));

	xmlAddChild(root, create_line(
		0, im_h / 2, im_w, im_h / 2, 
		"stroke: #444; stroke-width: .5px;"));

	xmlNodePtr rc;

	xmlAddChild(root, rc = create_rect(
		im_w / 2 - 15, -5, 30, 30, "fill: #666;"));
	xaprintf(rc, "rx", "5"); xaprintf(rc, "ry", "5");
	
	xmlAddChild(root, rc = create_rect(
		im_w / 2 - 15, im_h - 25, 30, 30, "fill: #666;"));
	xaprintf(rc, "rx", "5"); xaprintf(rc, "ry", "5");
	
	xmlAddChild(root, rc = create_rect(
		-5, im_h / 2 - 15, 30, 30, "fill: #666;"));
	xaprintf(rc, "rx", "5"); xaprintf(rc, "ry", "5");
	
	xmlAddChild(root, rc = create_rect(
		im_w - 25, im_h / 2 - 15, 30, 30, "fill: #666;"));
	xaprintf(rc, "rx", "5"); xaprintf(rc, "ry", "5");

	xmlAddChild(root, rc = create_text(im_w / 2 - 8, 19, 18, "N"));
	xaprintf(rc, "style",	"font-weight: bold; fill: #fff;");
	xmlAddChild(root, rc = create_text(im_w / 2 - 7, im_h - 5, 18, "S"));
	xaprintf(rc, "style",	"font-weight: bold; fill: #fff;");
	xmlAddChild(root, rc = create_text(2, im_h / 2 + 7, 18, "W"));
	xaprintf(rc, "style",	"font-weight: bold; fill: #fff;");
	xmlAddChild(root, rc = create_text(im_w - 19, im_h / 2 + 7, 18, "E"));
	xaprintf(rc, "style",	"font-weight: bold; fill: #fff;");

	xmlNodePtr g = create_g("sats");
	xaprintf(g, "clip-path", "url(#ZenithClip)");
	
	xmlAddChild(root, g);
	
	for(s = sti; s != NULL; s = s->next)
	{
		xmlNodePtr text;

		if(s->elevation > 80) continue;

		verbose_msg("%s: Elev: %d, Azim: %d", s->n, s->elevation, s->azimuth);
		
		double x =
			(sin((s->azimuth + 90) * M_PI / 180) *
			((double) (s->elevation - 90) / 100)) *
			xcoeff + (im_w / 2);
			
		double y =
			(cos((s->azimuth + 90) * M_PI / 180) *
			((double) (s->elevation - 90) / 100)) * ycoeff +
			(im_h / 2);

		xmlAddChild(g, create_circle(x, y, 14,
			"fill: #000; filter: url(#blur); opacity: 0.541;"));

		xmlAddChild(g, create_circle(x, y, 14,
			"fill: #fff; stroke: #000; stroke-width: 1px;"));

		xmlAddChild(g, text = create_text(x - 9, y + 5, 13, s->n));
		xaprintf(text, "style", "font-weight: bold");
	}

	xmlDocDump(f, doc);
}
