var http = require('http');
var fs = require('fs');
var url = require('url');

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
	res.write( JSON.stringify( users.find(u => u.user === username) ) );
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
	for (let i = 0; i < chat.log.length; i++) {
		if (chat.log[i].timestamp < user.timestamp) continue;
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
	if (cmd[0].substr(0, 1) !== "/") {
		chatlog(user, message);
	}
	else if (cmd[0] === "/ping") {
		// noop - just update
	}
	else if (cmd[0] === "/warp") {
		if (["red", "green", "blue"].indexOf(cmd[1]) !== -1) {
			chatlog(user, "<warped to "+cmd[1]+">");  // room 1
			user.room = cmd[1];
			chatlog(user, "<warped in>");  // room 2
		}
		else {
			sysmessage.push("unknown warp location: "+cmd[1]);
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
		console.log("login");
		let body = "";
		req.on('data', chunk => { body += chunk.toString(); });
		req.on('end', e => {
			let data = JSON.parse(body);
			let username = data.user;
			// validate username
			if (!(/[A-Z][A-Z0-9]*/i).test(username)) {
				console.log("username invalid - "+username);
				return serve404(res);
			}
			// log out duplicates (temp)
			users = users.filter(u => u.user !== username);
			// add user
			users.push({
				user: username,
				uniqueid: Math.random()*100000|0,
				timestamp: Date.now(),
				room: "red"
			});
			chatlog(users.find(u => u.user === username), "<"+username+" awakens>");
			// return user credentials
			serveuser(res, data.user);
		});
	}
	else if (q.pathname === "/message" && req.method === "POST") {
		console.log("message POST");
		let body = "";
		req.on('data', chunk => { body += chunk.toString(); });
		req.on('end', e => {
			let data = JSON.parse(body);
			// validate user
			let user = users.find(u => u.user === data.user && u.uniqueid === data.uniqueid);
			if (!user) {
				console.log("user credentials invalid - " + data.user);
				return serve404(res, { error: "username invalid - "+data.user });
			}
			let sysmessage = handlemessage( user, data.message || "" );
			servechatroom(res, user.user, sysmessage);
		});
	}
	else if (q.pathname === "/getlog") {
		console.log("getlog");
		servechatlog(res);
	}
	else {
		console.log("unknown: "+q.pathname);
		serve404(res, { error: "unknown path: "+q.pathname });
	}
}).listen(8081);

console.log("chatserv started, port: 8081");
