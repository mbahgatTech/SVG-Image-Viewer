/*
Authored by: Mazen Bahgat (1157821)
Last Revision Date: January 26th 2022
*/

#include <string.h>
#include <ctype.h>

#include <math.h>
#define PI 3.141592653589793238

#include "SVGParser.h"

char *findTitle (xmlNode *node);

char *findDesc (xmlNode *node);

char *floatToString(float number);

float getUnits(char *units, char *string);

List *createAttributeList(xmlNode *node, char **coreAttributes, int length);

List *createRectangleList(xmlNode *img);

List *createCircleList(xmlNode *img);

List *createPathList(xmlNode *img);

List *createGroupList(xmlNode *node);

void addGroupCircles(List *groups, List *circles);

void addGroupRects(List *groups, List *rects);

void addGroupPaths(List *groups, List *paths);

void addInnerGroups(List *groups, List *allGroups);

void dummyDel(void *data);

bool validSVG(xmlDoc *img, const char *xsdFile);

xmlDoc *SVGToDoc(const SVG *img);

bool createRectNodes(xmlNode *svgNode, List *rectangles);

bool createCircleNodes(xmlNode *svgNode, List *circles);

bool createPathNodes(xmlNode *svgNode, List *paths);

bool createGroupNodes(xmlNode *svgNode, List *groups);

void createProps(xmlNode *node, List *attributes);

bool editAttributes(List *otherAttributes, Attribute *newAttribute);
