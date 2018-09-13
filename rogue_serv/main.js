const http = require('http');
const URL = require('url');
const fs = require('fs');
const st = require('st');

const port = 9001;
const mount = st({ path: "./static", url: "/static" });

const server = http.createServer((request, response) => {
	try {
		// do work
		serve(request, response);
	} catch(e) {
		// report
		console.log("[[an error ocurred]]");
		console.trace(e);
		// show user
		if (!response.finished) {
			response.write("<p>[[an error ocurred]]</p>");
			response.write(`<p>${e}</p>`);
			response.end();
		}
	}
}).listen(port, err => {
	if (err)  return console.log("[error]", err);
	console.log("running...");
});


function serve(request, response) {
	// handle the request url
	let url = URL.parse(request.url, true);
	console.log(url.pathname);

	switch (url.pathname) {
	case "/":
		fs.readFile("./static/index.html", function(err, html) {
			response.writeHead(200, {"Content-Type": "text/html"});
			response.end(html);
		});
		break;
	case "/action.json": {
		// get initial player data
		let pdata = gameplayers[0];
		// do action
		console.log(url.query);
		if (url.query.walk) {
			switch (url.query.walk) {
			case 'n':  pdata.y--;  break;
			case 's':  pdata.y++;  break;
			case 'e':  pdata.x++;  break;
			case 'w':  pdata.x--;  break;
			}
		}
		// draw map into response
		pdata.map = getmapdata(pdata);
		// return player object and position
		response.writeHead(200, {"Content-Type": "text/json"});
		response.write(JSON.stringify(pdata));
		response.end();
		} break;
	case "/favicon.ico":
		send404(response);
		break;
	default:
		// serve static file
		if (mount(request, response))  break;
		send404(response);
	};
}

function send404(response) {
	response.writeHead(404, {'Content-Type': 'text/html'});
	response.write("404: not found");
	response.end();
}


let gameplayers = [{
	id: "player1",
	x: 2,
	y: 2
}];
let gamemap = [
	"##########             ",
	"#        #             ",
	"#        ##########    ",
	"#                 #    ",
	"#        ######## #    ",
	"##########      # #    ",
	"                # #    ",
	"                # #    ",
	"                # #    ",
	"            ##### #####",
	"            #         #",
	"            #         #",
	"            #         #",
	"            ###########",
];
function getmapdata(pdata) {
	let map = {
		camw: 9,
		camh: 9,
		camx: 0,
		camy: 0,
		data: JSON.parse(JSON.stringify(gamemap))
	};
	// add player
	let r = map.data[pdata.y];
	map.data[pdata.y] = r.substr(0, pdata.x) + '@' + r.substr(pdata.x+1);
	// update camera info
	// pdata.camx = pdata.x - (pdata.camw/2|0);
	// pdata.camy = pdata.y - (pdata.camh/2|0);
	// OK
	return map;
}