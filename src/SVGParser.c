#include "SVGHelpers.h"
#include "SVGParser.h"

SVG* createSVG(const char* fileName) {
    LIBXML_TEST_VERSION

    if (fileName == NULL) {
        return NULL;
    }

    xmlDoc *svgImg = xmlReadFile(fileName, NULL, 0);
    if (svgImg == NULL) {
        return NULL;
    }
    
    xmlNode *svgNode = xmlDocGetRootElement(svgImg);
    SVG *tempSVG = malloc(sizeof(SVG));
    if (svgNode == NULL || tempSVG == NULL) {
        return NULL;
    }

    // copy only the first 255 chars or length of the first namespace to fit into
    // our SVG namespace element. Note: only 1 namesapce is assumed in this program
    strncpy(tempSVG -> namespace, (char *)svgNode -> nsDef -> href, 255);


    char *tempPtr = findTitle(svgNode);
    // copy first 255 or length chars of title
    if (tempPtr == NULL) {
        return NULL;
    }
    strncpy(tempSVG -> title, tempPtr, 255);
    
    tempPtr = findDesc(svgNode);
    // copy first 255 or length chars of desc
    if (tempPtr == NULL) {
        return NULL;
    }
    strncpy(tempSVG -> description, tempPtr, 255);

    // initialize lists with NULL for now
    tempSVG -> rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    tempSVG -> circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    tempSVG -> paths = initializeList(&pathToString, &deletePath, &comparePaths);
    tempSVG -> groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    tempSVG -> otherAttributes = createAttributeList(svgNode, NULL, 0);

    // free svgImg object
    xmlFreeDoc(svgImg);
    
    // free any global variables that may have been used by the parser
    xmlCleanupParser();
    return tempSVG;
}

char* SVGToString(const SVG* img) {
    // enough initial space for namespace, title and desc seperated by new lines  
    char *svgString = malloc(sizeof(char) * (
        strlen(img -> namespace) +
        strlen(img -> title) + 
        strlen(img -> description) + 4)
        );
    
    // copy SVG data elements
    strcpy(svgString, img -> namespace);
    strcat(svgString, "\n");
    strcat(svgString, img -> title);
    strcat(svgString, "\n");
    strcat(svgString, img -> description);
    strcat(svgString, "\n");

    // copy SVG lists

    return svgString;
}

void deleteSVG(SVG* img) {
    // free lists 
    freeList(img -> rectangles);
    freeList(img -> circles); 
    freeList(img -> paths);
    freeList(img -> groups);
    freeList(img -> otherAttributes);

    free(img);
}

List* getRects(const SVG* img) {
    return NULL;
}

List* getCircles(const SVG* img) {
    return NULL;
}

List* getGroups(const SVG* img) {
    return NULL;
}

List* getPaths(const SVG* img) {
    return NULL;
}

int numRectsWithArea(const SVG* img, float area) {
    return 0;
}

int numCirclesWithArea(const SVG* img, float area) {
    return 0;
}

int numPathsWithdata(const SVG* img, const char* data) {
    return 0;
}

int numGroupsWithLen(const SVG* img, int len) {
    return 0;
}

int numAttr(const SVG* img) {
    return 0;
}

void deleteAttribute( void* data) {
    free(((Attribute *) data) -> name);
    
    // frees the attribute and the flexible array element (value)
    free(data);
}

char* attributeToString(void* data) {
    Attribute *tempAttr = (Attribute *) data;

    // check for invalid attribute
    if (tempAttr == NULL || tempAttr -> name == NULL) {
        return NULL;
    }

    // enough space for name and value followed by new lines
    char *tempString = malloc(sizeof(char) * (strlen(tempAttr -> name) + 2));

    // return null if malloc failed
    if (tempString == NULL) {
        return NULL;
    }
    // concatenate name and value followed by new lines to string and return it
    strcpy(tempString, tempAttr -> name);
    strcat(tempString, "\n");

    // if value is NULl just return the string with the name
    if (tempAttr -> value == NULL || strcmp(tempAttr -> value, "") == 0) {
        return tempString;
    }
    
    // otherwise reallocate memory for the tempString to fit in the value
    tempString = realloc(tempString, sizeof(char) * 
        (strlen(tempAttr -> name)  + strlen(tempAttr -> value) + 3));    

    strcat(tempString, tempAttr -> value);
    strcat(tempString, "\n");

    return tempString;
}

// returns 0 if equal and 1 otherwise (follows strcmp)
int compareAttributes(const void *first, const void *second){
    // 1 of the pointers doesnt point to a valid attribute
    if (((Attribute *)first) == NULL || ((Attribute *)second) == NULL || 
        ((Attribute *)first) -> name == NULL || ((Attribute *)second) -> name == NULL) {
        return 1;
    }
    
    // since value can be NULL, we check only for name equality
    if (((Attribute *)first) -> value == NULL || ((Attribute *)second) -> value == NULL) {
        // branch means that 1 of the values is NULL and the other isnt (not equal)
        if (((Attribute *)first) -> value != NULL || ((Attribute *)second) -> value != NULL) {
            return 1;
        }

        return strcmp(((Attribute *)first) -> name, ((Attribute *)second) -> name);
    }
    
    if (strcmp(((Attribute *)first) -> name, ((Attribute *)second) -> name) == 0 && 
        strcmp(((Attribute *)first) -> value, ((Attribute *)second) -> value) == 0) {
        // both equal
        return 0;
    } 
    
    // not equal
    return 1;
}  


void deleteGroup(void* data){
    
}

char* groupToString(void* data){
    return NULL;
}

int compareGroups(const void *first, const void *second){
    return 0;    
}

void deleteRectangle(void* data){

}

char* rectangleToString(void* data){
    return NULL;
}

int compareRectangles(const void *first, const void *second){
    return strcmp(rectangleToString((Rectangle *)first), rectangleToString((Rectangle *)second));
}

void deleteCircle(void* data){
    Circle *circle = (Circle *)data;
    void *elem = NULL;
    ListIterator iter;

    // initialize an iterator over the circle's otherAttributes
    if (circle -> otherAttributes != NULL) {
        iter = createIterator(circle -> otherAttributes);
        elem = iter.current;
    }
    
    // iterate over the list and delete the attribute
    while (elem != NULL) {
        deleteAttribute(elem);
        elem = nextElement(&iter);
    }

    // free list and circle structure
    free(circle -> otherAttributes);
    free(circle);
}

char* circleToString(void* data){
    // check if circle ptr is null
    if (data == NULL) {
        return NULL;
    }
    
    //easier access to data's elements without having to cast to Circle * every time
    Circle *temp = (Circle *)data;
    char *tempString = floatToString(temp -> cx);
    char *circleString = malloc(sizeof(char) * (strlen(tempString) + 2));

    // concatenate circle elements to the string
    strcpy(circleString, tempString);
    strcat(circleString, "\n");
    free(tempString); 
    
    tempString = floatToString(temp -> cy);
    // reallocate memory for circleString and tempString plus new line after.
    circleString = realloc(circleString, sizeof(char) * (strlen(circleString) + strlen(tempString) + 2));
    strcat(circleString, tempString);
    strcat(circleString, "\n");
    free(tempString);

    tempString = floatToString(temp -> r);
    circleString = realloc(circleString, sizeof(char) * (strlen(circleString) + strlen(tempString) + 2));
    strcat(circleString, tempString);
    strcat(circleString, "\n");
    free(tempString); // free memory from floatToString call
    
    circleString = realloc(circleString, sizeof(char) * (strlen(circleString) + strlen(temp -> units) + 2));
    strcat(circleString, temp -> units);
    strcat(circleString, "\n");

    // concatenate list of attributes' strings to the circleString
    void * elem;
    ListIterator iter;

    // in case otherAttributes is NULL (if SVG created properly it shouldn't) 
    //  return the string with cx, cy, r and units
    if (temp -> otherAttributes == NULL) {
        return circleString;
    }
    
    // iterator through list of attributes
    iter = createIterator(temp -> otherAttributes);
    elem = iter.current;
    
    // get attribute string and  reallocate circleString to fit concatenation
    while(elem != NULL) {
        tempString = attributeToString(elem);
        circleString = realloc(circleString, sizeof(char) * (strlen(circleString) + strlen(tempString) + 1));
        strcat(circleString, tempString);
        free(tempString);

        elem = nextElement(&iter);
    }

    return circleString;
}

// returns 0 if both circles are equal, non-zero otherwise (follows strcmp convention)
int compareCircles(const void *first, const void *second){
    return strcmp(circleToString((Circle *)first), circleToString((Circle *)second));
}

void deletePath(void* data){
    
}

char* pathToString(void* data){
    return NULL;
}

// return 1 if not equal, 0 if equal
int comparePaths(const void *first, const void *second){
    return strcmp(pathToString((Path *)first), pathToString((Path *)second));
}
