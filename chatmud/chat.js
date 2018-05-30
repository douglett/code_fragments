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
game.sendhelper = function(path, postobj) {
	// semaphore
	// if (this.fetchGuard)  return;
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
	// check for fetch errors
	.then(d => {
		if (d.status !== 200)
			throw { "error": d };
		return d.json();
	})
	// on return, update client session info
	.then(d => {
		if (this.user !== null) {
			this.user.lastupdate = d.lastupdate;
			this.user.room = d.room;
		}
		return d;
	});
}
game.sendlogin = async function() {
	// input
	let txt = this.getinput();
	if (!txt)  return;
	this.addlog("logging in...");
	// request
	let data = await game.sendhelper("login", {
		user: txt
	});
	this.addlog("user OK...");
	this.user = data;
	await this.sendping();
	this.addlog("ready!");
	setInterval(this.sendping.bind(this), 1000);
}
game.sendgetlog = async function() {
	// request
	let data = await this.sendhelper("getlog");
	this.displaydata(data);
}
game.sendping = async function() {
	// request
	let data = await this.sendhelper("message", {
		// user: this.user.user,
		// uniqueid: this.user.uniqueid,
		message: "/ping"
	});
	this.displaydata(data);
}
game.sendmessage = async function() {
	// input
	let txt = this.getinput();
	if (!txt)  return;
	// request
	let data = await this.sendhelper("message", {
		// user: this.user.user,
		// uniqueid: this.user.uniqueid,
		message: txt
	});
	this.displaydata(data);
}

//-- display logic
game.addlog = function(str) {
	var content = document.querySelector("#textcontent");
	var row = document.createElement("div");
		row.className = "row";
	var msg = row.appendChild( document.createElement("span") );
		msg.className = "message";
		msg.textContent = str;
	content.appendChild(row);
}
game.displaydata = function(data) {
	// setup
	var content = document.querySelector("#textcontent");
	var space = document.createElement("span");
		space.textContent = " ";
	// system messages
	data.sysmessage.forEach(m => game.addlog(m));
	// update log
	data.log.forEach(d => {
		var row = document.createElement("div");
			row.className = "row";
		var ts = row.appendChild( document.createElement("span") );
			ts.className = "timestamp";
			ts.textContent = d.timestamp;
		row.appendChild( space.cloneNode(true) );
		var user = row.appendChild( document.createElement("span") );
			user.className = "user";
			user.textContent = d.user;
		row.appendChild( space.cloneNode(true) );
		var msg = row.appendChild( document.createElement("span") )
			msg.className = "message";
			msg.textContent = d.message;
		content.appendChild(row);
	});
	content.scrollTop = content.scrollHeight;
	// update indicators
	let room = document.querySelector("#indicator_room");
	room.textContent = this.user.room;
}
