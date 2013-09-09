#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "svg.h"

/**
 * SVG-fejl�cet �s gy�k�relemet tartalmaz� XML-dokumentum l�trehoz�sa
 * @return A k�sz dokumentumra mutat� pointer
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
 * "T�glalap" SVG-alakzat l�trehoz�sa
 * @param x A t�glalap x-koordin�t�ja
 * @param y A t�glalap y-koordin�t�ja
 * @param w A t�glalap sz�less�ge
 * @param h A t�glalap magass�ga
 * @param s A t�glalap CSS-st�lusa
 * @return A k�sz XML-csom�pontra mutat� pointer
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
 * "K�r" SVG-alakzat l�trehoz�sa
 * @param x A k�r k�z�ppontj�nak x-koordin�t�ja
 * @param y A k�r k�z�ppontj�nak y-koordin�t�ja
 * @param r A k�r sugara
 * @param s A k�r CSS-st�lusa
 * @return A k�sz XML-csom�pontra mutat� pointer
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
 * "Vonal" SVG-alakzat l�trehoz�sa
 * @param x1 A vonal kezdet�nek x-koordin�t�ja
 * @param y1 A vonal kezdet�nek y-koordin�t�ja
 * @param x2 A vonal v�g�nek x-koordin�t�ja
 * @param y2 A vonal v�g�nek y-koordin�t�ja
 * @param s A vonal CSS-st�lusa
 * @return A k�sz XML-csom�pontra mutat� pointer
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
 * M�s alakzatokat mag�ba foglal� "csoport" SVG-objektum l�trehoz�sa
 * @param id A csoport azonos�t�ja
 * @return A k�sz XML-csom�pontra mutat� pointer
 */
xmlNodePtr create_g(const char *id)
{
	xmlNodePtr g = xmlNewNode(NULL, BAD_CAST "g");
	
	xaprintf(g,		"id",		"%s",	id);

	return g;
}

/**
 * "�tvonal" SVG-alakzat l�trehoz�sa
 * @param id Az elem azonos�t�ja
 * @param data Az �tvonal pontjainak adatai
 * @return A k�sz XML-csom�pontra mutat� pointer
 */
xmlNodePtr create_path(const char *id, const char *data)
{
	xmlNodePtr path = xmlNewNode(NULL, BAD_CAST "path");
	
	xaprintf(path,	"id",		"%s",	id);
	xaprintf(path,	"d",		"%s",	data);

	return path;
}

/**
 * SVG sz�veg l�trehoz�sa
 * @param x A sz�veg x-koordin�t�ja
 * @param y A sz�veg y-koordin�t�ja
 * @param s A sz�veg m�rete
 * @param t A sz�veg tartalma
 * @return A k�sz XML-csom�pontra mutat� pointer
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
 * XML-elem tulajdons�g�nak be�ll�t�sa printf-szer�en
 * @param n Az XML-elem
 * @param s A tulajdons�g neve
 * @param f A tulajdons�g �rt�ke printf-form�tumsztringk�nt
 * @param ... A form�tumsztring param�terei
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
