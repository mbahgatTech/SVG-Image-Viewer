#include <string.h>
#include "SVGParser.h"

char *findTitle (xmlNode *node);

char *findDesc (xmlNode *node);

char *floatToString(float number);

List *createAttributeList(xmlNode *node, char **coreAttributes, int length);

List *createRectangleList(xmlNode *img);

List *createCircleList(xmlNode *img);

List *createPathList(xmlNode *img);
