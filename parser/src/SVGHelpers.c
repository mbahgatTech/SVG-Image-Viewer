/*
Authored by: Mazen Bahgat (1157821)
Last Revision Date: February 18th 2022
*/

#include "SVGHelpers.h"

char *findTitle (xmlNode *node) {
    // returns NULL if the parent node or list of children is NULL
    if (node == NULL || node -> children == NULL) {
        return NULL;
    }
    
    // go through all  the nodes children and look for a child with name "title"
    for (xmlNode *tempNode = node -> children; tempNode; tempNode = tempNode -> next) {
        if (strcmp((char *)tempNode -> name, "title") == 0) {
            // go through title's children to find the xmlNode with name "text"
            for (xmlNode *textNode = tempNode ->children; textNode; textNode = textNode -> next) {
                if (strcmp((char *)textNode -> name, "text") == 0) { 
                    return (char *)textNode -> content; 
                }
            }
        }
    }
    
    // null returned if there is no title
    return NULL;
}

char *findDesc (xmlNode *node) {
    // returns NULL if the parent node or list of children is NULL
    if (node == NULL || node -> children == NULL) {
        return NULL;
    }

    // go through all  the nodes children and look for a child with name "title"
    for (xmlNode *tempNode = node -> children; tempNode; tempNode = tempNode -> next) {
        if (strcmp((char *)tempNode -> name, "desc") == 0) {
            // go through desc's children to find the xmlNode with name "text"
            for (xmlNode *textNode = tempNode ->children; textNode; textNode = textNode -> next) {
                if (strcmp((char *)textNode -> name, "text") == 0) { 
                    return (char *)textNode -> content; 
                }
            }
        }
    }

    return NULL;
}


char *floatToString(float number) {
    // 500 chars should be more than enough for any number
    char *numString = malloc(sizeof(float) * 500); 
    sprintf(numString, "%f", number);
    
    // reallocate for memory to fit exactly the number
    numString = realloc(numString, sizeof(char) * (strlen(numString) + 1)); 

    return numString;
}

bool getUnits(char *units, char *string, float *value) {
    if (string == NULL || units == NULL || (!isdigit(string[0]) && string[0] != '.' && string[0] != '-')) {
        return false; // indicates error
    }
    
    bool negativeVal = false;
    if (string[0] == '-') {
        negativeVal = true;
    }

    char *unitString = NULL; 
    int i = 0;
    for (i = 0; i < strlen(string); i++) {
        // check if the current char is a part of a float number
        if (!isdigit(string[i]) && string[i] != '.') {
            // this basically starts unitString from the first non digit/non period char
            unitString = string + sizeof(char) * i;
            break;
        }
    }
    
    // copy units into passed by reference units array
    if (unitString != NULL) {
        strncpy(units, unitString, sizeof(char) * 49);
        units[49] = '\0';
    }
    // reuse unitString for value of float
    unitString = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(unitString, string); //now u can exclude units from the string
    unitString[i] = '\0';
    
    // remove the negative sign from the beginning of the string
    if (negativeVal) {
        char *floatString = unitString + sizeof(char);
        strcpy(unitString, floatString);
    }
    
    *value = atof(unitString);
    free(unitString);
    
    // negate value if the string originally begun with
    // a negative sign
    if (negativeVal) {
        *value = *value * (-1);
    }

    return true;
}

List *createAttributeList(xmlNode *node, char **coreAttributes, int length) {
    List *attributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    Attribute **temp = malloc(sizeof(Attribute *));
    
    // avoid seg fault if called on null node or node without properties
    if (node == NULL || node -> properties == NULL) {
        free(temp);
        return attributes;
    } 

    // loop through all properties of the node
    int skip = 0;
    int counter = 0;
    for (xmlAttr *attr = node -> properties; attr; attr = attr -> next) {
        for (int i = 0; i < length; i++) {  
            // assign 1 to skip to indicate current property is 1 of the core ones
            if (strcmp((char *)attr -> name, coreAttributes[i]) == 0) {
                skip = 1;
            }
        }
        
        // skip property if the skip value is 1
        if (skip == 1) {
            skip = 0;
            continue;
        }
        
        temp = realloc(temp, sizeof(Attribute *) * (counter + 1));
        
        // allocate memory for value (need to consider empty values)
        temp[counter] = malloc(sizeof(Attribute) + sizeof(char) * (strlen((char *)attr -> children -> content) + 1));
        if ((char *)attr -> children -> content != NULL) {
            strcpy(temp[counter] -> value, (char *)attr -> children -> content);
        }
        else {
            strcpy(temp[counter] -> value, "");
        }

        temp[counter] -> name = malloc(sizeof(char) * (strlen((char *)attr -> name) + 1));
        strcpy(temp[counter] -> name, (char *)attr -> name);
        insertBack(attributes, temp[counter]); // insert attribute at the end of the list

        counter++;
    }
    
    // free pointer to array of pointers (each atttribute is actually not freed and is part of list)
    free(temp);
    return attributes;
}

List *createRectangleList(xmlNode *img) {
    if (img == NULL) {
        return NULL;
    }
    List *rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    Rectangle **rect = malloc(sizeof(Rectangle *));
    char **coreAttr = malloc(sizeof(char *) * 4);
    char *tempUnits;

    // add core attributes to an array of strings
    coreAttr[0] = malloc(sizeof(char) * 2);
    strcpy(coreAttr[0], "x");
    coreAttr[1] = malloc(sizeof(char) * 2);
    strcpy(coreAttr[1], "y");
    coreAttr[2] = malloc(sizeof(char) * 6);
    strcpy(coreAttr[2], "width");
    coreAttr[3] = malloc(sizeof(char) * 7);
    strcpy(coreAttr[3], "height");

    int counter = 0;
    for (xmlNode *child = img -> children; child; child = child -> next) {
        if (strcmp((char *)child -> name, "rect") != 0) {
            continue;
        }
        
        // realloc memory for a new rectangle pointer and allocate memory for rectangle
        rect = realloc(rect, sizeof(Rectangle *) * (counter + 1));
        rect[counter] = malloc(sizeof(Rectangle));

        // set deafult values for the struct elements
        rect[counter] -> x = 0;
        rect[counter] -> y = 0;
        rect[counter] -> height = 0;
        rect[counter] -> width = 0;
        strcpy(rect[counter] -> units, "");
        tempUnits = rect[counter] -> units;
        
        // initialize core rectangle properties 
        for (xmlAttr *attr = child-> properties; attr; attr = attr -> next) {
            if (strcmp((char *)attr -> name, "x") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &rect[counter] -> x);
            }
            else if (strcmp((char *)attr -> name, "y") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &rect[counter] -> y);
            }
            else if (strcmp((char *)attr -> name, "width") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &rect[counter] -> width);
            }
            else if (strcmp((char *)attr -> name, "height") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &rect[counter] -> height);
            }   
        } 

        // any other attributes not listed in the above loop will be added to otherAttributes list
        rect[counter] -> otherAttributes = createAttributeList(child, coreAttr, 4);

        insertBack(rectangles, rect[counter]);
        counter++;
    }
    for (int i = 0; i < 4; i++) {
        free(coreAttr[i]);
    }
    free(coreAttr);
    free(rect);

    return rectangles;
}

List *createCircleList(xmlNode *img) {
    if (img == NULL) {
        return NULL;
    }
    
    List *circleList = initializeList(&circleToString, &deleteCircle, &compareCircles);
    Circle **circles = malloc(sizeof(Circle *));
    char **coreAttr = malloc(sizeof(char *) * 3);
    char *tempUnits;

    // add core attributes to an array of strings
    coreAttr[0] = malloc(sizeof(char) * 3);
    strcpy(coreAttr[0], "cx");
    coreAttr[1] = malloc(sizeof(char) * 3);
    strcpy(coreAttr[1], "cy");
    coreAttr[2] = malloc(sizeof(char) * 2);
    strcpy(coreAttr[2], "r");

    int counter = 0;
    for (xmlNode *child = img -> children; child; child = child -> next) {
        if (strcmp((char *)child -> name, "circle") != 0) {
            continue;
        }
        
        // reallocate memory for a new Circle
        circles = realloc(circles, sizeof(Circle *) * (counter + 1));
        circles[counter] = malloc(sizeof(Circle));

        // set defaults for struct elements
        circles[counter] -> cx = 0;
        circles[counter] -> cy = 0;
        circles[counter] -> r = 0;
        strcpy(circles[counter] -> units, "");
        tempUnits = circles[counter] -> units;

        // initialize core circle properties 
        for (xmlAttr *attr = child-> properties; attr; attr = attr -> next) {
            if (strcmp((char *)attr -> name, "cx") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &circles[counter] -> cx);
            }
            else if (strcmp((char *)attr -> name, "cy") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &circles[counter] -> cy);
            }
            else if (strcmp((char *)attr -> name, "r") == 0) {
                getUnits(tempUnits, (char *)attr -> children -> content, &circles[counter] -> r);
            }
        } 

        // any other attributes not listed in the above loop will be added to otherAttributes list
        circles[counter] -> otherAttributes = createAttributeList(child, coreAttr, 3);

        insertBack(circleList, circles[counter]);
        counter++;
    }
    // free working pointers (core attrs and array of circle pointers)
    for (int i = 0; i < 3; i++) {
        free(coreAttr[i]);
    }
    free(coreAttr);
    free(circles);

    return circleList;
}

List *createPathList(xmlNode *img) {
    if (img == NULL) {
        return NULL;
    }
    
    List *pathList = initializeList(&pathToString, &deletePath, &comparePaths);
    Path **paths = malloc(sizeof(Path *));
    char **coreAttr = malloc(sizeof(char *) * 1);

    // add core "d" attribute to an array of strings for to distinguish it from other attributes
    coreAttr[0] = malloc(sizeof(char) * 2);
    strcpy(coreAttr[0], "d");

    int counter = 0;
    for (xmlNode *child = img -> children; child; child = child -> next) {
        if (strcmp((char *)child -> name, "path") != 0) {
            continue;
        }
        
        // reallocate memory for a new path
        paths = realloc(paths, sizeof(Path *) * (counter + 1));
        paths[counter] = malloc(sizeof(Path) + sizeof(char));
        strcpy(paths[counter] -> data, ""); 

        // initialize core Path property
        for (xmlAttr *attr = child-> properties; attr; attr = attr -> next) {
            if (strcmp((char *)attr -> name, "d") == 0) {
                // allocate enough memory for structure and flexible array member
                // to fit the value of "d" attribute of the path element 
                paths[counter] = realloc(paths[counter], sizeof(Path) + sizeof(char) * (strlen((char *)attr -> children -> content) + 1));
                strcpy(paths[counter] -> data, (char *)attr -> children -> content);
                break;
            }
        } 

        // any other attributes not listed in the above loop will be added to otherAttributes list
        paths[counter] -> otherAttributes = createAttributeList(child, coreAttr, 1);

        insertBack(pathList, paths[counter]);
        counter++;
    }
    // free working pointers (core attrs and array of Path pointers)
    free(coreAttr[0]);
    free(coreAttr);
    free(paths);

    return pathList;
}

List *createGroupList(xmlNode *node) {
    if (node == NULL) {
        return NULL;
    }
    
    List *groupList = initializeList(&groupToString, &deleteGroup, &compareGroups);
    Group **groups = malloc(sizeof(Group *));

    int counter = 0;
    // loop through all children of the node and look for g elements
    for (xmlNode *child = node -> children; child; child = child -> next) {
        if (strcmp((char *)child -> name, "g") != 0) {
            continue;
        }
        
        // reallocate memory for a new Group
        groups = realloc(groups, sizeof(Group *) * (counter + 1));
        groups[counter] = malloc(sizeof(Group));

        // create a list of all attributes on the g element since were not storing 
        // any core attributes in strcut elements
        groups[counter] -> otherAttributes = createAttributeList(child, NULL, 0);

        // create lists of other shapes inside this group element and store ptrs in the struct
        groups[counter] -> rectangles = createRectangleList(child);
        groups[counter] -> circles = createCircleList(child);
        groups[counter] -> paths = createPathList(child);        
        groups[counter] -> groups = createGroupList(child);

        insertBack(groupList, groups[counter]);
        counter++;
    }

    // free array of pointers to Group objects
    free(groups);

    return groupList;
}

void addGroupCircles(List *groups, List *circles) {
    // no groups to look for circles in
    if (groups == NULL || groups -> length == 0) {
        return;
    }
    
    ListIterator groupIter = createIterator(groups), circleIter;
    Group *temp = (Group *)groupIter.current -> data;
    void *data;
    
    // loop through all groups and add their circles to the list
    while((temp = (Group *)nextElement(&groupIter)) != NULL) {
        // loop through circles list in current group and add circle to front of list
        circleIter = createIterator(temp -> circles);
        while((data = nextElement(&circleIter)) != NULL) {
            insertBack(circles, data);
        }

        // recursive call for inner groups
        addGroupCircles(temp -> groups, circles);
    }
}

void addGroupPaths(List *groups, List *paths) {
    // no groups to look for paths in 
    if (groups == NULL || groups -> length == 0) {
        return;
    }
    
    ListIterator groupIter = createIterator(groups), pathIter;
    Group *temp;
    void *data;
    
    // loop through all groups and add their paths to the list
    while((temp = (Group *)nextElement(&groupIter)) != NULL) {
        // loop through paths list in current group and add path to front of paths list
        pathIter = createIterator(temp -> paths);
        while((data = nextElement(&pathIter)) != NULL) {
            insertBack(paths, data);
        }

        // recursive call for inner groups
        addGroupPaths(temp -> groups, paths);
    }
}

void addGroupRects(List *groups, List *rects) {
    // no groups to look for Rects in 
    if (groups == NULL || groups -> length == 0) {
        return;
    }
    
    ListIterator groupIter = createIterator(groups), rectIter;
    Group *temp;
    void *data;
    
    // loop through all groups and add their Rects to the list
    while((temp = (Group *)nextElement(&groupIter)) != NULL) {
        // loop through rectangle list in current group and add rectangle to front of rects list
        rectIter = createIterator(temp -> rectangles);
        while((data = nextElement(&rectIter)) != NULL) {
            insertBack(rects, data);
        }

        // recursive call for inner groups
        addGroupRects(temp -> groups, rects);
    }
}

void addInnerGroups(List *groups, List *allGroups) {
    // no groups to look for groups in 
    if (groups == NULL || groups -> length == 0) {
        return;
    }
    
    ListIterator groupIter = createIterator(groups);
    Group *temp;
    
    // iterate through "groups" list and add to allGroups with its inner groups
    while((temp = (Group *)nextElement(&groupIter)) != NULL) {
        insertBack(allGroups, temp);
        
        // add groups list in curent group to the allGroups
        addInnerGroups(temp -> groups, allGroups);
    }
}

// delete function that does nothing to its data
// use: freeing lists that point to elements in other lists
void dummyDel(void *data) {

}

bool validSVG(xmlDoc *img, const char *xsdFile) {
    if (img == NULL || xsdFile == NULL) {
        return false;
    }

    // create new parser context from xsd file name
    xmlSchemaParserCtxtPtr myCtxt =  xmlSchemaNewParserCtxt(xsdFile);
    if (myCtxt == NULL) {
        return false;
    }
    
    xmlSchemaPtr mySchema = xmlSchemaParse(myCtxt);
    xmlSchemaFreeParserCtxt(myCtxt);

    if (mySchema == NULL) {
        return false;
    }
    
    // validate img tree to myCtxt
    xmlSchemaValidCtxtPtr validCtxt = xmlSchemaNewValidCtxt(mySchema);
    if (validCtxt == NULL) {
        xmlSchemaFree(mySchema);
        xmlSchemaCleanupTypes();

        return false;
    }

    int result = xmlSchemaValidateDoc(validCtxt, img);

    // free memory used by libxml2
    xmlSchemaFreeValidCtxt(validCtxt);
    xmlSchemaFree(mySchema);
    xmlSchemaCleanupTypes();

    // 0 means its a valid svg image tree
    if (result != 0) {
        return false;
    }

    return true;
}

xmlDoc *SVGToDoc(const SVG *img) {
    if (img == NULL) {
        return NULL;
    }
    
    // create new doc and an svg fragment
    xmlDoc *doc = xmlNewDoc((const xmlChar *)"1.0");
    if (doc == NULL) {
        return NULL;
    }

    xmlNode *svgFragment = xmlNewDocFragment(doc);
    if (svgFragment == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    xmlNode *svgNode = xmlNewChild(svgFragment, NULL, (const xmlChar *)"svg", NULL);
    xmlDocSetRootElement(doc, svgNode); // svg element is root element of doc
    if (svgNode == NULL) {
        free(svgFragment);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }
    
    // create nameSpace and assign it to svgNode
    xmlNs *nameSpace = xmlNewNs(svgNode, (const xmlChar *)img -> namespace, NULL);
    xmlSetNs(svgNode, nameSpace);
    
    // create title and desc elements if existent
    if (strlen(img -> title) > 0) {
        // create text child to svgNode with img -> title 
        if (xmlNewTextChild(svgNode, NULL, (const xmlChar *)"title", (const xmlChar *)img -> title) == NULL) {
            free(svgFragment);
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return NULL;
        }
    }

    if (strlen(img -> description) > 0) {
        // create text child to svgNode with img -> description 
        if (xmlNewTextChild(svgNode, NULL, (const xmlChar *)"desc", (const xmlChar *)img -> description) == NULL) {
            free(svgFragment);
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return NULL;
        }
    }

    // get lists and write their elements to xmlNode children of svgNode
    if (!createRectNodes(svgNode, img -> rectangles)) {
        free(svgFragment);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }
    
    if (!createCircleNodes(svgNode, img -> circles)) {
        free(svgFragment);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    if (!createPathNodes(svgNode, img -> paths)) {
        free(svgFragment);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    if (!createGroupNodes(svgNode, img -> groups)) {
        free(svgFragment);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }
    
    createProps(svgNode, img -> otherAttributes);
    free(svgFragment);

    return doc;
}

bool createRectNodes(xmlNode *svgNode, List *rectangles) {
    if (svgNode == NULL || rectangles  == NULL) {
        return false;
    }
 
    void *data;
    ListIterator iter = createIterator(rectangles);

    while((data = nextElement(&iter)) != NULL) {
        Rectangle *currRect = (Rectangle *)data;

        // make a new child rect child node 
        xmlNode *rectNode = xmlNewChild(svgNode, NULL, (const xmlChar *)"rect", NULL);
        if(rectNode == NULL) {
            return false;
        }
        
        xmlAttr *attrs = NULL;
        char *floatString = NULL;
        
        // add x attribute to the rectangle
        if ((floatString = floatToString(currRect -> x)) == NULL) {
            return false;
        }
        else if (strlen(currRect -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currRect -> units) + 1));
            strcat(floatString, currRect -> units);
        }
        attrs = xmlNewProp(rectNode, (const xmlChar *)"x", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }
        
        // add y attribute
        if ((floatString = floatToString(currRect -> y)) == NULL) {
            return false;
        }
        else if (strlen(currRect -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currRect -> units) + 1));
            strcat(floatString, currRect -> units);
        }
        attrs = xmlNewProp(rectNode, (const xmlChar *)"y", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }
        
        // add width attribute
        if ((floatString = floatToString(currRect -> width)) == NULL) {
            return false;
        }
        else if (strlen(currRect -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currRect -> units) + 1));
            strcat(floatString, currRect -> units);
        }
        attrs = xmlNewProp(rectNode, (const xmlChar *)"width", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }
        
        if ((floatString = floatToString(currRect -> height)) == NULL) {
            return false;
        }
        else if (strlen(currRect -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currRect -> units) + 1));
            strcat(floatString, currRect -> units);
        }
        attrs = xmlNewProp(rectNode, (const xmlChar *)"height", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }

        createProps(rectNode, currRect -> otherAttributes);
    }

    return true;
}

bool createCircleNodes(xmlNode *svgNode, List *circles) {
    if (svgNode == NULL || circles  == NULL) {
        return false;
    }

    void *data;
    ListIterator iter = createIterator(circles);

    while((data = nextElement(&iter)) != NULL) {
        Circle *currCircle = (Circle *)data;

        // make a new Circle child node 
        xmlNode *circleNode = xmlNewChild(svgNode, NULL, (const xmlChar *)"circle", NULL);
        if(circleNode == NULL) {
            return false;
        }
        
        xmlAttr *attrs = NULL;
        char *floatString = NULL;
        
        // add cx attribute to the Circle
        if ((floatString = floatToString(currCircle -> cx)) == NULL) {
            return false;
        }
        else if (strlen(currCircle -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currCircle -> units) + 1));
            strcat(floatString, currCircle -> units);
        }
        attrs = xmlNewProp(circleNode, (const xmlChar *)"cx", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }
        
        // add cy attribute
        if ((floatString = floatToString(currCircle -> cy)) == NULL) {
            return false;
        }
        else if (strlen(currCircle -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currCircle -> units) + 1));
            strcat(floatString, currCircle -> units);
        }
        attrs = xmlNewProp(circleNode, (const xmlChar *)"cy", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }
        
        // add r attribute
        if ((floatString = floatToString(currCircle -> r)) == NULL) {
            return false;
        }
        else if (strlen(currCircle -> units) > 0) {
            // add units to the string if it exists in the struct
            floatString = realloc(floatString, sizeof(char) * (strlen(floatString) + 
                        strlen(currCircle -> units) + 1));
            strcat(floatString, currCircle -> units);
        }
        attrs = xmlNewProp(circleNode, (const xmlChar *)"r", (const xmlChar *)floatString);
        free(floatString);

        if (attrs == NULL) {
            return false;
        }

        createProps(circleNode, currCircle -> otherAttributes);
    }

    return true;
}

bool createPathNodes(xmlNode *svgNode, List *paths) {
    if (svgNode == NULL || paths  == NULL) {
        return false;
    }

    void *data;
    ListIterator iter = createIterator(paths);
    // loop through all paths and add their nodes to children of svgNode
    while((data = nextElement(&iter)) != NULL) {
        Path *currPath = (Path *)data;

        // make a new Path child node 
        xmlNode *pathNode = xmlNewChild(svgNode, NULL, (const xmlChar *)"path", NULL);
        if(pathNode == NULL) {
            return false;
        }
        
        // add d attribute to the Path
        xmlAttr *attrs = NULL;
        char *string = malloc(sizeof(char) * strlen(currPath -> data) + 1);

        if (string == NULL) {
            return false;
        }

        strcpy(string, currPath -> data);
        attrs = xmlNewProp(pathNode, (const xmlChar *)"d", (const xmlChar *)string);
        free(string);

        if (attrs == NULL) {
            return false;
        }

        createProps(pathNode, currPath -> otherAttributes);
    }

    return true;
}

bool createGroupNodes(xmlNode *svgNode, List *groups) {
    if (svgNode == NULL || groups == NULL) {
        return false;
    }

    void *data;
    ListIterator iter = createIterator(groups);

    // loop through all groups and add their nodes to children of svgNode
    while((data = nextElement(&iter)) != NULL) {
        Group *currGroup = (Group *)data;
        
        xmlNode *groupNode = xmlNewChild(svgNode, NULL, (const xmlChar *)"g", NULL);
        if (groupNode == NULL) {
            return false;
        }

        // get attributes of the group added to the node
        createProps(groupNode, currGroup -> otherAttributes);
        
        // add shape nodes added as elements to the group
        if (!createRectNodes(groupNode, currGroup -> rectangles)) {
            return false;
        }

        if (!createCircleNodes(groupNode, currGroup -> circles)) {
            return false;
        }
        
        if(!createPathNodes(groupNode, currGroup -> paths)) {
            return false;
        }


        if (!createGroupNodes(groupNode, currGroup -> groups)) {
            return false;
        }
    }
    
    return true;
}

void createProps(xmlNode *node, List *attributes) {
    if (node == NULL || attributes == NULL) {
        return;
    }
    
    ListIterator iter = createIterator(attributes);
    void *data;
    
    // loop through all attributes and add them as properties to node
    while ((data = nextElement(&iter)) != NULL) {
        // add currAttr to properties of node
        Attribute *currAttr = (Attribute *)data;
        xmlAttr *attr = xmlNewProp(node, (const xmlChar *)currAttr -> name, (const xmlChar *)currAttr -> value);   

        if (attr == NULL) {
            return;
        }
    }
}

bool editAttributes(List *otherAttributes, Attribute *newAttribute) {
    if (otherAttributes == NULL || newAttribute == NULL) {
        return false;
    }

    void *data;
    ListIterator iter = createIterator(otherAttributes);
    ListIterator iter2 = createIterator(otherAttributes);

    // check otherAttribute list if newAttribute already exists
    while ((data = nextElement(&iter)) != NULL) {
        Attribute *ptr = (Attribute *)data;
        if (strcmp(ptr -> name, newAttribute -> name) == 0) {
            // free members of old attribute
            free (ptr -> name);
            ptr = NULL;

            // reallocate memory for the new value of the attribute
            ptr = realloc(ptr, sizeof(Attribute) + sizeof(char) * (strlen(newAttribute -> value) + 1));
            strcpy(ptr -> value, newAttribute -> value);
            
            // allocate memory for the name and copy new attribute's name into it
            ptr -> name = malloc(sizeof(char) * (strlen(newAttribute -> name) + 1));
            strcpy(ptr -> name, newAttribute -> name);

            // insert the new attribute in the list
            Node *node = initializeNode(ptr);
            node -> next = iter2.current -> next;
            node -> previous = iter2.current -> previous;

            if(iter2.current && iter2.current -> previous) {
                iter2.current -> previous -> next = node;
            }
            if (iter2.current && iter2.current -> next) {
                iter2.current -> next -> previous = node; 
            }
            
            // delete the old node containing the old attribute and set
            // other attributes' head and tail accordingly
            Node *old = iter2.current;
            iter2.current = node;
            if (otherAttributes -> head == old) {
                otherAttributes -> head = iter2.current;
            }
            if (otherAttributes -> tail == old) {
                otherAttributes -> tail = iter2.current;
            }
            free(old -> data);
            free(old);
            

            deleteAttribute(newAttribute);
            return true;
        }
        nextElement(&iter2);
    }

    // insert the attribute at the end of the otherAtribute list
    insertBack(otherAttributes, newAttribute);

    return true;
}

char *getField(char *jsonString, char *field) {
    if (jsonString == NULL || field == NULL) {
        return NULL;
    }
    
    // copy jsonString into temp and tokenize it against json chars
    char *buffer;
    char *temp = malloc(sizeof(char) * (strlen(jsonString) + 1));
    strcpy(temp, jsonString);

    // get rid of the first token '{'
    buffer = strtok(temp, "\"");
    while (strcmp(buffer, "{") == 0) {
        buffer = strtok(NULL, "\"");
    }
    
    // tokenize fields until a field with name matching field is reached
    while (buffer != NULL && strcmp(buffer, field) != 0) {
        buffer = strtok(NULL, ",");        
        buffer = strtok(NULL, "\"");
    }
    
    // token value using :,} delims
    buffer = strtok(NULL, ":,}");
    if (buffer == NULL) {
        free(temp);
        return NULL;
    }
    
    // buffer overlaps with temp so I cant copy it directly to temp
    char *temp2 = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(temp2, buffer);    
    strcpy(temp, temp2);

    // remove quotes from returned string
    if (temp2[0] == '\"' && temp2[strlen(temp2) - 1] == '\"') {
        buffer = strtok(temp2, "\"");
        
        if (buffer == NULL || strlen(buffer) == 0) {
            strcpy(temp, "");
        }
        else {
            strcpy(temp, buffer);
        }
    }
    // reallocate appropriate memory for temp and return it
    temp = realloc(temp, sizeof(char) * (strlen(temp) + 1));
    free(temp2);

    return temp;
}

bool checkValidAttrs(List *attributes) {
    if (attributes == NULL) {
        return false;
    }

    // check all other attributes for invalid name pointer
    void *attrs;
    ListIterator iter2 = createIterator(attributes);

    // iterate through all attributes in the list
    while((attrs = nextElement(&iter2)) != NULL) {
        Attribute *currAttr = (Attribute *)attrs;

        // if name is null then its not a valid attribute list
        if (currAttr -> name == NULL) {
            // freeList(objects);
            return false;
        }
    }

    return true;
}