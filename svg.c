#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "svg.h"

/**
 * SVG-fejlécet és gyökérelemet tartalmazó XML-dokumentum létrehozása
 * @return A kész dokumentumra mutató pointer
 */
xmlDocPtr create_svg_doc()
{
	char s[256];
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "svg");
	xmlDtdPtr dtd = xmlCreateIntSubset
	(
		doc,
		BAD_CAST "svg",
		BAD_CAST "-//W3C//DTD SVG 1.1//EN",
		BAD_CAST "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd"
	);
	
    xmlDocSetRootElement(doc, root);

	xaprintf(root,	"xmlns",	"%s",	"http://www.w3.org/2000/svg");
	xaprintf(root,	"version",	"%s",	"1.0");

	return doc;
}

/**
 * "Téglalap" SVG-alakzat létrehozása
 * @param x A téglalap x-koordinátája
 * @param y A téglalap y-koordinátája
 * @param w A téglalap szélessége
 * @param h A téglalap magassága
 * @param s A téglalap CSS-stílusa
 * @return A kész XML-csomópontra mutató pointer
 */
xmlNodePtr create_rect(double x, double y, double w, double h, const char *s)
{
	xmlNodePtr rect = xmlNewNode(NULL, BAD_CAST "rect");
	
	xaprintf(rect,	"x",		"%lfpx",	x);
	xaprintf(rect,	"y",		"%lfpx",	y);
	xaprintf(rect,	"width",	"%lfpx",	w);
	xaprintf(rect,	"height",	"%lfpx",	h);
	xaprintf(rect,	"style",	"%s",		s);

	return rect;
}

/**
 * "Kör" SVG-alakzat létrehozása
 * @param x A kör középpontjának x-koordinátája
 * @param y A kör középpontjának y-koordinátája
 * @param r A kör sugara
 * @param s A kör CSS-stílusa
 * @return A kész XML-csomópontra mutató pointer
 */
xmlNodePtr create_circle(double x, double y, double r, const char *s)
{
	xmlNodePtr crcl = xmlNewNode(NULL, BAD_CAST "circle");

	xaprintf(crcl,	"cx",		"%lfpx",	x);
	xaprintf(crcl,	"cy",		"%lfpx",	y);
	xaprintf(crcl,	"r",		"%lfpx",	r);
	xaprintf(crcl,	"style",	"%s",		s);

	return crcl;
}

/**
 * "Vonal" SVG-alakzat létrehozása
 * @param x1 A vonal kezdetének x-koordinátája
 * @param y1 A vonal kezdetének y-koordinátája
 * @param x2 A vonal végének x-koordinátája
 * @param y2 A vonal végének y-koordinátája
 * @param s A vonal CSS-stílusa
 * @return A kész XML-csomópontra mutató pointer
 */
xmlNodePtr create_line(double x1, double y1, double x2, double y2, const char *s)
{
	xmlNodePtr line = xmlNewNode(NULL, BAD_CAST "line");
	
	xaprintf(line,	"x1",		"%lfpx",	x1);
	xaprintf(line,	"x2",		"%lfpx",	x2);
	xaprintf(line,	"y1",		"%lfpx",	y1);
	xaprintf(line,	"y2",		"%lfpx",	y2);
	xaprintf(line,	"style",	"%s",		s);

	return line;
}

/**
 * Más alakzatokat magába foglaló "csoport" SVG-objektum létrehozása
 * @param id A csoport azonosítója
 * @return A kész XML-csomópontra mutató pointer
 */
xmlNodePtr create_g(const char *id)
{
	xmlNodePtr g = xmlNewNode(NULL, BAD_CAST "g");
	
	xaprintf(g,		"id",		"%s",	id);

	return g;
}

/**
 * "Útvonal" SVG-alakzat létrehozása
 * @param id Az elem azonosítója
 * @param data Az útvonal pontjainak adatai
 * @return A kész XML-csomópontra mutató pointer
 */
xmlNodePtr create_path(const char *id, const char *data)
{
	xmlNodePtr path = xmlNewNode(NULL, BAD_CAST "path");
	
	xaprintf(path,	"id",		"%s",	id);
	xaprintf(path,	"d",		"%s",	data);

	return path;
}

/**
 * SVG szöveg létrehozása
 * @param x A szöveg x-koordinátája
 * @param y A szöveg y-koordinátája
 * @param s A szöveg mérete
 * @param t A szöveg tartalma
 * @return A kész XML-csomópontra mutató pointer
 */
xmlNodePtr create_text(double x, double y, double s, const char *t)
{
	xmlNodePtr text = xmlNewNode(NULL, BAD_CAST "text");

	xaprintf(text,	"x",		"%lfpx",	x);
	xaprintf(text,	"y",		"%lfpx",	y);
	xaprintf(text,	"font-size","%lfpx",	s);

	xmlNodeSetContent(text, BAD_CAST t);

	return text;
}

/**
 * XML-elem tulajdonságának beállítása printf-szerûen
 * @param n Az XML-elem
 * @param s A tulajdonság neve
 * @param f A tulajdonság értéke printf-formátumsztringként
 * @param ... A formátumsztring paraméterei
 */
void xaprintf(xmlNodePtr n, xmlChar *s, xmlChar *f, ...)
{
	va_list argp;
	
	char t[8096];

	va_start(argp, f);
	
	vsprintf(t, f, argp);
	
	xmlSetProp(n, BAD_CAST s, BAD_CAST t);
	
	va_end(argp);
}
