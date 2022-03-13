'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

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
    
    // declare the file to json function call from the c parser
    let fileData = ffi.Library('./libsvgparser', {
      'fileToJSON': ['string', ['string']],
      'getTitle': ['string', ['string']],
      'getDesc': ['string', ['string']]
    });

    // append all the file information about
    files.forEach(function (file) {
      if (!file || path.extname(file) != '.svg') {
        return;
      }

      let currFile = {"name":"","size":""};
      currFile.name = file;

      // add the file size to the list
      let stats = fs.statSync(__dirname + '/uploads/' + file);
      currFile.size = stats.size + "KB";

      // get the svg file properties from the c API
      let otherData = fileData.fileToJSON("uploads/" + file);
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
      currFile.rectsAttrsList = otherData.rectsAttrsList;
      currFile.circsAttrsList = otherData.circsAttrsList;
      currFile.pathsAttrsList = otherData.pathsAttrsList;
      currFile.groupsAttrsList = otherData.groupsAttrsList;
      console.log(currFile.groupsAttrsList);

      // add title and desc to the JSON object
      currFile.title = fileData.getTitle("uploads/" + currFile.name);
      currFile.desc = fileData.getDesc("uploads/" + currFile.name);

      fileObjs.push(currFile);
    });
    
    // respond with the list of JSON objects containing file names and sizes
    res.status(200).send(fileObjs);
  });
  
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);