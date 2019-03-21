var os=require('os');
var net=require('net');
var fs = require('fs');
var http = require('http');
var path = require('path');

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

var networkInterfaces=os.networkInterfaces();
var message_file = fs.createWriteStream('./inputs/message.bin', {'flags': 'w', encoding: 'binary'});
var id_file = fs.createWriteStream('./inputs/id.txt', {'flags': 'w'});
var info_file = fs.createWriteStream('./inputs/info.txt', {'flags': 'w'});

var port = 44444;
var send_mode = 0;
var array_count = 0;

var user_name = new Array();
user_name[0] = new Array();
user_name[1] = new Array();

var sockets = [];

function sleep(ms) {
	var start = new Date().getTime();
	for(var i=0; i<1e7; i++) {
		if((new Date().getTime() - start) > ms) break;
	}
}

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
  var temp = path.basename(req.url).split("?");
  var fileName = temp[0] || 'index.html';
  var ext = path.extname(fileName);
  var localFolder = __dirname + '/history/';
  var page404 = localFolder + '404.html';
  var flag = 0;
 
  getFile((localFolder + fileName),res,page404,extensions[ext]);
};

function callback_server_connection(socket){
    var remoteAddress = socket.remoteAddress;
    var remotePort = socket.remotePort;
    socket.setNoDelay(true);
    console.log("connected: ", remoteAddress, " : ", remotePort);

    sockets.push(socket);

    socket.on('data', function (data) {
        var uint8_data = new Uint8Array(data);
        console.log("data( " + user_name[1][sockets.indexOf(socket)] + "): " + data.toString());
        console.log("data size: " + uint8_data.length);

        if (uint8_data.length == 64 || uint8_data.length == 68) { //data size is 64 || 68
          var data_str = data.toString();

            if(uint8_data.length == 68) { //data size is 68 => trash byte added case
              data_str = data_str.substring(4, (data.toString()).length);
              uint8_data = new Uint8Array(data_str);
            }

            if (uint8_data[0] == 95) { //data is started with "_"
                if (uint8_data[1] == 115) { //data is started with "_s"
                    var idx = sockets.indexOf(socket);
                    var infoStr = data.toString();

                    for (var i = 0; i < sockets.length; i++) {
                      infoStr = infoStr.substring(6, infoStr.length);

                    	if (i == idx) { //broadcast "_start" to all clients except starter
				console.log("server: sent _startx to starter");
                    		sockets[i].write("_startx");
                    	} else {
				console.log("server: broadcast _start to client(" + i + ")");
                    		sockets[i].write("_start");
                    	}
                    }
		    info_file.write(infoStr);
		    info_file.write("\n");
                } else if (uint8_data[1] == 101) { //data is started with "_e"
		   //broadcast _end to all client except the client that click "meeting end button"
		   var infoStr = data.toString();
		   infoStr = infoStr.substring(4, infoStr.length);

		   var ender = sockets.indexOf(socket);
                   for (var i = 0; i < sockets.length; i++) {
			if(i != ender) {
				console.log("server: broadcast _end to all client(" + i + ")");
	                    	sockets[i].write("_end");
	                    	sockets[i].end();                    	
			}
                    }
		    info_file.write(infoStr);
		    info_file.write("\n");
                } else if (uint8_data[1] == 109) { //data is started with "_m"
		      //receive "meeting name" from the client: "_mname\n[meeting name]"
		      var infoStr = data.toString();
		      infoStr = infoStr.substring(7, infoStr.length);

		      console.log("server: _mname(" + infoStr + ")");

		      info_file.write(infoStr);

		      info_file.write("\n");

		      //send "_end" message to ender for informing process is ended
		      socket.write("_end");
		      sleep(1000);
		      process.kill(process.pid);
		}
            } else {
            	console.log("server: user_name store");
                user_name[0][array_count] = remoteAddress.toString();
                user_name[1][array_count] = data_str;
                array_count++;

                for (var i = 0; i < array_count; i++) {
                	console.log("No.", i, " : ", user_name[0][i], " ", user_name[1][i]);
                }
            }

        } else if (uint8_data.length == 1024) {
            for (var i = 0; i < array_count; i++) {
                if (remoteAddress.toString() == user_name[0][i]) {
                    id_file.write(user_name[1][i]);
                    id_file.write("\n");
                    message_file.write(data.toString());
                }
            }
        }
    });

	    socket.on('end', function () {
	    	console.log("ended: ", remoteAddress, " : ", remotePort);
    	});
}

console.log("node.js net server is waiting:");
for (var interface in networkInterfaces) {
    networkInterfaces[interface].forEach(function(details){
        
        if ((details.family=='IPv4') && !details.internal) {
            console.log(interface, details.address);  
        }
    });
}

console.log("port: ", port);

var netServer = net.createServer(callback_server_connection);
netServer.listen(port);

http.createServer(requestHandler).listen(8080);

if (send_mode == 2) {
    process.exit();
}
