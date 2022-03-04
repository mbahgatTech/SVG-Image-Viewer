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

    let attributes = [{"name":"Attribute 1", "value":"Data Value"},{"name":"Attribute 2","value":"Data Value"}];
    $(document).on('click', "#btn-show", function() {
        $('#btn-show').attr("value", "Hide Attributes")
                      .attr("id", "btn-hide")

        // append an attributes panel that consists of a field labels
        $('#view-panel').append (
            $("<div/>")
                .addClass("file-log")
                .attr("id", "attrs-log")
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
        )
        
        // loop through all the attributes and append their names and values 
        // to their respective fields
        for (let attr of attributes) {
            $('#attrs-log')
            .append (
                $("<div/>")
                .addClass("file-log")
                .append (
                    $("<div/>")
                        .addClass("view-attrs no-edit-attr")
                        .text(attr.name)
                )
                // append the value as a text box that can be edited with user content
                .append (
                    $("<div/>")
                        .addClass("view-attrs")
                        .attr("id", "data")
                        .append (
                            $("<input/>")
                                .addClass("form-control entry-box2")
                                .attr("type", "text")
                                .attr("value", attr.value)
                                .attr("placeholder", "Enter Value")
                        )
                )
            )
        }
        
        // append buttons after all attributes have been added
        $('#attrs-log').append (
            $("<div/>")
                .addClass("panel-buttons")
                .attr("id", "add")
                .append(
                    $("<input/>")
                        .addClass("btn btn-secondary")
                        .attr("type", "submit")
                        .attr("value", "Add Attribute")
                        .attr("id", "btn-add")
                )
        )
        .append (
            $("<div/>")
                .addClass("panel-buttons")
                .attr("id", "add")
                .append(
                    $("<input/>")
                        .addClass("btn btn-secondary")
                        .attr("type", "submit")
                        .attr("value", "Save Changes")
                        .attr("id", "sbmt-attr")
                )
        )
        
        console.log('Showing all attributes');
    });
    
    $(document).on('click', "#btn-hide", function() {
        $('#attrs-log').remove();
        $('#btn-hide').attr("value", "Show Attributes")
                      .attr("id", "btn-show")
                      .css("background-color", "#9147ff");
        console.log('Hid attributes');
    });

    $(document).on('click', "#btn-add", function() {
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
        tempdiv.insertBefore($("#add"));
        
        // change hide button to discard
        $('#btn-hide')
            .attr("value", "Discard")
            .css("background-color", "#A80A01");
        
        // add listeners for all entry boxes in attrs-log (class entry-box2)
        // that turn the hide button into  a discard button when input change is made 
        let container = document.querySelectorAll(".entry-box2");
        for (let i = 0; i < container.length; i++) {
            container[i].addEventListener('input', function() {
                // change the hide button box to discard because the changes will be discarded
                $('#btn-hide')
                    .attr("value", "Discard")   
                    .css("background-color", "#A80A01");
                console.log("Hide button changed to discard.");
            });
        }

        console.log("Adding Default Attribute: Success.");
    });
    
    $(document).on('click', "#sbmt-attr", function() {
        attributes = [];
        
        // loop through all the  attributes and add them to a JSON string
        let count2 = 0;
        let staticAttrs = document.querySelectorAll(".no-edit-attr");
        let attr = {};
        $(".entry-box2").each( function() {
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
                        return;
                    }
                    
                    // convert html element to a html object
                    temp = $(temp);
                    if (!temp || !temp.text()) {
                        return;
                    }
                    attr.name = temp.text();
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
                attributes.push(attr);
                count2++;
                attr = {};
            }
        }); 

        // change the discard button to hide attributes
        $('#btn-hide').attr("value", "Hide Attributes")
                      .css("background-color", "#9147ff");
    });
    
});