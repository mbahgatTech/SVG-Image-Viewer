// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            data1: "Value 1",
            data2:1234.56
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    // append all the files in /uploads to file log
    let files = [];
    $.ajax ({
        type: 'get',
        dataType: 'file',
        url: '/upload/:name',
        data: {"name":"quad01.svg"},
        success: function(data) {
            console.log(data);
        },
        fail: function(error) {
            console.log(error);
            alert("Failed to fetch SVG images from the server.");
        } 
    });

    // initialize the information of the current file
    // that will fill our table
    let names = ["quad01.svg", "rects.svg", "Emoji_poo.svg"];
    let size = "50Kb";
    let rects = 3;
    let circs = 4;
    let paths = 5;
    let grps = 6;
    for (let name of names) {
        // append a new div element to the file log panel containing the new image
        $('#log-panel').append(
            $('<div/>')
              .attr("id", "newDiv1")
              .addClass("file-log")
    
              // append an image with elem2 id and a link download to the svg image
              .append( 
                  $('<a/>')
                    .attr("href", name)
                    .attr("download", name)
                    .append(
                        $('<img/>')
                            .attr("src", name)
                            .addClass("fieldElems")
                            .attr("id", "elem2")
                    )
                )
    
                // append a text element with name of the svg file and a download link
                .append(
                    $("<a/>")
                        .addClass("fieldElems")
                        .text(name)
                        .attr("href", name)
                        .attr("download", name)
                )
    
                // append a text elements with the remaining information about the image
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(size)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(rects)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(circs)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(paths)
                )
                .append(
                    $("<div/>")
                        .addClass("fieldElems")
                        .text(grps)
                )
        );
    }

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('.entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

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
            console.log($('#' + newId).children);    
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

    let shapes = [];
    let arr = ["Rectangle", "Rectangle", "Circle", "Circle", "Path"];
    let attributes = [];

    for (let i = 0; i < arr.length; i++) {
        // each shape has its own unique index and its own set of attributes
        // type element will help  add a class for the listeners
        let shape = {};
        shape.type = arr[i];
        shape.index = i;
        
        if (shape.type == "Path") {
            attributes = [{"name":"Data","value":"M300 L500 R30"}, {"name":"fill","value":"black"}];
        }
        else if (shape.type == "Rectangle"){
            attributes = [{"name":"x","value":"2cm"}, {"name":"y","value":"2cm"}];
        }
        else if (shape.type == "Circle"){
            attributes = [{"name":"cx","value":"2cm"}, {"name":"cy","value":"2cm"}];
        }

        shape.attributes = [...attributes];
        shapes.push(shape);
    }

    $(document).on('click', "#btn-show", function() {
        $('#btn-show').attr("value", "Hide Attributes")
                      .attr("id", "btn-hide");
        
        $('#view-panel').append ($('<div/>')
                        .attr("id", "shape-log")
                        .addClass("file-log")
        );

        for (shape of shapes) {
            console.log(shape.type);
            $('#shape-log').append ($('<h5/>').text(shape.type + " " + shape.index));

            // append an attributes panel that consists of a field labels
            appendAttributes("shape-log", `${shape.type}${shape.index}`, "view-attrs no-edit-attr", "form-control entry-box2", shape);
        }
        $('#shape-log').append (
            $("<div/>")
                .addClass("panel-buttons")
                .attr("id", "save")
                .append(
                    $("<input/>")
                        .addClass("btn btn-secondary")
                        .attr("type", "submit")
                        .attr("value", "Save Changes")
                        .attr("id", "sbmt-attr")
                )
        );
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
        tempdiv.insertBefore($(this).parent());
        addListeners("#btn-hide", ".entry-box2");

        // change hide button to discard
        $('#btn-hide')
            .attr("value", "Discard")
            .css("background-color", "#A80A01");

        console.log("Adding Default Attribute: Success.");
    });
    
    $(document).on('click', "#sbmt-attr", function() {
        // loop through all the  attributes and add them to a JSON string
        let count2 = 0;
        let staticAttrs = document.querySelectorAll(".no-edit-attr");
        let attr = {};
        let shape = {};
        for (shape of shapes) {
            shape.attributes = [];
            $(".entry-box2").each( function() {
                // check which shape has the button field belongs too
                console.log($(this).parent().parent().parent().attr("id"));
                if (!($(this).parent().parent().parent().attr("id") == (shape.type + shape.index))) {
                    return;
                }
    
                // get the name of the current attribute
                if ($(this).attr('placeholder') == "Enter Attribute"){
                    attr.name = $(this)[0].value;
                }
                // get the value of the current attribute
                else if ($(this).attr('placeholder') == "Enter Value") {
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
                    console.log( JSON.stringify(attr));
    
                    // update the JSON array of attributes with the new attr
                    shape.attributes.push(attr);
                    attr = {};
                }
            }); 
        }

        // change the discard button to hide attributes
        $('#btn-hide').attr("value", "Hide Attributes")
                      .css("background-color", "#9147ff");
    });
    
    $(".shape-add").click(function() {
        // make new dropboxes for the user to select the image and the type of
        // shape they want to add to the selected image
        let tempdiv = $('<form id="add-form" action="/add-shape-form" method="post" encType="multipart/form-data"></form>');
        let tempcontent = $('<div class="new-shape"> \
                            <div id="mySelect"> \
                                <label>Select Image</label> \
                                <select id="image2" name="Image"> \
                                    <option value="rects.svg">rects.svg</option> \
                                    <option value="quad01.svg">quad01.svg</option> \
                                    <option value="Emoji_poo.svg">Emoji_poo.svg</option> \
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
                                        <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter x"> \
                                    </div> \
                                </div> \
                                <div class="file-log"> \
                                    <div class="view-attrs2 no-edit-attr-shapes">y</div> \
                                    <div class="view-attrs2"> \
                                        <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter y"> \
                                    </div> \
                                </div> \
                                <div class="file-log"> \
                                    <div class="view-attrs2 no-edit-attr-shapes">Width</div> \
                                    <div class="view-attrs2"> \
                                        <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter Width"> \
                                    </div> \
                                </div> \
                                <div class="file-log"> \
                                    <div class="view-attrs2 no-edit-attr-shapes">Height</div> \
                                    <div class="view-attrs2"> \
                                        <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter Height"> \
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
        tempdiv.insertBefore($(this).parent())
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
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter x"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">y</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter y"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">Width:</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter Width"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">Height:</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter Height"> \
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
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter cx"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">cy</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter cy"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">r</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box3" value="0.5cm" placeholder="Enter r"> \
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
                                    <input type="text" class="form-control entry-box3" value="M200,300 L400,50 L600,300 L800,550 L1000,300" placeholder="Enter Data"> \
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
            $("#log2img").attr("src", $("#image option:selected").text());
        }
        catch (e) {
            console.log("ERROR: Failed to display image.")
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

    $(document).on('click', '#scale-btn', function () {
        try {
            // make a dropdown list for the image that will be scaled and input
            // text boxes for the scale factor
            let tempdiv = $('\
            <form id="scale-form" action="/scale-shape-form" method="post" encType="multipart/form-data"> \
                    <div class="scale-shape-class"> \
                        <label>Select Image</label> \
                        <select id="image3" name="Image"> \
                            <option value="rects.svg">rects.svg</option> \
                            <option value="quad01.svg">quad01.svg</option> \
                            <option value="Emoji_poo.svg">Emoji_poo.svg</option> \
                        </select>\
                        <div class="file-log"> \
                            <div class="fields"> \
                                <div class="view3" id="attr-name">Type</div> \
                                <div class="view3" id="attr-value">Scale Factor</div> \
                            </div> \
                            <div class="file-log">\
                                <div class="view-attrs2 no-edit-attr-shapes">Rectangle</div>\
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box4" value="1" placeholder="Enter Scale Factor"> \
                                </div> \
                            </div> \
                            <div class="file-log"> \
                                <div class="view-attrs2 no-edit-attr-shapes">y</div> \
                                <div class="view-attrs2"> \
                                    <input type="text" class="form-control entry-box4" value="1" placeholder="Enter Scale Factor"> \
                                </div> \
                            </div> \
                            <div class="panel-buttons" id="submit-shape-scale"> \
                                <input type="submit" value="Save Shape" class="btn btn-secondary" id="btn-scale-shape"> \
                            </div> \
                        </div> \
                    </div>\
                </form>');
            
            // add the scale div to the edit log section
            $('#edit-panel').append(tempdiv);
            $('#scale-btn').css("background-color", "#A80A01");
            $('#scale-btn').text("Discard Scaling");
            $('#scale-btn').attr("id", "scale-hide"); 
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