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
    char *svgString = malloc(sizeof(char) * (strlen(img -> namespace) + 2));
    
    // copy SVG data elements
    strcpy(svgString, img -> namespace);
    strcat(svgString, "\n");

    svgString = realloc(svgString, sizeof(char) * (
        strlen(svgString) + strlen(img -> title) + 2)
    );
    strcat(svgString, img -> title);
    strcat(svgString, "\n");

    svgString = realloc(svgString, sizeof(char) * (
        strlen(svgString) + strlen(img -> description) + 2)
    );
    strcat(svgString, img -> description);
    strcat(svgString, "\n");

    // copy SVG lists
    char *temp = toString(img -> rectangles);
    svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
    strcat(svgString, temp);
    free(temp);

    temp = toString(img -> circles);
    svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
    strcat(svgString, temp);
    free(temp);

    temp = toString(img -> paths);
    svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
    strcat(svgString, temp);
    free(temp);

    temp = toString(img -> groups);
    svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
    strcat(svgString, temp);
    free(temp);
    
    temp = toString(img -> otherAttributes);
    svgString = realloc(svgString, sizeof(char) * (strlen(svgString) + strlen(temp) + 1));
    strcat(svgString, temp);
    free(temp);
    
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
    // cant get rectangles of a NULL image
    if (img == NULL) {
        return NULL;
    }
    
    // initialize a list, return it if rectangles list is null
    List *rectList = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
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
    List *circleList = initializeList(&circleToString, &deleteCircle, &compareCircles);
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
    List *groupList = initializeList(&groupToString, &deleteGroup, &compareGroups);
    addInnerGroups(img -> groups, groupList);

    return groupList;
}

List* getPaths(const SVG* img) {
    // cant get paths of a NULL image
    if (img == NULL) {
        return NULL;
    }
    
    // initialize a list, return it if paths list is null
    List *pathList = initializeList(&pathToString, &deletePath, &comparePaths);
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
    freeNodes(rects);
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
    freeNodes(circles);
    return numCircles;
}

int numPathsWithData(const SVG* img, char *data) {
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
    freeNodes(paths);
    
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
    freeNodes(groups);
    
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
    freeNodes(lists);

    lists = getCircles(img);
    iter = createIterator(lists);
    // iterate through circles list of img and add the num of attrs to numAttrs
    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Circle *)data) -> otherAttributes -> length;
    }
    freeNodes(lists);

    lists = getPaths(img);
    iter = createIterator(lists);
    // iterate through paths list of img and add the num of attrs to numAttrs
    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Path *)data) -> otherAttributes -> length;
    }
    freeNodes(lists);
    
    lists = getGroups(img);
    iter = createIterator(lists);
    // iterate through groups list of img and add the num of attrs to numAttrs
    while((data = nextElement(&iter)) != NULL) {
        numAttrs += ((Group *)data) -> otherAttributes -> length;
    }
    freeNodes(lists);

    // add number of attributes in the SVG struct itself
    numAttrs += img -> otherAttributes -> length;

    return numAttrs; 
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
    strcat(tempString, " ");

    // if value is NULl just return the string with the name
    if (tempAttr -> value == NULL || strcmp(tempAttr -> value, "") == 0) {
        return tempString;
    }
    
    // otherwise reallocate memory for the tempString to fit in the value
    tempString = realloc(tempString, sizeof(char) * 
        (strlen(tempAttr -> name)  + strlen(tempAttr -> value) + 2));    

    strcat(tempString, tempAttr -> value);

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
    char *groupString = malloc(sizeof(char) * (strlen(tempString) + 1));

    strcpy(groupString, tempString);
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

    return groupString;
}

int compareGroups(const void *first, const void *second){
    return strcmp(groupToString((Group *)first), groupToString((Group *)second));    
}

void deleteRectangle(void* data){
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
    char *rectString = malloc(sizeof(char) * strlen(temp) + 2);
    strcpy(rectString, temp);
    strcat(rectString, "\n");
    free(temp);
    
    temp = floatToString(rect -> y);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen(temp) + 2));
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);
    
    temp = floatToString(rect -> width);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen(temp) + 2));
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);

    temp = floatToString(rect -> height);
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen(temp) + 2));
    strcat(rectString, temp);
    strcat(rectString, "\n");
    free(temp);

    // should only add new line if units is empty
    temp = rect -> units;
    rectString = realloc(rectString, sizeof(char) * (strlen(rectString) + strlen(temp) + 2));
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
    pathString = malloc(sizeof(char) * (strlen(path -> data) + strlen(tempString) + 2));
    strcpy(pathString, tempString);
    strcat(pathString, "\n");
    strcat(pathString, path -> data);

    free(tempString);

    return pathString;
}

// return 1 if not equal, 0 if equal
int comparePaths(const void *first, const void *second){
    return strcmp(pathToString((Path *)first), pathToString((Path *)second));
}
