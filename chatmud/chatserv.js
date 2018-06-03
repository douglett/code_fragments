var http = require('http');
var fs = require('fs');
var url = require('url');
var rooms = require('./rooms');

// get saved chat log
var users = [];
var chat;
try {
	chat = fs.readFileSync("chatlog.json");
	chat = JSON.parse(chat);
}
catch(e) {
	chat = {
		log: []
	};
}

function serve404(res, errobj) {
	res.writeHead(404, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	errobj = errobj || {};
	errobj.timestamp = Date.now();
	res.write(JSON.stringify(errobj));
	return res.end();
}
function serveuser(res, username) {
	res.writeHead(200, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	let user = users.find(u => u.user === username);
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
	let user = users.find(u => u.user === username);
	// serve chat logs since last check-in
	let log = [];
	for (let i = chat.log.length-1; i >= 0; i--) {
		if (chat.log[i].timestamp < user.timestamp) break;
		if (chat.log[i].room !== user.room) continue;
		log.push(chat.log[i]);
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

function chatlog(user, message) {
	chat.log.push({
		user: user.user,
		timestamp: Date.now(),
		room: user.room,
		message: message
	});
	fs.writeFileSync("chatlog.json", JSON.stringify(chat)+"\n");
}

// handle client messages
function handlemessage(user, message) {
	message = message || "";
	var sysmessage = [];
	// check command
	let cmd = message.split(/\s+/);
	// normal chat message - add to chat log
	if (cmd[0].substr(0, 1) !== "/" || cmd[0] === "/say") {
		console.log("[/say] ["+user.user+"] "+message.substr(0, 25));
		chatlog(user, message);
	}
	else if (cmd[0] === "/ping") {
		// noop - just update
	}
	else if (cmd[0] === "/warp") {
		let warp = rooms.warp(cmd[1]);
		if (warp === null) {
			console.log("[/warp] unknown ["+cmd[1]+"]");
			sysmessage.push("unknown warp location: "+cmd[1]);
		}
		else {
			let roomid = warp.join(".");
			console.log("[/warp] ["+cmd[1]+"] ["+roomid+"]");
			chatlog(user, "<warped to "+cmd[1]+">");  // room 1
			user.room = roomid;
			chatlog(user, "<warped in>");  // room 2
			sysmessage.push("you warped to room ["+cmd[1]+"]");
		}
	}
	else if (cmd[0] === "/look") {
		// console.log("looking");
		let msg = rooms.look(user.room);
		msg.forEach(m => sysmessage.push(m));
	}
	else if (cmd[0] === "/walk") {
		let roomid = rooms.walk(user.room, cmd[1]);
		if (roomid === null) {
			sysmessage.push("cannot walk "+rooms.directionName(cmd[1]));
		}
		else {
			user.room = roomid;
			sysmessage.push("you walked "+rooms.directionName(cmd[1]));
			var msg = handlemessage(user, "/look");
			msg.forEach(m => sysmessage.push(m));
		}
	}
	else {
		sysmessage.push("unknown command: "+cmd[0]);
	}
	// return errors
	return sysmessage;
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
			users = users.filter(u => u.user !== username);
			// add user
			users.push({
				user: username,
				uniqueid: Math.random()*100000|0,
				timestamp: Date.now(),
				room: ""
			});
			let user = users.find(u => u.user === username);
			console.log("{/login} user ["+username+"]");
			handlemessage(user, "/warp hut");
			chatlog(user, "<"+username+" awakens>");
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
			let user = users.find(u => u.user === data.user && u.uniqueid === data.uniqueid);
			if (!user) {
				console.log("{/message} user credentials invalid - " + data.user);
				return serve404(res, { error: "username invalid - "+data.user });
			}
			let sysmessage = handlemessage( user, data.message || "" );
			servechatroom(res, user.user, sysmessage);
		});
	}
	else if (q.pathname === "/getlog") {
		console.log("{/getlog}");
		servechatlog(res);
	}
	else {
		console.log("{/?} unknown: "+q.pathname);
		serve404(res, { error: "unknown path: "+q.pathname });
	}
}).listen(8081);

console.log("chatserv started, port: 8081");
