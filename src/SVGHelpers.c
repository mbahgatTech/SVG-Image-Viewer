/*
Authored by: Mazen Bahgat (1157821)
Last Revision Date: January 26th 2022
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
    // 50 chars should be more than enough for any number
    char *numString = malloc(sizeof(float) * 50); 
    sprintf(numString, "%f", number);
    
    // reallocate for memory to fit exactly the number
    numString = realloc(numString, sizeof(char) * (strlen(numString) + 1)); 

    return numString;
}

float getUnits(char *units, char *string){
    if (string == NULL || units == NULL) {
        return -1; // indicates error
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
        strcpy(units, unitString);
    }
    // reuse unitString for value of float
    unitString = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(unitString, string); //now u can exclude units from the string
    unitString[i] = '\0';

    float value = atof(unitString);
    free(unitString);

    return value;
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
    if (img == NULL || img -> properties == NULL) {
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
        strcpy(rect[counter] -> units, "");
        tempUnits = rect[counter] -> units;

        // initialize core rectangle properties 
        for (xmlAttr *attr = child-> properties; attr; attr = attr -> next) {
            if (strcmp((char *)attr -> name, "x") == 0) {
                rect[counter] -> x = getUnits(tempUnits, (char *)attr -> children -> content);
            }
            else if (strcmp((char *)attr -> name, "y") == 0) {
                rect[counter] -> y = getUnits(tempUnits, (char *)attr -> children -> content);
            }
            else if (strcmp((char *)attr -> name, "width") == 0) {
                rect[counter] -> width = getUnits(tempUnits, (char *)attr -> children -> content);
            }
            else if (strcmp((char *)attr -> name, "height") == 0) {
                rect[counter] -> height = getUnits(tempUnits, (char *)attr -> children -> content);
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
    if (img == NULL || img -> properties == NULL) {
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
        strcpy(circles[counter] -> units, "");
        tempUnits = circles[counter] -> units;

        // initialize core circle properties 
        for (xmlAttr *attr = child-> properties; attr; attr = attr -> next) {
            if (strcmp((char *)attr -> name, "cx") == 0) {
                circles[counter] -> cx = getUnits(tempUnits, (char *)attr -> children -> content);
            }
            else if (strcmp((char *)attr -> name, "cy") == 0) {
                circles[counter] -> cy = getUnits(tempUnits, (char *)attr -> children -> content);
            }
            else if (strcmp((char *)attr -> name, "r") == 0) {
                circles[counter] -> r = getUnits(tempUnits, (char *)attr -> children -> content);
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
    if (img == NULL || img -> properties == NULL) {
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

        // initialize core Path property
        for (xmlAttr *attr = child-> properties; attr; attr = attr -> next) {
            if (strcmp((char *)attr -> name, "d") == 0) {
                // allocate enough memory for structure and flexible array member
                // to fit the value of "d" attribute of the path element 
                paths[counter] = malloc(sizeof(Path) + sizeof(char) * (strlen((char *)attr -> children -> content) + 1));
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

// the goal of this function is to free nodes of a list
// without freeing the data inside the nodes (data could be used in other lists)
void freeNodes(List *list) {
    if (list == NULL) {
        return;
    }
    
    Node *node;

    // loops through all nodes in a list and frees them
    for (int i = 0; i < list -> length; i++) {
        node = list -> head;
        list -> head = list -> head -> next;
        free(node);
    }

    // frees list pointer
    free(list);
}
