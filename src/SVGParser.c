/*
Authored by: Mazen Bahgat (1157821)
Last Revision Date: January 26th 2022
*/


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
    if (svgNode == NULL) {
        return NULL;
    }
    
    if (tempSVG == NULL || svgNode == NULL) {
        xmlFreeDoc(svgImg);
        xmlCleanupParser();
        return NULL;
    }
    
    // initialize string with empty values
    strcpy(tempSVG -> namespace, "");
    strcpy(tempSVG -> title, "");
    strcpy(tempSVG -> description, "");

    // copy only the first 255 chars or length of the first namespace to fit into
    // our SVG namespace element. Note: only 1 namesapce is assumed in this program
    strncpy(tempSVG -> namespace, (char *)svgNode -> nsDef -> href, 255);


    char *tempPtr = findTitle(svgNode);
    // copy first 255 or length chars of title
    if (tempPtr != NULL) {
        strncpy(tempSVG -> title, tempPtr, 255);
    }
    
    tempPtr = findDesc(svgNode);
    // copy first 255 or length chars of desc
    if (tempPtr != NULL) {
        strncpy(tempSVG -> description, tempPtr, 255);
    }

    // initialize lists with NULL for now
    tempSVG -> rectangles = createRectangleList(svgNode);
    tempSVG -> circles = createCircleList(svgNode);
    tempSVG -> paths = createPathList(svgNode);
    tempSVG -> groups = createGroupList(svgNode);
    tempSVG -> otherAttributes = createAttributeList(svgNode, NULL, 0);

    // free svgImg object
    xmlFreeDoc(svgImg);
    
    // free any global variables that may have been used by the parser
    xmlCleanupParser();
    return tempSVG;
}

char* SVGToString(const SVG* img) {
    if (img == NULL) {
        return NULL;
    }

    // enough initial space for namespace, title and desc seperated by new lines  
    char *svgString = malloc(sizeof(char) * (strlen("Namespace: ") + strlen(img -> namespace) + 2));
    
    // copy SVG data elements
    strcpy(svgString, "Namespace: ");
    strcat(svgString, img -> namespace);
    strcat(svgString, "\n");

    svgString = realloc(svgString, sizeof(char) * (
        strlen(svgString) + strlen("Title: ") + strlen(img -> title) + 2)
    );
    strcat(svgString, "Title: ");
    strcat(svgString, img -> title);
    strcat(svgString, "\n");

    svgString = realloc(svgString, sizeof(char) * (
        strlen(svgString) + strlen("Description: ") + strlen(img -> description) + 2)
    );
    strcat(svgString, "Description: ");
    strcat(svgString, img -> description);
    strcat(svgString, "\n");

    // copy SVG lists
    char *temp;
    if(img -> rectangles) {
        temp = toString(img -> rectangles);
        svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
        strcat(svgString, temp);
        free(temp);
    }

    if(img -> circles) {
        temp = toString(img -> circles);
        svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
        strcat(svgString, temp);
        free(temp);
    }

    if(img -> paths) {
        temp = toString(img -> paths);
        svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
        strcat(svgString, temp);
        free(temp);
    }

    if (img -> groups) {
        temp = toString(img -> groups);
        svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
        strcat(svgString, temp);
        free(temp);
    }

    if (img -> otherAttributes) {
        temp = toString(img -> otherAttributes);
        svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
        strcat(svgString, temp);
        free(temp);
    }
    
    return svgString;
}

void deleteSVG(SVG* img) {
    if (img == NULL) {
        return;
    }

    // free lists 
    freeList(img -> rectangles);
    freeList(img -> circles); 
    freeList(img -> paths);
    freeList(img -> groups);
    freeList(img -> otherAttributes);

    free(img);
}

List* getRects(const SVG* img) {
    // cant get rectangles of a NULL image
    if (img == NULL) {
        return NULL;
    }
    
    // initialize a list, return it if rectangles list is null
    List *rectList = initializeList(&rectangleToString, &dummyDel, &compareRectangles);
    if (img -> rectangles == NULL || img -> rectangles -> length == 0) {
        addGroupRects(img -> groups, rectList);
        return rectList;
    }

    ListIterator iter = createIterator(img -> rectangles);
    void *data;

    // whatever what is inserted first in rectangles list 
    // will be inserted first in our new list
    while((data = nextElement(&iter)) != NULL) {
        insertBack(rectList, data);
    }
    
    // add rectangles in groups to rectList
    addGroupRects(img -> groups, rectList);
    
    return rectList;
}

List* getCircles(const SVG* img) {
    // cant get circles of a NULL image
    if (img == NULL) {
        return NULL;
    }
    
    // initialize a list, return it if circles list is null
    List *circleList = initializeList(&circleToString, &dummyDel, &compareCircles);
    if (img -> circles == NULL || img -> circles -> length == 0) {
        addGroupCircles(img -> groups, circleList);
        return circleList;
    }

    ListIterator iter = createIterator(img -> circles);
    void *data;

    // whatever what is inserted first in circles list 
    // will be inserted first in our new list
    while((data = nextElement(&iter)) != NULL) {
        insertBack(circleList, data);
    }
    
    // add circles in groups to circleList
    addGroupCircles(img -> groups, circleList);
    
    return circleList;
}

List* getGroups(const SVG* img) {
    // cant get circles of a NULL image
    if (img == NULL) {
        return NULL;
    }
    
    // all groups list
    List *groupList = initializeList(&groupToString, &dummyDel, &compareGroups);
    addInnerGroups(img -> groups, groupList);

    return groupList;
}

List* getPaths(const SVG* img) {
    // cant get paths of a NULL image
    if (img == NULL) {
        return NULL;
    }
    
    // initialize a list, return it if paths list is null
    List *pathList = initializeList(&pathToString, &dummyDel, &comparePaths);
    if (img -> paths == NULL || img -> paths -> length == 0) {
        addGroupPaths(img -> groups, pathList); // paths in groups added too
        return pathList;
    }

    ListIterator iter = createIterator(img -> paths);
    void *data;

    // whatever what is inserted first in paths list 
    // will be inserted first in our new list
    while((data = nextElement(&iter)) != NULL) {
        insertBack(pathList, data);
    }
    
    // add paths in groups to pathList
    addGroupPaths(img -> groups, pathList);
    
    return pathList;
}

int numRectsWithArea(const SVG* img, float area) {
    if (img == NULL) {
        return 0;
    }

    List *rects = getRects(img); //get all rects in the img
    
    void *data;
    ListIterator iter = createIterator(rects);
    float currArea = 0;
    int numRects = 0;
    
    // iterate through all rectangles in the list and count one with matching area
    while((data = nextElement(&iter)) != NULL) {
        // get area of current rectangle
        currArea = ((Rectangle *)data) -> width * ((Rectangle *)data) -> height;
        
        // increment num of matching rectangles when ceiling of areas match
        if (ceil(area) == ceil(currArea)) {
            numRects++;
        }

        currArea = 0; //reset currarea for next rectangle
    }
    
    // free rects list without freeing pointers to rects
    freeList(rects);
    return numRects;
}

int numCirclesWithArea(const SVG* img, float area) {
    if (img == NULL) {
        return 0;
    }
    
    List *circles = getCircles(img); //get all circles in the img
    
    void *data;
    ListIterator iter = createIterator(circles);
    float currArea = 0;
    int numCircles = 0;
    
    // iterate through all circles in the list and count one with matching area
    while((data = nextElement(&iter)) != NULL) {
        // get area of current circle
        currArea = PI * (((Circle *)data) -> r * ((Circle *)data) -> r);
        
        // increment num of matching circles when ceiling of areas match
        if (ceil(area) == ceil(currArea)) {
            numCircles++;
        }

        currArea = 0; //reset currarea for next circle
    }
    
    // free circles list without freeing pointers to circles
    freeList(circles);
    return numCircles;
}

int numPathsWithdata(const SVG* img, const char* data) {
    if (img == NULL) {
        return 0;
    }
    
    List *paths = getPaths(img); //get all paths in the img
    
    void *currPath;
    ListIterator iter = createIterator(paths);
    int numPaths = 0;
    
    // iterate through all paths in the list and count one with matching area
    while((currPath = nextElement(&iter)) != NULL) {        
        // increment num of matching paths when data string matches target data 
        if (strcmp(((Path *)currPath) -> data, data) == 0) {
            numPaths++;
        }
    }
    
    // free paths list without freeing pointers to paths
    freeList(paths);
    
    return numPaths;
}

int numGroupsWithLen(const SVG* img, int len) {
    if (img == NULL) {
        return 0;
    }
    
    List *groups = getGroups(img); //get all groups in the img
    
    void *data;
    ListIterator iter = createIterator(groups);
    int currLen = 0;
    int numGroups = 0;
    
    // iterate through all groups in the list and count one with matching length
    while((data = nextElement(&iter)) != NULL) {
        // get length of current group
        currLen = ((Group *)data) -> rectangles -> length + 
        ((Group *)data) -> circles -> length + ((Group *)data) -> paths -> length
        + ((Group *)data) -> groups -> length;
        
        // increment num of matching groups when ceiling of areas match
        if (currLen == len) {
            numGroups++;
        }

        currLen = 0; //reset currarea for next group
    }
    
    // free groups list without freeing pointers to groups
    freeList(groups);
    
    return numGroups;
}

int numAttr(const SVG* img) {
    if (img == NULL) {
        return 0;
    }

    int numAttrs = 0; // number of attributes in svg struct
    ListIterator iter;
    void *data;

    // get all lists and count their attributes
    List *lists = getRects(img);
    iter = createIterator(lists);

    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Rectangle *)data) -> otherAttributes -> length;
    }
    freeList(lists);

    lists = getCircles(img);
    iter = createIterator(lists);
    // iterate through circles list of img and add the num of attrs to numAttrs
    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Circle *)data) -> otherAttributes -> length;
    }
    freeList(lists);

    lists = getPaths(img);
    iter = createIterator(lists);
    // iterate through paths list of img and add the num of attrs to numAttrs
    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Path *)data) -> otherAttributes -> length;
    }
    freeList(lists);
    
    lists = getGroups(img);
    iter = createIterator(lists);
    // iterate through groups list of img and add the num of attrs to numAttrs
    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Group *)data) -> otherAttributes -> length;
    }
    freeList(lists);

    // add number of attributes in the SVG struct itself
    numAttrs += img -> otherAttributes -> length;

    return numAttrs; 
}

SVG* createValidSVG(const char* fileName, const char* schemaFile) {
     LIBXML_TEST_VERSION

    if (fileName == NULL || schemaFile == NULL) {
        return NULL;
    }

    xmlDoc *svgImg = xmlReadFile(fileName, NULL, 0);
    if (svgImg == NULL) {
        return NULL;
    }

    // validate xmlDoc against schemaFile
    if (!validSVG(svgImg, schemaFile)) {
        return NULL;
    }
    
    xmlNode *svgNode = xmlDocGetRootElement(svgImg);
    SVG *tempSVG = malloc(sizeof(SVG));
    if (svgNode == NULL) {
        return NULL;
    }
    
    if (tempSVG == NULL || svgNode == NULL) {
        xmlFreeDoc(svgImg);
        xmlCleanupParser();
        return NULL;
    }
    
    // initialize string with empty values
    strcpy(tempSVG -> namespace, "");
    strcpy(tempSVG -> title, "");
    strcpy(tempSVG -> description, "");

    // copy only the first 255 chars or length of the first namespace to fit into
    // our SVG namespace element. Note: only 1 namesapce is assumed in this program
    strncpy(tempSVG -> namespace, (char *)svgNode -> nsDef -> href, 255);


    char *tempPtr = findTitle(svgNode);
    // copy first 255 or length chars of title
    if (tempPtr != NULL) {
        strncpy(tempSVG -> title, tempPtr, 255);
    }
    
    tempPtr = findDesc(svgNode);
    // copy first 255 or length chars of desc
    if (tempPtr != NULL) {
        strncpy(tempSVG -> description, tempPtr, 255);
    }

    // initialize lists with NULL for now
    tempSVG -> rectangles = createRectangleList(svgNode);
    tempSVG -> circles = createCircleList(svgNode);
    tempSVG -> paths = createPathList(svgNode);
    tempSVG -> groups = createGroupList(svgNode);
    tempSVG -> otherAttributes = createAttributeList(svgNode, NULL, 0);

    // free svgImg object
    xmlFreeDoc(svgImg);
    
    // free any global variables that may have been used by the parser
    xmlCleanupParser();
    return tempSVG;
}

bool writeSVG(const SVG* img, const char* fileName) {
    if (img == NULL || fileName == NULL) {
        return false;
    }
    
    
    xmlDoc *imgDoc = SVGToDoc(img);
    // open file for writing xml 
    xmlSaveFile(fileName, imgDoc);
    
    return true;
}

bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute) {
    return false;
}

void addComponent(SVG* img, elementType type, void* newElement) {

}

char* attrToJSON(const Attribute *a) {
    return NULL;
}

char* circleToJSON(const Circle *c) {
    return NULL;
}

char* rectToJSON(const Rectangle *r) {
    return NULL;
}

char* pathToJSON(const Path *p) {
    return NULL;
}

char* groupToJSON(const Group *g) {
    return NULL;
}

char* attrListToJSON(const List *list) {
    return NULL;
}

char* circListToJSON(const List *list) {
    return NULL;
}

char* rectListToJSON(const List *list) {
    return NULL;
}

char* pathListToJSON(const List *list) {
    return NULL;
}

char* groupListToJSON(const List *list) {
    return NULL;
}

char* SVGtoJSON(const SVG* img) {
    return NULL;
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
    char *tempString = malloc(sizeof(char) * (strlen("Attribute: ") + strlen(tempAttr -> name) + 2));

    // return null if malloc failed
    if (tempString == NULL) {
        return NULL;
    }
    // concatenate name and value followed by new lines to string and return it
    strcpy(tempString, "Attribute: ");
    strcat(tempString, tempAttr -> name);
    strcat(tempString, "=");

    // if value is NULl just return the string with the name
    if (tempAttr -> value == NULL || strcmp(tempAttr -> value, "") == 0) {
        return tempString;
    }
    
    // otherwise reallocate memory for the tempString to fit in the value
    tempString = realloc(tempString, sizeof(char) * 
        (strlen(tempString)  + strlen(tempAttr -> value) + 1));    

    strcat(tempString, tempAttr -> value);

    return tempString;
}

// returns 0 if equal and 1 otherwise (follows strcmp)
int compareAttributes(const void *first, const void *second) {
    return strcmp(attributeToString((Attribute *) first), attributeToString((Attribute *) second));
}  


void deleteGroup(void* data){
    // free lists inside the group struct
    freeList(((Group *)data) -> rectangles);
    freeList(((Group *)data) -> circles);
    freeList(((Group *)data) -> paths);
    freeList(((Group *)data) -> groups);
    freeList(((Group *)data) -> otherAttributes);

    // free pointer to group 
    free(data);
}

char* groupToString(void* data) {
    if (data == NULL) {
        return NULL;
    }
    Group *group = (Group *)data;

    // copy list strings into groupString 
    char *tempString = toString(group -> rectangles);
    char *groupString = malloc(sizeof(char) * (strlen("Element type: Group\n") + strlen(tempString) + 1));
    
    strcpy(groupString, "Element type: Group\n");
    strcat(groupString, tempString);
    free(tempString);
    
    tempString = toString(group -> circles);
    groupString = realloc(groupString, sizeof(char) * (strlen(groupString) + strlen(tempString) + 1));
    strcat(groupString, tempString);
    free(tempString);

    tempString = toString(group -> paths);
    groupString = realloc(groupString, sizeof(char) * (strlen(groupString) + strlen(tempString) + 1));
    strcat(groupString, tempString);
    free(tempString);

    tempString = toString(group -> groups);
    groupString = realloc(groupString, sizeof(char) * (strlen(groupString) + strlen(tempString) + 1));
    strcat(groupString, tempString);
    free(tempString);

    tempString = toString(group -> otherAttributes);
    groupString = realloc(groupString, sizeof(char) * (strlen(groupString) + strlen(tempString) + 1));
    strcat(groupString, tempString);
    free(tempString);
    
    groupString = realloc(groupString, sizeof(char) * (strlen(groupString) + strlen("\nEnd of group elements.\n") + 1));
    strcat(groupString, "\nEnd of group elements.\n");

    return groupString;
}

int compareGroups(const void *first, const void *second){
    return strcmp(groupToString((Group *)first), groupToString((Group *)second));    
}

void deleteRectangle(void* data) {
    freeList(((Rectangle *)data) -> otherAttributes);
    free(data);
}

char* rectangleToString(void* data){
    // cant print null rectangle
    if (data == NULL) {
        return NULL;
    }
    Rectangle *rect = (Rectangle *)data;
    
    // call floatToString on x,y, width and height and concatenate them to rectString
    char *temp = floatToString(rect -> x);
    char *rectString = malloc(sizeof(char) * (strlen("Element type: Rectangle\nx: ") + strlen(temp) + 2));
    
    strcpy(rectString, "Element type: Rectangle\nx: ");
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);
    
    temp = floatToString(rect -> y);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen("y: ") + strlen(temp) + 2));
    strcat(rectString, "y: ");
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);
    
    temp = floatToString(rect -> width);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen("width: ") + strlen(temp) + 2));
    strcat(rectString, "width: ");
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);

    temp = floatToString(rect -> height);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen("height: ") + strlen(temp) + 2));
    strcat(rectString, "height: ");
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);

    // should only add new line if units is empty
    temp = rect -> units;
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen("units: ") + strlen(temp) + 2));
    strcat(rectString, "units: ");
    strcat(rectString, temp);
    strcat(rectString, "\n");
    
    // add list of otherAttributes to the string and return it
    temp = toString(rect -> otherAttributes);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen(temp) + 1));
    strcat(rectString, temp);
    free(temp);

    return rectString;
}

int compareRectangles(const void *first, const void *second){
    return strcmp(rectangleToString((Rectangle *)first), rectangleToString((Rectangle *)second));
}

void deleteCircle(void* data){
    freeList(((Circle *)data) -> otherAttributes);
    free(data);
}

char* circleToString(void* data){
    // check if circle ptr is null
    if (data == NULL) {
        return NULL;
    }
    
    //easier access to data's elements without having to cast to Circle * every time
    Circle *temp = (Circle *)data;
    char *tempString = floatToString(temp -> cx);
    char *circleString = malloc(sizeof(char) * (strlen("Element type: Circle\ncx: ") + strlen(tempString) + 2));

    // concatenate circle elements to the string
    strcpy(circleString, "Element type: Circle\ncx: ");
    strcat(circleString, tempString);
    strcat(circleString, "\n");
    free(tempString); 
    

    tempString = floatToString(temp -> cy);
    // reallocate memory for circleString and tempString plus new line after.
    circleString = realloc(circleString, sizeof(char) * (strlen("cy: ") + strlen(circleString) + strlen(tempString) + 2));
    strcat(circleString, "cy: ");
    strcat(circleString, tempString);
    strcat(circleString, "\n");
    free(tempString);

    tempString = floatToString(temp -> r);
    circleString = realloc(circleString, sizeof(char) * (strlen("r: ") + strlen(circleString) + strlen(tempString) + 2));
    strcat(circleString, "r: ");
    strcat(circleString, tempString);
    strcat(circleString, "\n");
    free(tempString); // free memory from floatToString call
    
    circleString = realloc(circleString, sizeof(char) * (strlen("units: ") + strlen(circleString) + strlen(temp -> units) + 2));
    strcat(circleString, "units: ");
    strcat(circleString, temp -> units);
    strcat(circleString, "\n");

    // concatenate list of attributes' strings to the circleString
    tempString = toString(temp -> otherAttributes);
    circleString = realloc(circleString, sizeof(char) * (strlen(circleString) + strlen(tempString) + 1));
    strcat(circleString, tempString);
    free(tempString);

    return circleString;
}

// returns 0 if both circles are equal, non-zero otherwise (follows strcmp convention)
int compareCircles(const void *first, const void *second){
    return strcmp(circleToString((Circle *)first), circleToString((Circle *)second));
}

void deletePath(void* data){
    freeList(((Path *)data) -> otherAttributes);
    free(data);
}

char* pathToString(void* data){
    // cant print null path
    if (data == NULL) {
        return NULL;
    }

    Path *path = (Path *)data;
    char *pathString;
    char *tempString;

    // in case no otherAttributes were present copy empty string to tempString 
    if (path -> otherAttributes == NULL) {
        tempString = malloc(sizeof(char));
        strcpy(tempString, "");
    }
    else {
        tempString = toString(path -> otherAttributes);
    }
    
    // copy otherAttributes then data to pathString and return it
    pathString = malloc(sizeof(char) * (strlen("Element type: Path\nData: ")
    + strlen(path -> data) + strlen(tempString) + 2)
     );
    strcpy(pathString, "Element type: Path\n");
    strcat(pathString, tempString);
    strcat(pathString, "\n");
    strcat(pathString, "Data: ");
    strcat(pathString, path -> data);

    free(tempString);

    return pathString;
}

// return 1 if not equal, 0 if equal
int comparePaths(const void *first, const void *second){
    return strcmp(pathToString((Path *)first), pathToString((Path *)second));
}
