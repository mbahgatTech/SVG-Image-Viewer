'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const bodyParser = require('body-parser');
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.env.PORT || 5000;

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

const fileData = ffi.Library('./libsvgparser', {
  'fileToJSON': ['string', ['string']],
  'getTitle': ['string', ['string']],
  'getDesc': ['string', ['string']],
  'createShape':['bool', ['string', 'string', 'string']],
  'createShapeinGroup':['bool', ['string', 'string', 'string', 'int']],
  'scaleShapes':['bool', ['float', 'float', 'string']],
  'JSONtoSVGFile':['bool', ['string', 'string']],
  'appendAttributeToFile':['bool', ['string', 'string', 'int', 'string']],
  'appendAttributeToGroup':['bool', ['string', 'string', 'int', 'int', 'string']]
});

// an end point that sends all the files in uploads as JSON objects 
// containing all the image information and the 
app.get('/get-files', function(req , res){
  let fileObjs = [];
  let dirPath = path.join(__dirname, 'uploads');
  fs.readdir(dirPath, (err, files) => {
    // respond with code 500 if error is encountered reading the dir
    if (err) {
      return res.status(500).send(err);
    }

    // append all the file information about
    files.forEach(function (file) {
      if (!file || path.extname(file) != '.svg') {
        fs.unlinkSync("uploads/" + file);
        return;
      }

      let currFile = {"name":"","size":""};
      currFile.name = file;

      // add the file size to the list
      let stats = fs.statSync(__dirname + '/uploads/' + file);
      currFile.size = Math.round(parseFloat(stats.size) / 1024.0) + "KB";

      // get the svg file properties from the c API
      let otherData = fileData.fileToJSON("uploads/" + file);
      if (!otherData) {
        fs.unlinkSync("uploads/" + file);
        return;
      }

      try {
        otherData = JSON.parse(otherData);
      }
      catch (err2) {
        return res.status(500).send(err2);
      }
      
      // add the parsed data to currFile and append the JSON obj to fileObjs
      currFile.rects = otherData.numRect;
      currFile.circs = otherData.numCirc;
      currFile.paths = otherData.numPaths;
      currFile.groups = otherData.numGroups;
      currFile.rectList = otherData.rects;
      currFile.circList = otherData.circs;
      currFile.pathList = otherData.paths;
      currFile.groupList = otherData.groups;

      // add the attrs lists to the the file
      currFile.rectsAttrsList = otherData.rectsAttrsList;
      currFile.circsAttrsList = otherData.circsAttrsList;
      currFile.pathsAttrsList = otherData.pathsAttrsList;
      currFile.groupsAttrsList = otherData.groupsAttrsList;
      currFile.svgAttrs = otherData.svgAttrs;

      // add title and desc to the JSON object
      currFile.title = fileData.getTitle("uploads/" + currFile.name);
      currFile.descr = fileData.getDesc("uploads/" + currFile.name);

      fileObjs.push(currFile);
    });
    
    // respond with the list of JSON objects containing file names and sizes
    return res.status(200).send(fileObjs);
  });
  
});


app.post('/post-attrs', function (req, res) {
  let file = req.body.file;
  try {
    let fileCreate = fileData.JSONtoSVGFile(JSON.stringify(file), "uploads/" + req.body.file.name);
    if (!fileCreate) {
      return res.status(400).send('Invalid input.');
    }
    file = req.body.file;
    
    // get the number of components across all groups in the svg struct
    let groupRects = 0, groupCircs = 0, groupPaths = 0; 
    file.groupList.forEach((group) => {
      groupRects += group.rects;
      groupCircs += group.circs;
      groupPaths += group.paths;
    });
  
    // compute the total amount of components at the svg level
    let maxRects = Math.max(0, file.rectList.length - groupRects);
    let maxCircs = Math.max(0, file.circList.length - groupCircs);
    let maxPaths = Math.max(0, file.pathList.length - groupPaths);

    // call create shape on each rectangle object and add them to the file
    file.rectList.forEach( (rect, index) => {
      if (index >= maxRects) {
        return;
      }

      // add rectangle to the file and check the response for errors
      let response = fileData.createShape("RECT", JSON.stringify(rect), "uploads/" + file.name);
      if (!response) {
        throw ('Invalid Rectangle core attribute input.');
      }
      
      // add the attributes to the current rect
      file.rectsAttrsList[index].forEach((attrs) => {
        response = fileData.appendAttributeToFile("RECT", JSON.stringify(attrs), index, "uploads/" + file.name);
        if(!response) {
          throw ('Invalid Rectangle attributes input.');
        }
      });
    });

    // create all circles and add them to the svg file
    file.circList.forEach((circ, index) => {
      if (index >= maxCircs) {
        return;
      }

      let response = fileData.createShape("CIRC", JSON.stringify(circ), "uploads/" + file.name);
      if (!response) {
        throw ('Invalid Circle core attribute input.');
      }

      // add the attributes to the current circ
      file.circsAttrsList[index].forEach((attrs) => {
        response = fileData.appendAttributeToFile("CIRC", JSON.stringify(attrs), index, "uploads/" + file.name);
        if(!response) {
          throw ('Invalid Circle attributes input.');
        }
      });
    });

    // create all paths and add them to the svg file
    file.pathList.forEach((path, index) => {
      if (index >= maxPaths) {
        return;
      }

      let response = fileData.createShape("PATH", path.d, "uploads/" + file.name);
      if (!response) {
        throw ('Invalid path data input.');
      }

      // add the attributes to the current path 
      file.pathsAttrsList[index].forEach((attrs) => {
        if (!attrs || attrs.val === 'd' || attrs.val === 'data') {
          return;
        }
        
        response = fileData.appendAttributeToFile("PATH", JSON.stringify(attrs), index, "uploads/" + file.name);
        if(!response) {
          throw ('Invalid Path attributes input.');
        }
      });
    });
    
    // create all groups and add them to the svg file
    file.groupList.forEach((group, index) => {
      let response = fileData.createShape("GROUP", "", "uploads/" + file.name);
      if (!response) {
        throw ('Failed to create group.');
      }

      // add the attributes to the current path 
      file.groupsAttrsList[index].forEach((attrs) => {
        response = fileData.appendAttributeToFile("GROUP", JSON.stringify(attrs), index, "uploads/" + file.name);
        if(!response) {
          throw ('Invalid Group attributes input.');
        }
      });

      // append the remaining components not added to the svg in these structs
      for (let i = maxRects; (i - maxRects) < group.rects; i++) {
        response = fileData.createShapeinGroup("RECT", JSON.stringify(file.rectList[i]), "uploads/" + file.name, index);
        if (!response) {
          throw ('Invalid Rectangle core attribute input.');
        }

        // add the attributes to the current rect
        file.rectsAttrsList[i].forEach((attrs) => {
          response = fileData.appendAttributeToGroup("RECT", JSON.stringify(attrs), i - maxRects, index, "uploads/" + file.name);
          if(!response) {
            throw ('Invalid Rectangle attributes input.');
          }
        });
      }
      maxRects += group.rects;

      for (let i = maxCircs; (i - maxCircs) < group.circs; i++) {
        response = fileData.createShapeinGroup("CIRC", JSON.stringify(file.circList[i]), "uploads/" + file.name, index);
        if(!response) {
          throw ('Invalid Circle core attribute input.');
        }

        // add the attributes to the current circ
        file.circsAttrsList[i].forEach((attrs) => {
          response = fileData.appendAttributeToGroup("CIRC", JSON.stringify(attrs), i - maxCircs, index, "uploads/" + file.name);
          if(!response) {
            throw ('Invalid Circle attributes input.');
          }
        });
      }
      maxCircs += group.circs;

      for (let i = maxPaths; (i - maxPaths) < group.paths; i++) {
        response = fileData.createShapeinGroup("PATH", file.pathList[i].d, "uploads/" + file.name, index);
        if(!response) {
          throw ('Invalid path data input.');
        }

        // add the attributes to the current path 
        file.pathsAttrsList[i].forEach((attrs) => {
          if (attrs.val === 'd' || attrs.val === 'data') {
            return;
          }

          response = fileData.appendAttributeToGroup("PATH", JSON.stringify(attrs), i - maxPaths, index, "uploads/" + file.name, );
          if(!response) {
            throw ('Invalid Path attributes input.');
          }
        });
      }
      maxPaths += group.paths;
    });

    // add all the svg attributes to the svg image
    file.svgAttrs.forEach((attr) => {
      let response = fileData.appendAttributeToFile("SVG", JSON.stringify(attr), 0, "uploads/" + file.name);
      if(!response) {
        throw ('SVG Attributes are invalid.');
      }
    });
  }
  catch (errr) {
    console.log(errr.message);
    return res.status(400).send('Setting attributes failed due to invalid input. ' + errr);
  }
  
  return res.status(200).send(file);
});

app.post('/scale-shape-form', function (req, res) {  
  // scale the shapes for the given file
  let response = fileData.scaleShapes(req.body.rects, req.body.circs, "uploads/" + req.body.Image);
  if (!response) {
    res.status(400);
  }

  res.redirect('/');
}); 

app.post('/add-shape-form', function (req, res) { 
  let jsonObj = {};
  let type = '';

  // populate the json object with the shape data
  if (req.body.shape === 'Rectangle') {
    type = 'RECT';
    jsonObj.x = req.body.x;
    jsonObj.y = req.body.y;
    jsonObj.w = req.body.width;
    jsonObj.h = req.body.height;
    jsonObj.units = req.body.units;
  }
  else if (req.body.shape === 'Circle') {
    type = 'CIRC';
    jsonObj.cx = req.body.cx;
    jsonObj.cy = req.body.cy;
    jsonObj.r = req.body.r;
    jsonObj.units = req.body.units;
  }
  else if (req.body.shape === 'Path') {
    type = 'PATH';
    jsonObj.d = req.body.d;
  }
  
  // append the shape to the given file
  if (type === 'PATH') {
    fileData.createShape(type, jsonObj.d, "uploads/" + req.body.Image);
  }
  else {
    fileData.createShape(type, JSON.stringify(jsonObj), "uploads/" + req.body.Image);
  }
  res.redirect('/');
});

app.post('/create', function (req, res) { 
  // initialize the file with the title and description
  let svgJSON = {"title":req.body.title,"descr":req.body.descr};
  fileData.JSONtoSVGFile(JSON.stringify(svgJSON), "uploads/" + req.body.name);
  svgJSON = {};
  
  if (!req.body.shape) {
    return res.redirect('/');
  }

  // add the shapes(s) to the svg file and redirect the user to home page
  if ((typeof req.body.shape) === 'object') {
    if(req.body.x) {
      if ((typeof req.body.x) === 'object') {
        // loop through all rectangle inputs and add them to the svg image
        for(let i = 0; i < req.body.x.length; i++) {
          svgJSON.x = req.body.x[i];
          svgJSON.y = req.body.y[i];
          svgJSON.w = req.body.width[i];
          svgJSON.h = req.body.height[i];
          svgJSON.units = req.body.units[i];

          fileData.createShape("RECT", JSON.stringify(svgJSON), "uploads/" + req.body.name);
          svgJSON = {};
        }
      }
      else {
        // only one rectangle exists so add that to the image
        svgJSON.x = req.body.x;
        svgJSON.y = req.body.y;
        svgJSON.w = req.body.width;
        svgJSON.h = req.body.height;
        svgJSON.units = req.body.units;

        fileData.createShape("RECT", JSON.stringify(svgJSON), "uploads/" + req.body.name);
        svgJSON = {};
      }
    }

    if(req.body.cx) {
      if ((typeof req.body.cx) === 'object') {
        // loop through all circle inputs and add them to the svg image
        for(let i = 0; i < req.body.cx.length; i++) {
          svgJSON.cx = req.body.cx[i];
          svgJSON.cy = req.body.cy[i];
          svgJSON.r = req.body.r[i];
          svgJSON.units = req.body.units[i];

          fileData.createShape("CIRC", JSON.stringify(svgJSON), "uploads/" + req.body.name);
          svgJSON = {};
        }
      }
      else {
        // only one circle exists so add that to the image
        svgJSON.cx = req.body.cx;
        svgJSON.cy = req.body.cy;
        svgJSON.r = req.body.r;
        svgJSON.units = req.body.units;

        fileData.createShape("CIRC", JSON.stringify(svgJSON), "uploads/" + req.body.name);
        svgJSON = {};
      }
    }

    if(req.body.d) {
      if ((typeof req.body.d) === 'object') {
        // loop through all path inputs and add them to the svg image
        for(let i = 0; i < req.body.d.length; i++) {
          svgJSON.d = req.body.d[i];

          fileData.createShape("PATH", svgJSON.d, "uploads/" + req.body.name);
          svgJSON = {};
        }
      }
      else {
        // only one path exists so add that to the image
        fileData.createShape("PATH", req.body.d, "uploads/" + req.body.name);
        svgJSON = {};
      }
    }

    return res.redirect('/');
  }
  
  let type = '';
  // populate the json object with the shape data
  if (req.body.shape === 'Rectangle') {
    type = 'RECT';
    svgJSON.x = req.body.x;
    svgJSON.y = req.body.y;
    svgJSON.w = req.body.width;
    svgJSON.h = req.body.height;
    svgJSON.units = req.body.units;
  }
  else if (req.body.shape === 'Circle') {
    type = 'CIRC';
    svgJSON.cx = req.body.cx;
    svgJSON.cy = req.body.cy;
    svgJSON.r = req.body.r;
    svgJSON.units = req.body.units;
  }
  else if (req.body.shape === 'Path') {
    type = 'PATH';
    svgJSON.d = req.body.d;
  }

  fileData.createShape(type, JSON.stringify(svgJSON), "uploads/" + req.body.name);

  res.redirect('/');
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);