const http = require('http');
const URL = require('url');
// const fs = require('fs');
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
		// break;
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