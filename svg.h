#ifndef __XML_H
#define __XML_H

xmlDocPtr create_svg_doc();
xmlNodePtr create_circle(double, double, double, const char *); 
xmlNodePtr create_g(const char *);
xmlNodePtr create_line(double, double, double, double, const char *);
xmlNodePtr create_path(const char *, const char *);
xmlNodePtr create_rect(double, double, double, double, const char *);
xmlNodePtr create_text(double, double, double, const char*);

void xaprintf(xmlNodePtr, xmlChar *, xmlChar *, ...);

#endif
