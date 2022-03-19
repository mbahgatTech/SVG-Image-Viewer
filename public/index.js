$(document).ready(function() {
    // append all the files in /uploads to file log
    let files = [];
    let shapes = [];
    let arr = [];
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/get-files',   //The server endpoint we are connecting to
        success: function (data) {
            files = [...data];
            console.log("Fetching files success.");
        },
        fail: function(error) {
            // Non-200 return, do something with error
            alert('Failed to load images from the server.');
            console.log(error); 
        }
    });

    $(document).ajaxComplete(function (event, xhr, settings) {
        if (settings.url != "/get-files" ) {
            return;
        }

        if (!files || files.length < 1) {
            $('#log-panel').append(
                $('<h2/>')
                    .attr("id", "no-files")
                    .text("No Files.")
            );
            return;
        }

        $('#log-panel').find('.file-log').remove();

        for (let file of files) {
            // append a new div element to the file log panel containing the new image
            $('#log-panel').append(
                $('<div/>')
                .attr("id", "newDiv1")
                .addClass("file-log")
        
                // append an image with elem2 id and a link download to the svg image
                .append( 
                    $('<a/>')
                        .attr("href", file.name)
                        .attr("download", file.name)
                        .append(
                            $('<img/>')
                                .attr("src", file.name)
                                .addClass("fieldElems")
                                .attr("id", "elem2")
                        )
                    )
        
                // append a text element with name of the svg file and a download link
                .append(
                    $("<a/>")
                        .addClass("fieldElems")
                        .text(file.name)
                        .attr("href", file.name)
                        .attr("download", file.name)
                )
    
                // append a text elements with the remaining information about the image
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(file.size)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(file.rects)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(file.circs)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(file.paths)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(file.groups)
                )
            );
        }
        
        // get old selected option so you can restore it later 
        // (need this if u made this ajax call after image edits)
        let name2 = $('#image').val();
        let editedFile = 1;
        if(!name2 || name2.length <= 0) {
            editedFile = 0;
            name2 = files[0].name;
        }

        $('.image-select').find('option').remove();
        $('.image-select').each(function () {
            for (let file of files) {
                $(this).append(
                    $('<option/>').text(file.name)
                );
            }
        });
        
        $('#image').val(name2).trigger('change');
        let myFile2 = getFile('#image');

        createSVGView(myFile2);
        
        if (editedFile === 0) {
            return;
        }

        // update images containing the updated image
        $('img').each(function () {
            let oldSrc = $(this).attr('src');
            if (!oldSrc.includes(myFile2.name)) {
                return;
            }

            $(this).attr('src', oldSrc + `?v=${new Date().getTime()}`);
        });
    });
    
    function createSVGView (file) {
        // display the first image in the view panel on load
        shapes = [];
        $("#log2img")
                .attr("src", file.name)
                .attr("alt", file.name);

        $("#title2").children("input").each(function () {
            $(this).val(file.title);
        });
        $("#desc").children("input").each(function () {
            $(this).val(file.descr);
        });

        // remove existing table
        $('#inside-table').find('.file-log').remove();
        
        file.rectList.forEach((rect, index) => {
            let tempdiv = $('<div class="file-log"> \
                            </div>');

            tempdiv.append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Rectangle " + (index + 1))
            )
            .append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Upper left corner: x = " + rect.x + rect.units + ", y = " + rect.y + rect.units + 
                    "\nWidth: " + rect.w + rect.units + ", Height: " + rect.h + rect.units)
            )
            .append (
                $("<div/>")
                    .addClass("view-content")
                    .text(rect.numAttr)
            );

            $("#inside-table").append(tempdiv);
        });
        file.circList.forEach((circ, index) => {
            let tempdiv = $('<div class="file-log"> \
                            </div>');
        
            tempdiv.append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Circle " + (index + 1))
            )
            .append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Centre: x = " + circ.cx + circ.units + ", y = " + circ.cy + circ.units + 
                    ", radius: " + circ.r + circ.units)
            )
            .append (
                $("<div/>")
                    .addClass("view-content")
                    .text(circ.numAttr)
            );
        
            $("#inside-table").append(tempdiv);
        });
        file.pathList.forEach((path, index) => {
            let tempdiv = $('<div class="file-log"> \
                            </div>');
        
            tempdiv.append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Path " + (index + 1))
            )
            .append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Path Data: " + path.d)
            )
            .append (
                $("<div/>")
                    .addClass("view-content")
                    .text(path.numAttr)
            );
        
            $("#inside-table").append(tempdiv);
        });
        file.groupList.forEach((group, index) => {
            let tempdiv = $('<div class="file-log"> \
                            </div>');
        
            tempdiv.append(
                $("<div/>")
                    .addClass("view-content")
                    .text("Group " + (index + 1))
            )
            .append(
                $("<div/>")
                    .addClass("view-content")
                    .text(group.children + " child elements")
            )
            .append (
                $("<div/>")
                    .addClass("view-content")
                    .text(group.numAttr)
            );
        
            $("#inside-table").append(tempdiv);
        });
        
        // push all the shape types to arr 
        arr = [];
        for (let i = 0; i < file.rects; i++) {
            arr.push("Rectangle");
        }
        for (let i = 0; i < file.circs; i++) {
            arr.push("Circle");
        }
        for (let i = 0; i < file.paths; i++) {
            arr.push("Path");
        }
        for (let i = 0; i < file.groups; i++) {
            arr.push("Group");
        }

        for (let i = 0; i < arr.length; i++) {
            // each shape has its own unique index and its own set of attributes
            // type element will help  add a class for the listeners
            let shape = {};
            shape.type = arr[i];
            shape.index = i;
            shape.printIndex = i;
            
            // based on the type of the shape, add its "required" attributes
            // and its "other attributes" to its attributes lis member
            if (shape.type === "Path") {
                let index = i - file.rects - file.circs;
                shape.printIndex = index;
                shape.attributes = [...file.pathsAttrsList[index]];

                // make sure you dont return undefined lists, replace with empty list
                // if undefined
                if (!shape.attributes) {
                    shape.attributes = [];
                }
                
                // push data attribute to the attributes list
                let attr = {};
                attr.name = "data";
                attr.value = file.pathList[index].d;

                shape.attributes.push(attr);
            }
            else if (shape.type === "Rectangle"){
                shape.attributes = [...file.rectsAttrsList[i]];

                // make sure you dont return undefined lists, replace with empty list
                // if undefined
                if (!shape.attributes) {
                    shape.attributes = [];
                }

                let attr = {};
                attr.name = "x";
                attr.value = file.rectList[i].x;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "y";
                attr.value = file.rectList[i].y;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "width";
                attr.value = file.rectList[i].w;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "height";
                attr.value = file.rectList[i].h;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "units";
                attr.value = file.rectList[i].units;
                shape.attributes.push(attr);
            }
            else if (shape.type === "Circle"){
                let index = i - file.rects;
                shape.attributes = [...file.circsAttrsList[index]];
                shape.printIndex = index;

                // make sure you dont return undefined lists, replace with empty list
                // if undefined
                if (!shape.attributes) {
                    shape.attributes = [];
                }

                let attr = {};
                attr.name = "cx";
                attr.value = file.circList[index].cx;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "cy";
                attr.value = file.circList[index].cy;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "radius";
                attr.value = file.circList[index].r;
                shape.attributes.push(attr);

                attr = {};
                attr.name = "units";
                attr.value = file.circList[index].units;
                shape.attributes.push(attr);
            }
            else if (shape.type === "Group") {
                shape.attributes = [...file.groupsAttrsList[i - file.rects - file.circs - file.paths]];
                
                // make sure you dont return undefined lists, replace with empty list
                // if undefined
                if (!shape.attributes) {
                    shape.attributes = [];
                }

                shape.printIndex = i - file.rects - file.circs - file.paths;
            }

            shapes.push(shape);
        }
    }

    function getFile (mySelect) {
        if (!mySelect) {
            return undefined;
        }
        
        // get the selected file name from the mySelect menu
        let fileName = $(mySelect + " option:selected").text();

        // loop through all the files on the server and return the file that
        // matches fileName
        for (let file of files) {
            if (file.name === fileName) {
                return file;
            }
        }
        
        // returns the first file if nothing was selected
        return files[0];
    }

    function addListeners(button, listenTo) {
        // add listeners for all entry boxes (class listenTo)
        // that turn the hide button into  a discard button when input change is made 
        let container = document.querySelectorAll(listenTo);
        for (let i = 0; i < container.length; i++) {
            container[i].addEventListener('input', function() {
                // change the hide button box to discard because the changes will be discarded
                $(button)
                    .attr("value", "Discard")   
                    .css("background-color", "#A80A01");
                console.log("Hide button changed to discard.");
            });
        }
    } 

    function appendAttributes (elemId, newId, elemClass, dataClass, shape) {
        // append an attributes panel that consists of a field labels
        $('#' + elemId).append (
            $("<div/>")
                .addClass("file-log")
                .attr("id", newId)
                .append(
                    $("<div/>")
                        .addClass("fields")
                        .append (
                            $("<div/>")
                                .addClass("view2")
                                .attr("id", "attr-name")
                                .text("Attribute")
                        )
                        .append (
                            $("<div/>")
                                .addClass("view2")
                                .attr("id", "attr-value")
                                .text("Value")
                        )
                )
        );
        
        // loop through all the attributes and append their names and values 
        // to their respective fields
        console.log(shape.attributes);
        for (let attr of shape.attributes) {
            console.log(attr);
            $('#' + newId)
            .append (
                $("<div/>")
                .addClass("file-log")
                .append (
                    $("<div/>")
                        .addClass(elemClass)
                        .text(attr.name)
                )
                // append the value as a text box that can be edited with user content
                .append (
                    $("<div/>")
                        .addClass("view-attrs")
                        .attr("id", "data")
                        .append (
                            $("<input/>")
                                .addClass(dataClass)
                                .attr("type", "text")
                                .attr("value", attr.value)
                                .attr("placeholder", "Enter Value")
                        )
                )
            );
        }
        
        // append buttons after all attributes have been added
        $('#' + newId).append (
            $("<div/>")
                .addClass("panel-buttons add")
                .attr("id", elemId + "-add")
                .append(
                    $("<input/>")
                        .addClass("btn btn-secondary btn-add")
                        .attr("type", "submit")
                        .attr("value", "Add Attribute")
                        .attr("id", elemId + "-btn-add")
                )
        );
    }

    function setShapes (file, newShapes) {
        if (!file || !newShapes) {
            return undefined;
        }
        
        file.rectsAttrsList = [];
        file.circsAttrsList = [];
        file.pathsAttrsList = [];
        file.groupsAttrsList = [];

        newShapes.forEach(function (shape, index) {
            if (shape.type === "Rectangle") {
                // set the default attributes for the rectangle and push it to rectList
                file.rectList[index].x = shape.attributes.filter(function(attr) { return attr.name === 'x'; })[0].value;
                file.rectList[index].y = shape.attributes.filter(function(attr) { return attr.name === 'y'; })[0].value;
                file.rectList[index].w = shape.attributes.filter(function(attr) { return attr.name === 'width'; })[0].value;
                file.rectList[index].h = shape.attributes.filter(function(attr) { return attr.name === 'height'; })[0].value;
                file.rectList[index].units = shape.attributes.filter(function(attr) { return attr.name === 'units'; })[0].value;
                
                // validate that the x,y,w and h values are all numbers
                if (isNaN(file.rectList[index].x)) {
                    throw `Rectangle [${index + 1}] x value is not a valid number.`;
                }

                if (isNaN(file.rectList[index].y)) {
                    console.log("HEER");
                    throw `Rectangle [${index + 1}] y value is not a valid number.`;
                }

                if (isNaN(file.rectList[index].w)) {
                    throw `Rectangle [${index + 1}] width value is not a valid number.`;
                }

                if (isNaN(file.rectList[index].h)) {
                    throw `Rectangle [${index + 1}] height value is not a valid number.`;
                }

                // push other attributes to the rects attrs list and filter out the required ones
                file.rectsAttrsList.push([...shape.attributes]);
                file.rectsAttrsList[index] = file.rectsAttrsList[index].filter(function (attr) { 
                    return attr.name != 'x' && attr.name != 'y' && attr.name != 'width' && attr.name != 'height' && attr.name != 'units';
                });
                file.rectList[index].numAttr = file.rectsAttrsList[index].length;
            }
            else if (shape.type === "Circle") {
                let idx = index - file.rectList.length;
                // set the default attributes for the circle and push it to circList
                file.circList[idx].cx = shape.attributes.filter(function(attr) { return attr.name === 'cx'; })[0].value;
                file.circList[idx].cy = shape.attributes.filter(function(attr) { return attr.name === 'cy'; })[0].value;
                file.circList[idx].r = shape.attributes.filter(function(attr) { return attr.name === 'radius'; })[0].value;
                file.circList[idx].units = shape.attributes.filter(function(attr) { return attr.name === 'units'; })[0].value;

                // validate that the x,y and r values are all numbers
                if (isNaN(file.circList[idx].cx)) {
                    throw `Circle [${idx + 1}] cx value is not a valid number.`;
                }

                if (isNaN(file.circList[idx].cy)) {
                    throw `Circle [${idx + 1}] cy value is not a valid number.`;
                }

                if (isNaN(file.circList[idx].r)) {
                    throw `Circle [${idx + 1}] radius value is not a valid number.`;
                }

                // push other attributes to the circs attrs list and filter out the required ones
                file.circsAttrsList.push([...shape.attributes]);
                file.circsAttrsList[idx] = file.circsAttrsList[idx].filter(function (attr) { 
                    return attr.name != 'cx' && attr.name != 'cy' && attr.name != 'radius' && attr.name != 'units';
                });
                file.circList[idx].numAttr = file.circsAttrsList[idx].length;
            }
            else if (shape.type === "Path") {
                // push other attributes to the paths attrs list and filter out the required ones
                let idx = index - file.rectList.length - file.circList.length;
                file.pathList[idx].d = shape.attributes.filter(function(attr) { return attr.name === 'data'; })[0].value;

                file.pathsAttrsList.push([...shape.attributes]);
                file.pathsAttrsList[idx] = file.pathsAttrsList[idx].filter(function (attr) { return attr.name != 'data'; });
                file.pathList[idx].numAttr = file.pathsAttrsList[idx].length; 
            }
            else if (shape.type === "Group") {
                let idx = index - file.rectList.length - file.circList.length - file.pathList.length;
                // copy the shape attributes 
                file.groupsAttrsList.push([...shape.attributes]);
                file.groupList[idx].numAttr = file.groupsAttrsList[idx].length;
            }
        });
    }

    $(document).on('click', "#btn-show", function() {
        $('#btn-show').attr("value", "Hide Attributes")
                      .attr("id", "btn-hide");
        
        $('#view-panel').append ($('<div/>')
                        .attr("id", "shape-log")
                        .addClass("file-log")
        );

        let file = getFile("#image");
        $('#shape-log').append ($('<h5/>').text("SVG"));
        let svgElem = {attributes:file.svgAttrs};
        // append an attributes panel that consists of a field labels
        appendAttributes("shape-log", "svg1", "view-attrs-svg no-edit-attr-svg", "form-control entry-box2-svg", svgElem);
        addListeners("#btn-hide", ".entry-box2-svg");

        for (let shape of shapes) {
            $('#shape-log').append ($('<h5/>').text(shape.type + " " + (shape.printIndex + 1)));

            // append an attributes panel that consists of a field labels
            appendAttributes("shape-log", `${shape.type}${shape.index}`, "view-attrs no-edit-attr", "form-control entry-box2", shape);
        }

        addListeners("#btn-hide", ".entry-box2");
        console.log('Showing all attributes');
    });
    
    $(document).on('click', "#btn-hide", function() {
        $('#shape-log').remove();
        $('#btn-hide').attr("value", "Show Attributes")
                      .attr("id", "btn-show")
                      .css("background-color", "#9147ff");
        console.log('Hid attributes');
    });

    $(document).on('click', ".btn-add", function() {
        let tempdiv = $("<div/>")
                .addClass("file-log")
                // append the name as a text box that can be edited with user content
                .append (
                    $("<div/>")
                    .addClass("view-attrs data-name")
                    .append (
                        $("<input/>")
                            .addClass("form-control entry-box2")
                            .attr("type", "text")
                            .attr("value", "Name")
                            .attr("placeholder", "Enter Attribute")
                    )
                )
                // append the value as a text box that can be edited with user content
                .append (
                    $("<div/>")
                        .addClass("view-attrs data-val")
                        .append (
                            $("<input/>")
                                .addClass("form-control entry-box2")
                                .attr("type", "text")
                                .attr("value", "Value")
                                .attr("placeholder", "Enter Value")
                        )
                );
        
        // add the appropriate class for svg attribute text boxes
        let svg = 0;
        if ($(this).parent().parent().attr("id") === "svg1") {
            svg = 1;
            tempdiv.children().each(function () {
                $(this).children().removeClass('entry-box2');
                $(this).children().addClass('entry-box2-svg');
            });
        }
        tempdiv.insertBefore($(this).parent());

        if(svg === 0) {
            addListeners("#btn-hide", ".entry-box2");
        }
        else {
            addListeners("#btn-hide", ".entry-box2-svg");
        }

        // change hide button to discard
        $('#btn-hide')
            .attr("value", "Discard")
            .css("background-color", "#A80A01");

        console.log("Adding Default Attribute: Success.");
    });
    
    $(document).on('click', "#btn-submit-view", function() {
        let currFile = getFile("#image");
        let title = '';
        let desc = '';

        // get the title and description values from their entry boxes
        $(".entryBox").each( function() {
            if ($(this).attr('id') === "enter-title-1"){
                title = $(this)[0].value;
            }
            else if ($(this).attr('id') === "enter-desc-1") {
                desc = $(this)[0].value;
            }
        });  

        // validate the title and desc lengths against the character limit
        if (!checkTitleDesc(title, desc)) {
            return;
        }
        currFile.title = title;
        currFile.descr = desc;

        // loop through all the  attributes and add them to a JSON string
        let count2 = 0;
        let staticAttrs = document.querySelectorAll(".no-edit-attr");
        let attr = {};
        let shape = {};
        for (shape of shapes) {
            if($("#shape-log").length === 0) {
                break;
            }
            
            shape.attributes = [];
            $(".entry-box2").each( function() {
                // check which shape has the button field belongs too
                if (!($(this).parent().parent().parent().attr("id") == (shape.type + shape.index))) {
                    return;
                }
    
                // get the name of the current attribute
                if ($(this).attr('placeholder') === "Enter Attribute"){
                    attr.name = $(this)[0].value;
                }
                // get the value of the current attribute
                else if ($(this).attr('placeholder') === "Enter Value") {
                    if (!attr.name) {
                        // get the name of the uneditable attribute at index count2
                        let temp = staticAttrs.item(count2);
                        if (!temp) {
                            console.log("Failed to save the attributes due to undefined names.");
                            alert("ERROR: Failed to save attributes!")
                            console.log($(this)[0].value);
                            return;
                        }
                        
                        // convert html element to a html object
                        temp = $(temp);
                        if (!temp || !temp.text()) {
                            return;
                        }
                        attr.name = temp.text();
                        count2++;
                    }
                    
                    // give the current attribute th value of this.
                    attr.value = $(this)[0].value;
                    if (!attr.value && attr.name != 'units') {
                        console.log("Failed to save the attributes due to empty values.");
                        alert("ERROR: Some or all core attributes are empty, please fill these values before resubmission!")
                        return;
                    }

                    // update the JSON array of attributes with the new attr
                    shape.attributes.push(attr);
                    attr = {};
                }
            }); 
        }
        
        
        // change the discard button to hide attributes
        $('#btn-hide').attr("value", "Hide Attributes")
        .css("background-color", "#9147ff");
        
        try {
            setShapes(currFile, shapes);
        }
        catch(exc) {
            alert(exc);
            console.log(`Invalid attributes were found while setting shapes.`);
            return;
        }
        
        attr = {};
        count2 = 0;
        staticAttrs = staticAttrs = document.querySelectorAll(".no-edit-attr-svg");
        if ($(".entry-box2-svg").length > 0) {
            currFile.svgAttrs = [];
        }
        $(".entry-box2-svg").each(function () {
            // get the name of the current attribute
            if ($(this).attr('placeholder') === "Enter Attribute"){
                attr.name = $(this)[0].value;
            }
            // get the value of the current attribute
            else if ($(this).attr('placeholder') === "Enter Value") {
                if (!attr.name) {
                    // get the name of the uneditable attribute at index count2
                    let temp = staticAttrs.item(count2);
                    if (!temp) {
                        console.log("Failed to save the attributes due to undefined names.");
                        alert("ERROR: Failed to save attributes!")
                        console.log($(this)[0].value);
                        return;
                    }
                    
                    // convert html element to a html object
                    temp = $(temp);
                    if (!temp || !temp.text()) {
                        return;
                    }
                    attr.name = temp.text();
                    count2++;
                }
                
                // give the current attribute th value of this.
                attr.value = $(this)[0].value;
                if (!attr.value) {
                    console.log("Failed to save the attributes due to undefined values.");
                    alert("ERROR: Failed to save attributes!")
                    return;
                }

                // update the JSON array of attributes with the new attr
                currFile.svgAttrs.push(attr);
                attr = {};
            }
        });

        // make a post request with all the shapes and their attributes for the selected file 
        $.ajax({
            type: 'post',            
            dataType: 'json',
            contentType: 'application/json',   
            url: '/post-attrs',   
            data: JSON.stringify({file:currFile}),
            success: function () {
                updateLogs(currFile);
                console.log(`Updated ${currFile.name} successfully.`);
            },
            error: function(error) {
                alert("ERROR: " + error.responseText);
                console.log(error.responseText);
            }
        });
    });

    function updateLogs (file) {
        if (!file) {
            return undefined;
        }

        // sync ajax
        makeCall();
        
        // update the log-panel with new file info
        $("#log-panel").children(".file-log").each(function () {
            let child = $($(this).children("a")[0]);
            if (child.attr("href") === file.name) {
                console.log(child.attr("href"));
                $(this).replaceWith(`
                <div id="newDiv1" class="file-log">
                    <a href="${file.name}" download="${file.name}">
                        <img src="${file.name}?v=${new Date().getTime()}" class="fieldElems" id="elem2">
                    </a>
                    <a class="fieldElems" href="${file.name}" download="${file.name}">${file.name}</a>
                    <div class="fieldElems">${file.size}</div>
                    <div class="fieldElems">${file.rects}</div>
                    <div class="fieldElems">${file.circs}</div>
                    <div class="fieldElems">${file.paths}</div>
                    <div class="fieldElems">${file.groups}</div>
                </div>`);
            }
        });
    }

    function makeCall () {
        return $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/get-files',   //The server endpoint we are connecting to
            success: function (data) {
                files = [...data];
            },
            fail: function(error) {
                // Non-200 return, do something with error
                alert('Failed to load images from the server.');
                console.log(error); 
            }
        });
    }
    
    $(".shape-add").click(function() {
        // make new dropboxes for the user to select the image and the type of
        // shape they want to add to the selected image
        let tempdiv = $('<form id="add-form" action="/add-shape-form" method="post" encType="multipart/form-data"></form>');
        let tempcontent = $('<div class="new-shape"> \
                            <div id="mySelect"> \
                                <label>Select Image</label> \
                                <select id="image2" class="image-select image-select2" name="Image"> \
                                </select><br> \
                            </div> \
                            <label>Select Shape</label> \
                            <select class="shape" name="shape"> \
                                <option value="Rectangle">Rectangle</option> \
                                <option value="Circle">Circle</option> \
                                <option value="Path">Path</option> \
                            </select>\
                            <div class="file-log rect-shape"> \
                                <div class="fields"> \
                                    <div class="view3" id="attr-name">Attribute</div> \
                                    <div class="view3" id="attr-value">Value</div> \
                                </div> \
                                <div class="file-log">\
                                    <div class="view-attrs2 no-edit-attr-shapes">x</div>\
                                    <div class="view-attrs2"> \
                                        <input type="text" name="x" class="form-control entry-box3" value="0.5cm" placeholder="Enter x"> \
                                    </div> \
                                </div> \
                                <div class="file-log"> \
                                    <div class="view-attrs2 no-edit-attr-shapes">y</div> \
                                    <div class="view-attrs2"> \
                                        <input type="text" name="y" class="form-control entry-box3" value="0.5cm" placeholder="Enter y"> \
                                    </div> \
                                </div> \
                                <div class="file-log"> \
                                    <div class="view-attrs2 no-edit-attr-shapes">Width</div> \
                                    <div class="view-attrs2"> \
                                        <input type="text" name="width" class="form-control entry-box3" value="0.5cm" placeholder="Enter Width"> \
                                    </div> \
                                </div> \
                                <div class="file-log"> \
                                    <div class="view-attrs2 no-edit-attr-shapes">Height</div> \
                                    <div class="view-attrs2"> \
                                        <input type="text" name="height" class="form-control entry-box3" value="0.5cm" placeholder="Enter Height"> \
                                    </div> \
                                </div>\
                                <div class="panel-buttons" id="shape-del"> \
                                    <button type="button" class="btn btn-secondary btn-del-shape">Delete Shape</button> \
                                </div> \
                                <div class="panel-buttons" id="submit-shape-add"> \
                                    <input type="submit" value="Save Shape" class="btn btn-secondary" id="btn-submit-shape"> \
                                </div> \
                            </div> \
                        </div>');
            
        // input is wrapped in a form that communicates with the backend server
        if ($(this).attr("id") == "shape-add-btn2") {
            // remove the shape selector and form for the create panel
            tempdiv = tempcontent;
            tempdiv.addClass("create-shape-create");
            try {
                tempdiv.find("#mySelect").remove();
                tempdiv.find(".rect-shape").find("#btn-submit-shape").remove();
            }
            catch (e) {
                console.log(e.message);
            }
        }
        else {
            tempdiv.append(tempcontent);
        }
        
        // insert the new shape panel before the button
        tempdiv.insertBefore($(this).parent());
        
        // add the file names to the image selector
        $('.image-select2 option').remove();
        for (let file of files) {
            $('.image-select2').append(
                $('<option/>').text(file.name)
            );
        }
    });

    $(document).on('change', '.shape', function() {
        let tempdiv = undefined;
        
        // for each shape add it's own fields
        if ($(this).val() == "Rectangle") {
            tempdiv = $('<div class="file-log rect-shape"> \
                            <div class="fields"> \
                                <div class="view3" id="attr-name">Attribute</div> \
                                <div class="view3" id="attr-value">Value</div> \
                            </div> \
                            <div class="file-log">\
                                <div class="view-attrs2 no-edit-attr-shapes">x</div>\
                                <div class="view-attrs2"> \
                                    <input type="text" name="x" class="form-control entry-box3" value="0.5cm" placeholder="Enter x"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">y</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" name="y" class="form-control entry-box3" value="0.5cm" placeholder="Enter y"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">Width:</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" name="width" class="form-control entry-box3" value="0.5cm" placeholder="Enter Width"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">Height:</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" name="height" class="form-control entry-box3" value="0.5cm" placeholder="Enter Height"> \
                                </div> \
                            </div>\
                            <div class="panel-buttons" id="shape-del"> \
                                <button type="button" class="btn btn-secondary btn-del-shape">Delete Shape</button> \
                            </div> \
                            <div class="panel-buttons" id="submit-shape-add"> \
                                <input type="submit" value="Save Shape" class="btn btn-secondary" id="btn-submit-shape"> \
                            </div> \
                        </div>');
        }

        else if ($(this).val() == "Circle") {
            tempdiv = $('<div class="file-log circle-shape"> \
                            <div class="fields"> \
                                <div class="view3" id="attr-name">Attribute</div> \
                                <div class="view3" id="attr-value">Value</div> \
                            </div> \
                            <div class="file-log">\
                                <div class="view-attrs2 no-edit-attr-shapes">cx</div>\
                                <div class="view-attrs2"> \
                                    <input type="text" name="cx" class="form-control entry-box3" value="0.5cm" placeholder="Enter cx"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">cy</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" name="cy" class="form-control entry-box3" value="0.5cm" placeholder="Enter cy"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">r</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" name="r" class="form-control entry-box3" value="0.5cm" placeholder="Enter r"> \
                                </div> \
                            </div> \
                            <div class="panel-buttons" id="shape-del"> \
                                <button type="button" class="btn btn-secondary btn-del-shape">Delete Shape</button> \
                            </div> \
                            <div class="panel-buttons" id="submit-shape-add"> \
                                <input type="submit" value="Save Shape" class="btn btn-secondary" id="btn-submit-shape"> \
                            </div> \
                        </div>');
        }

        else if ($(this).val() == "Path") {
            tempdiv = $('<div class="file-log path-shape"> \
                            <div class="fields"> \
                                <div class="view3" id="attr-name">Attribute</div> \
                                <div class="view3" id="attr-value">Value</div> \
                            </div> \
                            <div class="file-log">\
                                <div class="view-attrs2 no-edit-attr-shapes">Path Data</div>\
                                <div class="view-attrs2"> \
                                    <input type="text" name="d" class="form-control entry-box3" value="M200,300 L400,50 L600,300 L800,550 L1000,300" placeholder="Enter Data"> \
                                </div> \
                            </div> \
                            <div class="panel-buttons" id="shape-del"> \
                                <button type="button" class="btn btn-secondary btn-del-shape">Delete Shape</button> \
                            </div> \
                            <div class="panel-buttons" id="submit-shape-add"> \
                                <input type="submit" value="Save Shape" class="btn btn-secondary" id="btn-submit-shape"> \
                            </div> \
                        </div>');
        }
        else if ($(this).val() == "none") {
            console.log("Shape switched to none choice.");
            return;
        }

        if (!tempdiv) {
            console.log("ERROR: Failed to add a shape to the selected image due to undefined JQuery Object.");
            alert("Failed to add a shape to the selected image.")
            return;
        }
        
        // remove submit button if in the create panel
        if ($(this).parent().hasClass("create-shape-create")) {
            tempdiv.find("#btn-submit-shape").remove();
        }

        // if a shape already exists in this context (will always happen), then replace it with tempdiv
        // replaceWith tempdiv[0] is used because tempdiv is a jquery object that we want to replace
        // am html element, first index element in a jquery object is html element
        if ($(this).parent()[0].querySelector(".rect-shape")) {
            $(this).parent()[0].querySelector(".rect-shape").replaceWith(tempdiv[0]);
        }
        else if ($(this).parent()[0].querySelector(".circle-shape")) {
            $(this).parent()[0].querySelector(".circle-shape").replaceWith(tempdiv[0]);
        }
        else if ($(this).parent()[0].querySelector(".path-shape")){
            $(this).parent()[0].querySelector(".path-shape").replaceWith(tempdiv[0]);
        }
        else {
            $(this).parent().append(tempdiv);
        }
    });

    $(document).on('change', '#image', function() {
        // change the image displayed based on the selected file
        try {
            let myFile = getFile("#image");

            $("#log2img")
                .attr("src", myFile.name)
                .attr("alt", myFile.name);
            
            // remove all the shape panels inside the summary table
            $("#inside-table").children(".file-log").each(function () {
                $(this).remove();
            });
            
            // delete the attributes/shape list from svg view
            $("#btn-hide").trigger("click");

            createSVGView(myFile);
        }
        catch (e) {
            console.log("ERROR: Failed to display image." + e.message)
        }
    });

    $(document).on('click', '.btn-del-shape', function () {
        try {
            let tempdiv = $(this).parent();

            // assign the shape that occupies this button to tempdiv
            while (!tempdiv.hasClass("new-shape")) {
                tempdiv = tempdiv.parent();
            }

            tempdiv.remove();
        }
        catch (e) {
            console.log("ERROR: Couldn't delete shape; " + e.message);
            alert("Failed to remove shape due to an undefined shape panel.");
        }
    });

    $('#uploadForm').submit('/upload', function (action) {
        action.preventDefault();
        
        // check if the uploaded file is of type svg
        let fileName = $('[name="uploadFile"]')[0].files[0].name;
        if(!fileName.endsWith('.svg')) {
            alert(`Invalid file type: ${fileName} isn't an SVG file.`);
            return;
        }

        // check if the uploaded file name already exists
        for (file of files) {
            // alert the user if the file already exists and dont proceed 
            // with the request
            if (file.name == fileName) {
                alert(`${fileName} already exists on the server, please rename the file before reupload.`);
                return;
            }
        }

        // if the file is unique then unbind this listener from
        // the form which will result in the request being forwarded to the server
        $(this).unbind('submit').submit();
    });
    
    $('#create-svg-form').submit('/create', function (action) {
        action.preventDefault();
        
        // check if the given file is an svg image
        let fileName = $($(action.target).find('[name="name"]')[0]).val();
        if(!fileName.endsWith('.svg')) {
            alert(`Invalid file type: ${fileName} isn't an SVG file.`);
            return;
        }

        // check if the title and desc go over the character limit we want 255
        let title = $($(action.target).find('[name="title"]')[0]).val();
        let desc = $($(action.target).find('[name="descr"]')[0]).val();
        if(!checkTitleDesc(title, desc)) {
            return;
        }
        
        // check if the file name already exists on the server, otherwise
        // give the user an alert
        for (file of files) {
            if (file.name == fileName) {
                alert(`${fileName} already exists on the server, please rename the file before reupload.`);
                return; // end the request with an alert
            }
        }

        // if the file is unique then unbind this listener from
        // the form which will result in the request being forwarded to the server
        $(this).unbind('submit').submit();
    });

    function checkTitleDesc (title, descr) {
        try {
            // alert the user if the max number of characters exceeded in title or descr
            if (title.length > 255) {
                alert('ERROR: Title exceeded maximum number of characters (255).')
                return false;
            }
            
            if (descr.length > 255) {
                alert('ERROR: Description exceeded maximum number of characters (255).')
                return false;
            }
        }
        catch(e) {
            console.log(e.message);
            alert('Internal error: failed to validate input.');
            return false;
        }

        return true;
    }
    
    $(document).on('click', '#scale-btn', function () {
        try {
            // make a dropdown list for the image that will be scaled and input
            // text boxes for the scale factor
            let tempdiv = $('\
            <form id="scale-form" action="/scale-shape-form" method="post" encType="multipart/form-data"> \
                    <div class="scale-shape-class"> \
                        <label>Select Image</label> \
                        <select id="image3" class="image-select" name="Image"> \
                        </select>\
                        <div class="file-log"> \
                            <div class="fields"> \
                                <div class="view3" id="attr-name">Type</div> \
                                <div class="view3" id="attr-value">Scale Factor</div> \
                            </div> \
                            <div class="file-log">\
                                <div class="view-attrs2 no-edit-attr-shapes">Rectangle</div>\
                                <div class="view-attrs2"> \
                                    <input type="text" name="rects" class="form-control entry-box4" value="1" placeholder="Enter Scale Factor"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">Circle</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" name="circs" class="form-control entry-box4" value="1" placeholder="Enter Scale Factor"> \
                                </div> \
                            </div> \
                            <div class="panel-buttons" id="submit-shape-scale"> \
                                <input type="submit" value="Save Shapes" class="btn btn-secondary" id="btn-scale-shape"> \
                            </div> \
                        </div> \
                    </div>\
                </form>');
                
            // add the scale div to the edit log section
            $('#edit-panel').append(tempdiv);
            $('#scale-btn').css("background-color", "#A80A01");
            $('#scale-btn').text("Discard Scaling");
            $('#scale-btn').attr("id", "scale-hide"); 

            // add the file names to the image selector
            for (let file of files) {
                $('#image3').append(
                    $('<option/>').text(file.name)
                );
            }

            $('#scale-form')[0].addEventListener('submit', function (action) {
                action.preventDefault();
                
                try {
                    // check if the scale factors are numbers
                    let rectScale = $($(action.target).find('[name="rects"]')[0]).val();
                    let circScale = $($(action.target).find('[name="circs"]')[0]).val();
                    
                    // give an error alert to the user if scale factors are invalid
                    if (!rectScale || rectScale.length <= 0 || isNaN(rectScale)) {
                        alert(`ERROR: Rectangle scale factor is not a valid number.`);
                        return;
                    }
                    if (!circScale || circScale.length <= 0 || isNaN(circScale)) {
                        alert(`ERROR: Circle scale factor is not a valid number.`);
                        return;
                    }
                }
                catch (e) {
                    console.log(e.message);
                    alert('Internal error: failed to validate input.');
                    return;
                }

                $(this).unbind('submit').submit();
            });
        }
        catch (e) {
            console.log(e.message);
            alert("Failed to display the shape scaling panel.");
        }
    });

    $(document).on('click', '#scale-hide', function () {
        $('#scale-form').remove();
        $('#scale-hide').css("background-color", "#9147ff");
        $('#scale-hide').text("Scale Shapes");
        $('#scale-hide').attr("id", "scale-btn");
    });
});