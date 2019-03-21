var http = require('http');
var path = require('path');
var fs = require('fs');
 
//these are the only file types we will support for now
extensions = {
  ".html" : "text/html",
  ".css" : "text/css",
  ".xml" : "text/xml",
  ".js" : "application/javascript",
  ".png" : "image/png",
  ".gif" : "image/gif",
  ".jpg" : "image/jpeg",
  ".doc" : "application/doc"
};
 
function getFile(filePath, res, page404, mimeType){
  fs.exists(filePath,function(exists){
    if(exists){
      fs.readFile(filePath,function(err,contents){
        if(!err){
          res.writeHead(200,{
            "Content-type" : mimeType,
            "Content-Length" : contents.length
          });
          res.end(contents);
        } else {
          console.dir(err);
        };
      });
    } else {
      fs.readFile(page404,function(err,contents){
        if(!err){
          res.writeHead(404, {'Content-Type': 'text/html'});
          res.end(contents);
        } else {
          console.dir(err);
        };
      });
    };
  });
};
 
function requestHandler(req, res) {
  var fileName = path.basename(req.url) || 'index.html';
  var ext = path.extname(fileName);
  var localFolder = __dirname + '/public/';
  var page404 = localFolder + '404.html';
  var flag = 0;
 
  getFile((localFolder + fileName),res,page404,extensions[ext]);
};
 
//step 2) create the server
http.createServer(requestHandler)
 
//step 3) listen for an HTTP request on port 3000
.listen(3000);