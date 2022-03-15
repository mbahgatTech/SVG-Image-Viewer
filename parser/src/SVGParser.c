/*
Authored by: Mazen Bahgat (1157821)
Last Revision Date: March 14th 2022
*/


#include "SVGHelpers.h"
#include "SVGParser.h"

bool JSONtoSVGFile (char *jsonString, char *filename) {
    if (jsonString == NULL || filename == NULL) {
        return false;
    }
    
    // generate a new svg struct from the json string and validate it
    SVG *new = JSONtoSVG(jsonString);
    if (new == NULL) {
        return false;
    } 
    
    if (!validateSVG(new, "svg.xsd")) {
        deleteSVG(new);
        return false;
    }
    
    // write the svg struct to a file with name fileName
    bool result = writeSVG(new, filename);
    deleteSVG(new);

    return result;
}

bool createShape(char *type, char *jsonString, char *fileName) {
    if (type == NULL || jsonString == NULL || fileName == NULL) {
        return false;
    }
        
    // read the svg file into a new SVG struct
    SVG *new = createSVG(fileName);
    if (new == NULL) {
        return false;
    }

    // create the shape of type "type" from the jsonString parameter
    // and add it to the svg struct
    if (strcmp(type, "RECT") == 0) {
        Rectangle *rect = JSONtoRect(jsonString);
        addComponent(new, RECT, rect);
    }
    else if (strcmp(type, "CIRC") == 0) {
        Circle *circ = JSONtoCircle(jsonString);
        addComponent(new, CIRC, circ);
    }
    else if (strcmp(type, "PATH") == 0) {
        Path *path = JSONtoPath(jsonString);
        addComponent(new, PATH, path);
    }
    else {
        deleteSVG(new);
        return false;
    }
    
    // validate the new SVG struct and write it back to the file
    if(!validateSVG(new, "svg.xsd")) {
        deleteSVG(new);
        return false;
    }
    writeSVG(new, fileName);
    deleteSVG(new);

    return true;
}

bool appendAttributeToFile (char *type, char *jsonString, int index, char *fileName) {
    if (type == NULL || jsonString == NULL || fileName == NULL || index < 0) {
        return false;
    }
    
    // create an SVG struct from the given file name
    SVG *new = createSVG(fileName);
    if(new == NULL) {
        return false;
    }

    // create the attribute struct from the given jsonString
    Attribute *attr = JSONtoAttr(jsonString);
    if (attr == NULL) {
        deleteSVG(new);
        return false;
    }

    // call set attribute on the svg struct using the given parameters
    if (strcmp(type, "RECT") == 0) {
        setAttribute(new, RECT, index, attr);
    }
    else if (strcmp(type, "CIRC") == 0) {
        setAttribute(new, CIRC, index, attr);
    }
    else if (strcmp(type, "PATH") == 0) {
        setAttribute(new, PATH, index, attr);
    }
    else if (strcmp(type, "GROUP") == 0) {
        setAttribute(new, GROUP, index, attr);
    }
    else {
        deleteSVG(new);
        return false;
    }
    
    // validate the new svg struct and write it to the file
    if (!validateSVG(new, "svg.xsd")) {
        deleteSVG(new);
        return false;
    }
    writeSVG(new, fileName);
    deleteSVG(new);

    return true;
}

char *fileToJSON(char *file) {
    if (file == NULL) {
        return NULL;
    }
    
    // create and validate and SVG struct from the given file name
    SVG *new = createSVG(file);
    if(!validateSVG(new, "svg.xsd")) {
        deleteSVG(new);
        return NULL;
    }
    
    // convert the struct to a jsonString summary
    char *jsonString = SVGtoJSON(new);

    List *objects = getRects(new);
    char *rectString = rectListToJSON(objects);
    char *attrsString = shapeListToAttrsJSON(objects, RECT);
    freeList(objects);

    objects = getCircles(new);
    char *circString = circListToJSON(objects);
    char *attrsString2 = shapeListToAttrsJSON(objects, CIRC);
    freeList(objects);

    objects = getPaths(new);
    char *pathString = pathListToJSON(objects);
    char *attrsString3 = shapeListToAttrsJSON(objects, PATH);
    freeList(objects);

    objects = getGroups(new);
    char *groupString = groupListToJSON(objects);
    char *attrsString4 = shapeListToAttrsJSON(objects, GROUP);
    freeList(objects);

    // add more fields to the JSON string, title, desc and components
    char *finalString = malloc(sizeof(char) * (strlen(jsonString) + strlen(",\"rects\":,\"circs\":,\"paths\":,\"groups\":,\"attrs\":") 
        + strlen(",\"rectAttrsList\":,\"circsAttrsList\":,\"pathsAttrsList\":,\"groupsAttrsList\":") + strlen(attrsString) + 
        strlen(attrsString2) + strlen(attrsString3) + strlen(attrsString4) + strlen(rectString) + strlen(circString) + strlen(pathString) 
        + strlen(groupString) + 1));

    jsonString[strlen(jsonString) - 1] = ',';        
    sprintf(finalString, "%s\"rects\":%s,\"circs\":%s,\"paths\":%s,\"groups\":%s"
            ",\"rectsAttrsList\":%s,\"circsAttrsList\":%s,\"pathsAttrsList\":%s,\"groupsAttrsList\":%s}", 
        jsonString, rectString, circString, pathString, groupString, attrsString,
        attrsString2, attrsString3, attrsString4);
    
    deleteSVG(new);
    free(jsonString);
    free(rectString);
    free(circString);
    free(pathString);
    free(groupString);
    free(attrsString);
    free(attrsString2);
    free(attrsString3);
    free(attrsString4);

    return finalString;
}

char *shapeListToAttrsJSON (List *shapes, elementType type) {
    if (shapes == NULL) {
        return NULL;
    }
    
    char **attrsList = NULL;
    ListIterator iter = createIterator(shapes);
    void *data;
    int i = 0;

    // loop through all the shapes and add each shape's attribute list
    // to  the array of strings.
    while((data = nextElement(&iter)) != NULL) {
        attrsList = realloc(attrsList, sizeof(char *) * (i + 1));

        if (type == RECT) {
            attrsList[i] = attrListToJSON(((Rectangle *)data) -> otherAttributes);
        }

        if (type == CIRC) {
            attrsList[i] = attrListToJSON(((Circle *)data) -> otherAttributes);
        }

        if (type == PATH) {
            attrsList[i] = attrListToJSON(((Path *)data) -> otherAttributes);
        }

        if (type == GROUP) {
            attrsList[i] = attrListToJSON(((Group *)data) -> otherAttributes);
        }

        i++;
    }
    
    char *jsonString = malloc(sizeof(char) * 2);
    strcpy (jsonString, "[");

    // add the attribute lists to jsonString containing a list of JSON arrays
    for (i = 0; i < shapes -> length; i++) {
        jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(attrsList[i]) + 2));

        if (i > 0) {
            strcat(jsonString, ",");
        }

        strcat(jsonString, attrsList[i]);

        free(attrsList[i]);
        attrsList[i] = NULL;
    }
    
    // close the JSON list and return it
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 2));
    strcat(jsonString, "]");
    free(attrsList);
    
    return jsonString;
}

char *getTitle (char *file) {
    if (file == NULL) {
        return NULL;
    }
    
    // create and validate and SVG struct from the given file name
    SVG *new = createSVG(file);
    if(!validateSVG(new, "svg.xsd")) {
        deleteSVG(new);
        return NULL;
    }
    
    // copy the title, delete the svg and return the temp string
    char *temp = malloc(sizeof(char) * (strlen(new -> title) + 1));
    strcpy(temp, new -> title);
    deleteSVG(new);

    return temp;
}

char *getDesc (char *file) {
    if (file == NULL) {
        return NULL;
    }
    
    // create and validate and SVG struct from the given file name
    SVG *new = createSVG(file);
    if(!validateSVG(new, "svg.xsd")) {
        deleteSVG(new);
        return NULL;
    }
    
    // copy the desc, delete the svg and return the temp string
    char *temp = malloc(sizeof(char) * (strlen(new -> description) + 1));
    strcpy(temp, new -> description);
    deleteSVG(new);

    return temp;
}

SVG* createSVG(const char* fileName) {
    LIBXML_TEST_VERSION

    if (fileName == NULL) {
        return NULL;
    }

    xmlDoc *svgImg = xmlReadFile(fileName, NULL, 0);
    if (svgImg == NULL) {
        xmlCleanupParser();
        return NULL;
    }
    
    xmlNode *svgNode = xmlDocGetRootElement(svgImg);
    if (svgNode == NULL) {
        xmlFreeDoc(svgImg);
        xmlCleanupParser();
        return NULL;
    }
    
    SVG *tempSVG = malloc(sizeof(SVG));
    if (tempSVG == NULL) {
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
    strncpy(tempSVG -> namespace, (char *)svgNode -> nsDef -> href, sizeof(char) * 255);


    char *tempPtr = findTitle(svgNode);
    // copy first 255 or length chars of title
    if (tempPtr != NULL) {
        strncpy(tempSVG -> title, tempPtr, sizeof(char) * 255);
        tempSVG -> title[255] = '\0'; 
    }
    
    tempPtr = findDesc(svgNode);
    // copy first 255 or length chars of desc
    if (tempPtr != NULL) {
        strncpy(tempSVG -> description, tempPtr, sizeof(char) * 255);
        tempSVG -> description[255] = '\0';
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
        xmlCleanupParser();
        return NULL;
    }

    // validate xmlDoc against schemaFile
    if (!validSVG(svgImg, schemaFile)) {
        xmlFreeDoc(svgImg);
        xmlCleanupParser();
        return NULL;
    }
    
    xmlNode *svgNode = xmlDocGetRootElement(svgImg);
    if (svgNode == NULL) {
        xmlFreeDoc(svgImg);
        xmlCleanupParser();
        return NULL;
    }
    
    SVG *tempSVG = malloc(sizeof(SVG));
    if (tempSVG == NULL) {
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
    strncpy(tempSVG -> namespace, (char *)svgNode -> nsDef -> href, sizeof(char) * 255);


    char *tempPtr = findTitle(svgNode);
    // copy first 255 or length chars of title
    if (tempPtr != NULL) {
        strncpy(tempSVG -> title, tempPtr, sizeof(char) * 255);
        tempSVG -> title[255] = '\0'; 
    }
    
    tempPtr = findDesc(svgNode);
    // copy first 255 or length chars of desc
    if (tempPtr != NULL) {
        strncpy(tempSVG -> description, tempPtr, sizeof(char) * 255);
        tempSVG -> description[255] = '\0'; 
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
    
    // create an xmlDoc from the SVG img
    xmlDoc *imgDoc = SVGToDoc(img);
    if(imgDoc == NULL) {
        return false;
    }

    // write the doc tree to the file 
    if (xmlSaveFile(fileName, imgDoc) == -1) {
        // -1 return means fail
        xmlFreeDoc(imgDoc);
        xmlCleanupParser();
        return false;
    }

    xmlFreeDoc(imgDoc);
    xmlCleanupParser();
    
    return true;
}

bool validateSVG(const SVG* img, const char* schemaFile) {
    if (img == NULL || schemaFile == NULL) {
        return false;
    }
    
    // convert SVG strcut to doc and check for validity against schemaFile
    xmlDoc *doc = SVGToDoc(img);
    if (doc == NULL) {
        return false;
    }

    if(!validSVG(doc, schemaFile)) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return false;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    // now check for constraints of the values in the struct //

    // rectangle constraints
    void *data;
    List *objects = getRects(img);
    ListIterator iter = createIterator(objects);

    // check for negative height or width and  null attributes pointer
    while((data = nextElement(&iter)) != NULL) {
        Rectangle *rect = (Rectangle *)data;

        if (rect -> otherAttributes == NULL || rect -> width < 0 || rect -> height < 0) {
            freeList(objects);
            return false;
        }

        // check all other attributes for invalid name pointer
        if (!checkValidAttrs(rect -> otherAttributes)) {
            freeList(objects);
            return false;
        }
    }
    freeList(objects);

    // circle constraints
    objects = getCircles(img);
    iter = createIterator(objects);

    // check for null attribute list or negative radius
    while((data = nextElement(&iter)) != NULL) {
        Circle *circle = (Circle *)data;

        if (circle -> otherAttributes == NULL || circle -> r < 0) {
            freeList(objects);
            return false;
        }

        // check all other attributes for invalid name pointer
        if (!checkValidAttrs(circle -> otherAttributes)) {
            freeList(objects);
            return false;
        }
    }
    freeList(objects);

    // path constraints
    objects = getPaths(img);
    iter = createIterator(objects);

    // check for null attribute list or null data pointer
    while((data = nextElement(&iter)) != NULL) {
        Path *path = (Path *)data;

        if (path -> otherAttributes == NULL || path -> data == NULL) {
            freeList(objects);
            return false;
        }
        
        // check all other attributes for invalid name pointer
        if (!checkValidAttrs(path -> otherAttributes)) {
            freeList(objects);
            return false;
        }
    }
    freeList(objects);

    // Group constraints
    objects = getGroups(img);
    iter = createIterator(objects);

    while((data = nextElement(&iter)) != NULL) {
        Group *group = (Group *)data;
        
        // check for null shape or attribute lists
        if (group -> rectangles == NULL || group -> circles == NULL || group -> paths == NULL
        || group -> groups == NULL || group -> otherAttributes == NULL) {
            freeList(objects);
            return false;
        }
        
        // check all other attributes for invalid name pointer
        if (!checkValidAttrs(group -> otherAttributes)) {
            freeList(objects);
            return false;
        }
    }
    freeList(objects);
    
    // check other attributes of img for invalid name pointer
    if (!checkValidAttrs(img -> otherAttributes)) {
        return false;
    }

    return true;
}

bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute) {
    if (img == NULL || newAttribute == NULL || newAttribute -> name == NULL || 
    newAttribute -> value == NULL) {
        return false;
    }
    
    // set attribute for SVG struct
    if (elemType == SVG_IMG) {
        if (img -> otherAttributes == NULL) {
            return false;
        }
        
        // newAttribute will be either appended or edited into otherAttributes
        if (!editAttributes(img -> otherAttributes, newAttribute)) {
            return false;
        }
    }// check the type of element and insert the attribute for element in elemIndex
    else if (elemType == RECT) {
        if (img -> rectangles == NULL) {
            return false;
        }
        
        // check for valid index
        int len = getLength(img -> rectangles);
        if (elemIndex < 0 || elemIndex >= len) {
            return false;
        }

        void *data;
        ListIterator iter = createIterator(img -> rectangles);
        int i = 0;
        while ((data = nextElement(&iter)) != NULL && i < len) {
            if (i == elemIndex) {
                break;
            }

            i++;
        }
        // data is rectangle at elemIndex
        Rectangle *rect = (Rectangle *)data;
        char units[50];
        // check if newAttribute is one of the core attributes
        if(strcmp(newAttribute -> name, "x") == 0) {
            if (!getUnits(units, newAttribute -> value, &rect -> x)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else if(strcmp(newAttribute -> name, "y") == 0) {
            if (!getUnits(units, newAttribute -> value, &rect -> y)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else if(strcmp(newAttribute -> name, "width") == 0) {
            if (!getUnits(units, newAttribute -> value, &rect -> width)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else if(strcmp(newAttribute -> name, "height") == 0) {
            if (!getUnits(units, newAttribute -> value, &rect -> height)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else {
            // newAttribute will be either appended or edited into otherAttributes
            if (!editAttributes(rect -> otherAttributes, newAttribute)) {
                return false;
            }
        }
    }
    else if(elemType == CIRC) {
        if (img -> circles == NULL) {
            return false;
        }
        
        // check for valid index
        int len = getLength(img -> circles);
        if (elemIndex < 0 || elemIndex >= len) {
            return false;
        }

        void *data;
        ListIterator iter = createIterator(img -> circles);
        int i = 0;
        while ((data = nextElement(&iter)) != NULL && i < len) {
            if (i == elemIndex) {
                break;
            }

            i++;
        }
        // data is circle at elemIndex
        Circle *circle = (Circle *)data;
        char units[50];
        // check if newAttribute is one of the core attributes
        if(strcmp(newAttribute -> name, "cx") == 0) {
            if(!getUnits(units, newAttribute -> value, &circle -> cx)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else if(strcmp(newAttribute -> name, "y") == 0) {
            if(!getUnits(units, newAttribute -> value, &circle -> cy)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else if(strcmp(newAttribute -> name, "r") == 0) {
            if(!getUnits(units, newAttribute -> value, &circle -> r)) {
                return false;
            }

            free(newAttribute -> name);
            free(newAttribute);
        }
        else {
            // newAttribute will be either appended or edited into otherAttributes
            if (!editAttributes(circle -> otherAttributes, newAttribute)) {
                return false;
            }
        }
    } 
    else if(elemType == PATH) {
        if (img -> paths == NULL) {
            return false;
        }
        
        // check for valid index
        int len = getLength(img -> paths);
        if (elemIndex < 0 || elemIndex >= len) {
            return false;
        }

        void *data;
        ListIterator iter = createIterator(img -> paths);
        int i = 0;
        Node *temp = iter.current;
        while ((data = nextElement(&iter)) != NULL && i < len) {
            if (i == elemIndex) {
                
                break;
            }
            temp = temp -> next;

            i++;
        }

        // data is path at elemIndex
        Path *path = (Path *)data;
        // check if newAttribute is one of the core attributes
        if(strcmp(newAttribute -> name, "d") == 0) {
            // reallocate Path for enough space for newAttribute value
            path = realloc(path, sizeof(Path) + sizeof(char) * (strlen(newAttribute -> value) + 5));
            strcpy(path -> data, newAttribute -> value);
            temp -> data = path;
            
            free(newAttribute -> name);
            free(newAttribute);
        }
        else {
            // newAttribute will be either appended or edited into otherAttributes
            if (!editAttributes(path -> otherAttributes, newAttribute)) {
                return false;
            }
        }
    }
    else if(elemType == GROUP) {
        if (img -> groups == NULL) {
            return false;
        }
        
        // check for valid index
        int len = getLength(img -> groups);
        if (elemIndex < 0 || elemIndex >= len) {
            return false;
        }

        void *data;
        ListIterator iter = createIterator(img -> groups);
        int i = 0;
        while ((data = nextElement(&iter)) != NULL && i < len) {
            if (i == elemIndex) {
                break;
            }

            i++;
        }
        // data is group at elemIndex
        Group *group = (Group *)data;
        // newAttribute will be either appended or edited into otherAttributes
        if (!editAttributes(group -> otherAttributes, newAttribute)) {
            return false;
        }
    }
    else {
        return false;
    }
    
    return true;
}

void addComponent(SVG* img, elementType type, void* newElement) {
    if (img == NULL || newElement == NULL) {
        return;
    }
    
    // function only handles rectangles, circles and paths

    // check for elementType and add newElement to the list of its type
    if (type == RECT) {
        if (img -> rectangles) {
            insertBack(img -> rectangles, newElement);
        }
    }
    else if (type == CIRC) {
        if (img -> circles) {
            insertBack(img -> circles, newElement); 
        }
    }
    else if(type == PATH) {
        if (img -> paths) {
            insertBack(img -> paths, newElement);
        }
    }
    else if(type == GROUP) {
        if(img -> groups) {
            insertBack(img -> groups, newElement);
        }
    }
}

bool addComponentToGroup (elementType type, int index, List *groups, void *newElement) {
    if (groups == NULL || groups -> length == 0 || index < 0 || index > groups -> length || newElement == NULL) {
        return false;
    }

    // iterate over the groups array until the specified group is reached
    int i = 0;
    void *data;
    ListIterator iter = createIterator(groups);
    while((data = nextElement(&iter)) != NULL && i < index);

    // check for elementType and add newElement to the list of its type
    Group *grp = (Group *)data;
    if (type == RECT) {
        if (grp -> rectangles) {
            insertBack(grp -> rectangles, newElement);
        }
        else {
            return false;
        }
    }
    else if (type == CIRC) {
        if (grp -> circles) {
            insertBack(grp -> circles, newElement); 
        }
        else {
            return false;
        }
    }
    else if(type == PATH) {
        if (grp -> paths) {
            insertBack(grp -> paths, newElement);
        }
        else {
            return false;
        }
    }

    return true;
}

char* attrToJSON(const Attribute *a) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "{}");

    if (a == NULL || a -> name == NULL) {
        return jsonString;
    }
    
    // copy attribute contents to jsonString in JSON format
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(a -> name) +
                        strlen("\"name\":\"\",\"value\":\"\"") + strlen(a -> value) + 1));
    strcpy(jsonString, "{");
    strcat(jsonString, "\"name\":\"");
    strcat(jsonString, a -> name);
    strcat(jsonString,"\",\"value\":\"");
    strcat(jsonString, a -> value);
    strcat(jsonString, "\"}");

    return jsonString;
}

char* circleToJSON(const Circle *c) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "{}");
    
    if (c == NULL || c -> otherAttributes == NULL) {
        return jsonString;
    }
    
    // copy circle contents to jsonString in JSON format
    char *temp = malloc(sizeof(char) * 500);
    sprintf(temp, "%.2f", c -> cx);

    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"cx\":") + strlen(temp) + 1));
    strcpy(jsonString, "{");
    strcat(jsonString, "\"cx\":");
    strcat(jsonString, temp);

    sprintf(temp, "%.2f", c -> cy);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"cy\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"cy\":");
    strcat(jsonString, temp);
    
    sprintf(temp, "%.2f", c -> r);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"r\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"r\":");
    strcat(jsonString, temp);  

    sprintf(temp, "%d", c -> otherAttributes -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"numAttr\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"numAttr\":");
    strcat(jsonString, temp);  
    
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                            strlen(",\"units\":\"\"}") + strlen(c -> units) + 1));
    strcat(jsonString,",\"units\":\"");
    strcat(jsonString, c -> units);
    strcat(jsonString, "\"}"); 

    free(temp); 
    
    return jsonString;
}

char* rectToJSON(const Rectangle *r) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "{}");
    
    if (r == NULL || r -> otherAttributes == NULL) {
        return jsonString;
    }
    
    // copy rectangle contents to jsonString in JSON format
    char *temp = malloc(sizeof(char) * 500);
    sprintf(temp, "%.2f", r -> x);

    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"x\":") + strlen(temp) + 1));
    strcpy(jsonString, "{");
    strcat(jsonString, "\"x\":");
    strcat(jsonString, temp);

    sprintf(temp, "%.2f", r -> y);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"y\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"y\":");
    strcat(jsonString, temp);
    
    sprintf(temp, "%.2f", r -> width);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"w\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"w\":");
    strcat(jsonString, temp); 

    sprintf(temp, "%.2f", r -> height);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"h\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"h\":");
    strcat(jsonString, temp);  

    sprintf(temp, "%d", r -> otherAttributes -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"numAttr\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"numAttr\":");
    strcat(jsonString, temp);  
    
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                            strlen(",\"units\":\"\"}") + strlen(r -> units) + 1));
    strcat(jsonString,",\"units\":\"");
    strcat(jsonString, r -> units);
    strcat(jsonString, "\"}"); 

    free(temp); 
    
    return jsonString;
}

char* pathToJSON(const Path *p) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "{}");
    
    if (p == NULL || p -> otherAttributes == NULL || p -> data == NULL) {
        return jsonString;
    }
    
    // copy up to 64 characters of the d attribute into jsonString 
    char *temp = malloc(sizeof(char) * 65); 
    strncpy(temp, p -> data, sizeof(char) * 64);

    // truncate data after 64 chars
    if (strlen(p -> data) >= 64) {
        temp[64] = '\0';
    }
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                        strlen("\"d\":\"\"") + strlen(temp) + 1));
    strcpy(jsonString, "{");
    strcat(jsonString,"\"d\":\"");
    strcat(jsonString, temp);
    strcat(jsonString, "\"");
    
    // copy num of otherAttributes into jsonString
    sprintf(temp, "%d", p -> otherAttributes -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen(",\"numAttr\":") + strlen(temp) + 2));
    strcat(jsonString,",\"numAttr\":");
    strcat(jsonString, temp);  
    strcat(jsonString, "}"); 

    free(temp); 
    
    return jsonString;
}

char* groupToJSON(const Group *g) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "{}");
    
    if (g == NULL || g -> rectangles == NULL || g -> circles == NULL ||
            g -> paths == NULL || g -> groups == NULL) {
        return jsonString;
    }
    
    // add children and num of attributes to jsonString
    int len = g -> rectangles -> length + g -> circles -> length + 
            g -> paths -> length + g -> groups -> length;
    char *temp = malloc(sizeof(char) * 500);
    sprintf(temp, "%d", len); 

    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                        strlen("\"children\":") + strlen(temp) + 1));
    strcpy(jsonString, "{");
    strcat(jsonString,"\"children\":");
    strcat(jsonString, temp);
    
    // copy num of otherAttributes into jsonString
    sprintf(temp, "%d", g -> otherAttributes -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen(",\"numAttr\":") + strlen(temp) + 2));
    strcat(jsonString,",\"numAttr\":");
    strcat(jsonString, temp);  
    strcat(jsonString, "}"); 

    free(temp); 
    
    return jsonString;
}

char* attrListToJSON(const List *list) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "[]");

    if (list == NULL) {
        return jsonString;
    }
    
    // start the list string with '['
    strcpy(jsonString, "[");

    void *data;
    ListIterator iter = createIterator((List *)list);
    // loop through all attributes and add their strings to jsonString
    while ((data = nextElement(&iter)) != NULL) {
        char *temp = attrToJSON((Attribute *)data);
        jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(temp) + 2));
        
        // add a comma before the current attribute string only when
        // jsonString only contains [. means this is the first element.
        if (strcmp(jsonString, "[") != 0) {
            strcat(jsonString, ",");
        }
        
        strcat(jsonString, temp);
        free(temp);
    }
    
    // close the list string with ] char
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 2));
    strcat(jsonString, "]");

    return jsonString;
}

char* circListToJSON(const List *list) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "[]");

    if (list == NULL) {
        return jsonString;
    }
    
    // start the list string with '['
    strcpy(jsonString, "[");

    void *data;
    ListIterator iter = createIterator((List *)list);
    // loop through all circles and add their strings to jsonString
    while ((data = nextElement(&iter)) != NULL) {
        char *temp = circleToJSON((Circle *)data);
        jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(temp) + 2));
        
        // add a comma before the current circle string only when
        // jsonString only contains [. means this is the first element.
        if (strcmp(jsonString, "[") != 0) {
            strcat(jsonString, ",");
        }
        
        strcat(jsonString, temp);
        free(temp);
    }
    
    // close the list string with ] char
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 2));
    strcat(jsonString, "]");

    return jsonString;
}

char* rectListToJSON(const List *list) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "[]");

    if (list == NULL) {
        return jsonString;
    }
    
    // start the list string with '['
    strcpy(jsonString, "[");

    void *data;
    ListIterator iter = createIterator((List *)list);
    // loop through all rectangles and add their strings to jsonString
    while ((data = nextElement(&iter)) != NULL) {
        char *temp = rectToJSON((Rectangle *)data);
        jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(temp) + 2));
        
        // add a comma before the current rectangle string only when
        // jsonString only contains [. means this is the first element.
        if (strcmp(jsonString, "[") != 0) {
            strcat(jsonString, ",");
        }
        
        strcat(jsonString, temp);
        free(temp);
    }
    
    // close the list string with ] char
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 2));
    strcat(jsonString, "]");

    return jsonString;
}

char* pathListToJSON(const List *list) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "[]");

    if (list == NULL) {
        return jsonString;
    }
    
    // start the list string with '['
    strcpy(jsonString, "[");

    void *data;
    ListIterator iter = createIterator((List *)list);
    // loop through all paths and add their strings to jsonString
    while ((data = nextElement(&iter)) != NULL) {
        char *temp = pathToJSON((Path *)data);
        jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(temp) + 2));
        
        // add a comma before the current path string only when
        // jsonString only contains [. means this is the first element.
        if (strcmp(jsonString, "[") != 0) {
            strcat(jsonString, ",");
        }
        
        strcat(jsonString, temp);
        free(temp);
    }
    
    // close the list string with ] char
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 2));
    strcat(jsonString, "]");

    return jsonString;
}

char* groupListToJSON(const List *list) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "[]");

    if (list == NULL) {
        return jsonString;
    }
    
    // start the list string with '['
    strcpy(jsonString, "[");

    void *data;
    ListIterator iter = createIterator((List *)list);
    // loop through all groups and add their strings to jsonString
    while ((data = nextElement(&iter)) != NULL) {
        char *temp = groupToJSON((Group *)data);
        jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + strlen(temp) + 2));
        
        // add a comma before the current group string only when
        // jsonString only contains [. means this is the first element.
        if (strcmp(jsonString, "[") != 0) {
            strcat(jsonString, ",");
        }
        
        strcat(jsonString, temp);
        free(temp);
    }
    
    // close the list string with ] char
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 2));
    strcat(jsonString, "]");

    return jsonString;
}

char* SVGtoJSON(const SVG* img) {
    // default string 
    char *jsonString = malloc(sizeof(char) * 3);
    strcpy(jsonString, "{}");
    
    if (img == NULL) {
        return jsonString;
    }
    
    // copy img contents to jsonString in JSON format // 

    // get lists and add their lengths to jsonString
    char *temp = malloc(sizeof(char) * 500);
    List *objects = getRects(img);
    if (objects == NULL) {
        // return default string if list is null
        strcpy(jsonString, "{}");
        return jsonString;
    }
    sprintf(temp, "%d", objects -> length);

    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"numRect\":") + strlen(temp) + 1));
    strcpy(jsonString, "{");
    strcat(jsonString, "\"numRect\":");
    strcat(jsonString, temp);
    freeList(objects);
    
    // get number of circles
    objects = getCircles(img);
    if (objects == NULL) {
        strcpy(jsonString, "{}");
        return jsonString;
    }
    sprintf(temp, "%d", objects -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"numCirc\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"numCirc\":");
    strcat(jsonString, temp);
    freeList(objects);

    // get number of paths    
    objects = getPaths(img);
    if (objects == NULL) {
        strcpy(jsonString, "{}");
        return jsonString;
    }
    sprintf(temp, "%d", objects -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"numPaths\":,") + strlen(temp) + 1));
    strcat(jsonString,",\"numPaths\":");
    strcat(jsonString, temp);
    freeList(objects);

    // get number of groups
    objects = getGroups(img);
    if (objects == NULL) {
        strcpy(jsonString, "{}");
        return jsonString;
    }
    sprintf(temp, "%d", objects -> length);
    jsonString = realloc(jsonString, sizeof(char) * (strlen(jsonString) + 
                                        strlen("\"numGroups\":,") + strlen(temp) + 2));
    strcat(jsonString,",\"numGroups\":");
    strcat(jsonString, temp); 
    strcat(jsonString, "}");
    freeList(objects);

    free(temp); 
    
    return jsonString;
}

SVG* JSONtoSVG(const char* svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    SVG *img = malloc(sizeof(SVG));

    // initialize ns, title amd desc with default values
    strcpy(img -> namespace, "http://www.w3.org/2000/svg");
    strcpy(img -> title, "");
    strcpy(img -> description, "");

    // parse svgString for title and desc
    char *temp = getField((char *)svgString, "title");
    if (temp != NULL) {
        strncpy(img -> title, temp, sizeof(char) * 255);
        img -> title[255] = '\0';
        free(temp);
    }

    temp = getField((char *)svgString, "descr");
    if (temp != NULL) {
        strncpy(img -> description, temp, sizeof(char) * 255);
        img -> description[255] = '\0';
        free(temp);
    }

    // initialize empty lists 
    img -> rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    img -> circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    img -> paths = initializeList(&pathToString, &deletePath, &comparePaths);
    img -> groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    img -> otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    
    // indicates failure of list initialization and should return null 
    if (img -> rectangles == NULL || img -> circles == NULL || img -> paths == NULL
    || img -> groups == NULL || img -> otherAttributes == NULL) {
        return NULL;
    }

    return img;
}

Rectangle* JSONtoRect(const char* svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    // set deafult values for the struct elements
    Rectangle *rect = malloc(sizeof(Rectangle));
    rect -> x = 0;
    rect -> y = 0;
    rect -> height = 0;
    rect -> width = 0;
    strcpy(rect -> units, "");
    rect -> otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // parse svgString for its attributes
    char *tempUnits = rect -> units; // point to units element
    char *temp = getField((char *)svgString, "x");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &rect -> x);
        free(temp);
    }
    
    temp = getField((char *)svgString, "y");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &rect -> y);
        free(temp);
    }

    temp = getField((char *)svgString, "w");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &rect -> width);
        free(temp);
    }

    temp = getField((char *)svgString, "h");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &rect -> height);
        free(temp);
    }

    temp = getField((char *)svgString, "units");
    if (temp != NULL) {
        strcpy(rect -> units, temp);
        free(temp);
    }

    return rect;
}

Circle* JSONtoCircle(const char* svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    // set deafult values for the struct elements
    Circle *circle = malloc(sizeof(Circle));
    circle -> cx = 0;
    circle -> cy = 0;
    circle -> r = 0;
    strcpy(circle -> units, "");
    circle -> otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // parse svgString for its attributes
    char *tempUnits = circle -> units; // point to units element
    char *temp = getField((char *)svgString, "cx");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &circle -> cx);
        free(temp);
    }
    
    temp = getField((char *)svgString, "cy");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &circle -> cy);
        free(temp);
    }

    temp = getField((char *)svgString, "r");
    if (temp != NULL) {
        getUnits(tempUnits, temp, &circle -> r);
        free(temp);
    }

    temp = getField((char *)svgString, "units");
    if (temp != NULL) {
        strcpy(circle -> units, temp);
        free(temp);
    }

    return circle;
}

Path *JSONtoPath (const char *svgString) {
    if (svgString == NULL) {
        return NULL;
    }
    
    // initialize a new path with empty data and other attrs
    Path *newPath = malloc(sizeof(Path) + (sizeof(char) * 65));
    strcpy(newPath -> data, "");
    newPath -> otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // copy the first 64 characters of the path field in svgString into path data
    char *temp = getField((char *)svgString, "d");
    if (temp != NULL) {
        strncpy(newPath -> data, temp, sizeof(char) * 64);
        newPath -> data[64] = '\0';
        free(temp);
    }

    return newPath; 
}

Attribute *JSONtoAttr (const char *svgString) {
    if (svgString == NULL) {
        return NULL;
    }

    // get the value of the the attribute from svgString and allocate enough memory
    // for the flexible array item
    Attribute *attr = NULL;
    char *temp = getField((char *)svgString, "value");
    if (temp != NULL) {
        attr = malloc(sizeof(Attribute) + (sizeof(char) * strlen(temp) + 1));
        strcpy(attr -> value, temp);
        free(temp);
    }
    
    // get the name of the attribute and return null
    temp = NULL;
    temp = getField((char *)svgString, "name");
    if (temp != NULL) {
        attr -> name = malloc(sizeof(char) * (strlen(temp) + 1));
        strcpy(attr -> name, temp);
        free(temp);
    }

    return attr; 
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


void deleteGroup(void* data) {
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
