#include <string.h>
#include <ctype.h>
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
