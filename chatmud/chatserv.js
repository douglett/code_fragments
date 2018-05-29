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

function serve404(res, path) {
	res.writeHead(404, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	res.write("unknown command: "+path);
	return res.end();
}
function serveuser(res, user) {
	res.writeHead(200, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	res.write( JSON.stringify( users.find(u => u.user == user) ) );
	return res.end();
}
function servechat(res) {
	res.writeHead(200, {
		'Content-Type': 'text/plain',
		'Access-Control-Allow-Origin': '*'
	});
	res.write(JSON.stringify(chat));
	return res.end();
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
			// validate username
			if (!(/[A-Z][A-Z0-9]*/i).test(data.user)) {
				console.log("username invalid - " + data.user);
				return serve404(res);
			}
			users = users.filter(u => u.user !== data.user);
			// add user
			users.push({
				user: data.user,
				uniqueid: Math.random()*100000|0
			});
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
			if (!users.find(u => u.user === data.user && u.uniqueid === data.uniqueid)) {
				console.log("user credentials invalid - " + data.user);
				return serve404();
			}
			// add to chat log
			chat.log.push({
				timestamp: Date.now(),
				user: data.user,
				message: data.message
			});
			fs.writeFileSync("chatlog.json", JSON.stringify(chat)+"\n");
			servechat(res);
		});
	}
	else if (q.pathname === "/getlog") {
		console.log("getlog");
		servechat(res);
	}
	else {
		console.log("unknown: "+q.pathname);
		serve404(res, q.pathname);
	}
}).listen(8081);

console.log("chatserv started, port: 8081");
