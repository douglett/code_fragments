function main() {
	console.log("main");
	game.addlog("Please enter a username to log in:")
	var input = document.getElementById("textinput");
	input.onkeydown = function(e) {
		if (e.key === "Enter")  game.sendbtn();
	};
	input.focus();
}

//-- main namespace
var game = {
	user: null
};

//-- dom manip
game.sendbtn = function() {
	if (this.user === null)
		this.sendlogin();
	else
		this.sendmessage();
}
game.getinput = function() {
	var input = document.getElementById("textinput");
	var txt = input.value;
	input.value = "";
	// check input
	if ((/^\W*$/g).test(txt)) {
		console.log("empty input - ignored");
		return false;
	}
	return txt;
}

//-- server calls
game.sendlogin = async function() {
	// input
	let txt = this.getinput();
	document.getElementById("textinput").focus();
	if (!txt)  return;
	this.addlog("logging in...");
	// request
	let data = await fetch("http://192.168.0.23:8081/login", {
		method: "POST",
		body: JSON.stringify({
			user: txt
		})
	});
	if (data.status == 200) {
		this.addlog("OK.");
		data = await data.json();
		this.user = data;
		this.sendgetlog();
	}
	else {
		data = await data.text();
		this.addlog("error: " + data);
	}
}
game.sendhelper = function(path, postobj) {
	postobj = postobj || {};
	// populate with session info
	if (this.user !== null) {
		postobj.user = this.user.user;
		postobj.uniqueid = this.user.uniqueid;
		postobj.lastupdate = this.user.lastupdate;
		postobj.room = this.user.room;
	}
	return fetch("http://192.168.0.23:8081/"+path, {
		method: "POST",
		body: JSON.stringify(postobj)
	})
	// on return, update client session info
	.then(d => d.json())
	.then(d => {
		if (this.user !== null) {
			this.user.lastupdate = d.lastupdate;
			this.user.room = d.room;
		}
		return d;
	});
}
game.sendgetlog = async function() {
	// request
	// let data = await fetch("http://192.168.0.23:8081/getlog");
	let data = await this.sendhelper("getlog");
	// data = await data.json();
	this.displaydata(data);
}
game.sendmessage = async function() {
	// input
	let txt = this.getinput();
	if (!txt)  return;
	// request
	let data = await fetch("http://192.168.0.23:8081/message", {
		method: "POST",
		body: JSON.stringify({
			user: this.user.user,
			uniqueid: this.user.uniqueid,
			message: txt
		})
	});
	data = await data.json();
	this.displaydata(data);
}

//-- display logic
game.addlog = function(str) {
	var content = document.querySelector("#textcontent");
	var row = document.createElement("div");
		row.className = "row";
	var msg = row.appendChild( document.createElement("span") );
		msg.className = "message";
		msg.innerHTML = str;
	content.appendChild(row);
}
game.displaydata = function(data) {
	var content = document.querySelector("#textcontent");
	content.innerHTML = "";
	var space = document.createElement("span");
		space.innerHTML = " ";
	data.log.forEach(d => {
		var row = document.createElement("div");
			row.className = "row";
		var ts = row.appendChild( document.createElement("span") );
			ts.className = "timestamp";
			ts.innerHTML = d.timestamp;
		row.appendChild( space.cloneNode(true) );
		var user = row.appendChild( document.createElement("span") );
			user.className = "user";
			user.innerHTML = d.user;
		row.appendChild( space.cloneNode(true) );
		var msg = row.appendChild( document.createElement("span") )
			msg.className = "message";
			msg.innerHTML = d.message;
		content.appendChild(row);
	});
	content.scrollTop = content.scrollHeight;
}
