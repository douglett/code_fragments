const http = require('http');
const URL = require('url');
const fs = require('fs');
const st = require('st');

const port = 9005;
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
	// console.log(url.pathname);
	console.log(request.url);

	switch (url.pathname) {
	case "/":
		fs.readFile("./static/index.html", function(err, html) {
			response.writeHead(200, {"Content-Type": "text/html"});
			response.end(html);
		});
		break;
	case "/action.json": {
		// get initial player data
		// let pdata = gameplayers[0];
		let pdata = gameplayers.find(p => url.query.playerid === p.id);
		if (!pdata)  throw `missing player: ${url.query.playerid}`;
		// do action
		if ("wait" in url.query) {
			switch (url.query.walk) {
			case 'n':  if (!gamemap.checkMapCollision(pdata.x, pdata.y-1))  pdata.y--;  break;
			case 's':  if (!gamemap.checkMapCollision(pdata.x, pdata.y+1))  pdata.y++;  break;
			case 'e':  if (!gamemap.checkMapCollision(pdata.x+1, pdata.y))  pdata.x++;  break;
			case 'w':  if (!gamemap.checkMapCollision(pdata.x-1, pdata.y))  pdata.x--;  break;
			}
		}
		else if ("say" in url.query) {
			gamechat.chat.push({ from: pdata.id, text: url.query.say });
		}
		else if ("wait" in url.query) {
			// noop
		}
		else {
			console.log("unknown:", url.query);
		}
		// return player object and position
		response.writeHead(200, {"Content-Type": "text/json"});
		response.write(JSON.stringify({
			pdata: pdata,  // current player data
			map: gamemap.getMapData(pdata),  // draw map into response
			chat: gamechat.chat  // current chat data
		}));
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
},{
	id: 'player2',
	x: 2,
	y: 2
}];
let gamechat = {
	chat: [
		{ from: "sys", text: "hello world" },
		{ from: "sys", text: "some chat text" }
	]
};
let gamemap = {
	gamemap: [
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
	],
	getMapData: function(pdata) {
		let map = {
			camw: 9,
			camh: 9,
			camx: 0,
			camy: 0,
			data: JSON.parse(JSON.stringify(this.gamemap))
		};
		// add players
		gameplayers.forEach(p => {
			let r = map.data[p.y];	
			map.data[p.y] = r.substr(0, p.x) + 'a' + r.substr(p.x+1);
		})
		// add current player
		let r = map.data[pdata.y];
		map.data[pdata.y] = r.substr(0, pdata.x) + '@' + r.substr(pdata.x+1);
		// update camera info
		// pdata.camx = pdata.x - (pdata.camw/2|0);
		// pdata.camy = pdata.y - (pdata.camh/2|0);
		// OK
		return map;
	},
	checkInBounds: function(x, y) {
		return ( x >= 0 && y >= 0 && y < this.gamemap.length && x < this.gamemap[y].length );
	},
	checkMapCollision: function(x, y) {
		if (!this.checkInBounds)  return false;
		return ( this.gamemap[y][x] !== ' ' );
	},
	// should these be in here?
	checkMobCollision: function(x, y) {
		return false;
	},
	checkPlayerCollision: function(x, y) {
		return false;
	}
};