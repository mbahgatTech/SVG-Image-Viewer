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
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

});