var http = require('http');
var url = require('url');
// user modules
let gamedata = require('./gamedata');
let rooms;
let command;

// set up live reload of user modules
function reload_modules() {
	['./rooms', './command'].forEach(m => 
		delete require.cache[require.resolve(m)] );
	rooms = require('./rooms');
	command = require('./command');
}
reload_modules(); // first load

// basic object serving
function serve200(res, obj) {
	return serveObj(200, res, obj);
}
function serve404(res, obj) {
	return serveObj(404, res, obj);
}
function serveObj(code, res, obj) {
	code |= 0;
	res.writeHead(code, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	obj = obj || {};
	obj.timestamp = obj.timestamp || Date.now();
	res.write(JSON.stringify(obj));
	return res.end();
}

// serve data
function serveuser(res, username) {
	res.writeHead(200, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	let user = gamedata.users.find(u => u.user === username);
	let obj = {
		user: user.user,
		uniqueid: user.uniqueid,
		timestamp: user.timestamp,
		room: user.room,
		sysmessage: [
			"Welcome to Dinosaur Island, Advenurer."
		]
	};
	res.write(JSON.stringify(obj));
	return res.end();
}
function servechatlog(res) {
	res.writeHead(200, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	res.write(JSON.stringify(chat));
	return res.end();
}
function servechatroom(res, username, sysmessage) {
	// get user
	let user = gamedata.users.find(u => u.user === username);
	// serve chat logs since last check-in
	let log = [];
	for (let i = gamedata.chat.log.length-1; i >= 0; i--) {
		if (gamedata.chat.log[i].timestamp < user.timestamp) break;
		if (gamedata.chat.log[i].room !== user.room) continue;
		log.push(gamedata.chat.log[i]);
	}
	// update check-in time
	user.timestamp = Date.now();
	// make response object
	let obj = {
		user: user.user,
		timestamp: user.timestamp,
		room: user.room,
		sysmessage: sysmessage || [],
		log: log
	};
	// write out
	res.writeHead(200, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	res.write(JSON.stringify(obj));
	return res.end();
}

// serve
http.createServer(function (req, res) {
	// console.log("request!");
	let q = url.parse(req.url, true);
	
	if (q.pathname === "/login" && req.method === "POST") {
		// console.log("login");
		let body = "";
		req.on('data', chunk => { body += chunk.toString(); });
		req.on('end', e => {
			let data = JSON.parse(body);
			let username = data.user;
			// validate username
			if (!(/^[A-Z][A-Z0-9]*$/i).test(username)) {
				console.log("{/login} username invalid - "+username);
				return serve404(res);
			}
			// log out duplicates (temp)
			gamedata.users = gamedata.users.filter(u => u.user !== username);
			// add user
			gamedata.users.push({
				user: username,
				uniqueid: Math.random()*100000|0,
				timestamp: Date.now(),
				room: ""
			});
			let user = gamedata.users.find(u => u.user === username);
			console.log("{/login} user ["+username+"]");
			command.cmd(user, "/warp hut");
			gamedata.chatlog(user, "<"+username+" awakens>");
			// return user credentials
			serveuser(res, data.user);
		});
	}
	else if (q.pathname === "/message" && req.method === "POST") {
		// console.log("message POST");
		let body = "";
		req.on('data', chunk => { body += chunk.toString(); });
		req.on('end', e => {
			let data = JSON.parse(body);
			// validate user
			let user = gamedata.users.find(u => u.user === data.user && u.uniqueid === data.uniqueid);
			if (!user) {
				console.log("{/message} user credentials invalid - " + data.user);
				return serve404(res, { error: "username invalid - "+data.user });
			}
			let sysmessage = command.cmd( user, data.message || "" );
			servechatroom(res, user.user, sysmessage);
		});
	}
	else if (q.pathname === "/getlog") {
		console.log("{/getlog}");
		servechatlog(res);
	}
	else if (q.pathname === "/reload") {
		console.log("{/reload}");
		reload_modules();
		serve200(res, { sysmessage: "module reload completed" });
	}
	else {
		console.log("{/?} unknown: "+q.pathname);
		serve404(res, { error: "unknown path: "+q.pathname });
	}
}).listen(8081);

console.log("chatserv started, port: 8081");
