let gamedata = require('./gamedata');
let rooms = require('./rooms');

// handle client messages
function handlemessage(user, message) {
	message = message || "";
	var sysmessage = [];
	// check command
	let cmd = message.split(/\s+/);
	// normal chat message - add to chat log
	if (cmd[0].substr(0, 1) !== "/" || cmd[0] === "/say") {
		console.log("[/say] ["+user.user+"] "+message.substr(0, 25));
		gamedata.chatlog(user, message);
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
			gamedata.chatlog(user, "<warped to "+cmd[1]+">");  // room 1
			user.room = roomid;
			gamedata.chatlog(user, "<warped in>");  // room 2
			sysmessage.push("you warped to room ["+cmd[1]+"]");
		}
	}
	else if (cmd[0] === "/look") {
		console.log("[look]");
		let msg = rooms.look(user.room);
		msg.forEach(m => sysmessage.push(m));
		// get users in room
		let inroom = gamedata.inRoom(user.room);
		if (inroom.indexOf(user.user) > -1)  inroom.splice(inroom.indexOf(user.user), 1);
		if (inroom.length > 0)  sysmessage.push("here: " + inroom.join(", "));
		// get items in room
		let items = gamedata.inRoomItems(user.room);
		if (items.length > 0)  sysmessage.push("items: " + items.join(", "));
	}
	else if (cmd[0] === "/walk") {
		let roomid = rooms.walk(user.room, cmd[1]);
		if (roomid === null) {
			sysmessage.push("cannot walk "+rooms.directionName(cmd[1]));
		}
		else {
			gamedata.chatlog(user, "<exits "+ rooms.directionName(cmd[1]) +">");
			user.room = roomid;
			gamedata.chatlog(user, "<enters "+ rooms.directionName(rooms.directionOpposite(cmd[1])) +">");
			// sysmessage.push("you walked "+rooms.directionName(cmd[1]));
			let msg = handlemessage(user, "/look");
			msg.forEach(m => sysmessage.push(m));
		}
	}
	else if (cmd[0] === "/inv") {
		sysmessage.push("items in inventory:");
		user.inv.forEach(it => sysmessage.push("... " + it.name));
	}
	else if (cmd[0] === "/drop") {
		let msg = gamedata.dropItem(user, cmd[1]);
		msg.forEach(m => sysmessage.push(m));
		if (msg.length === 0)  gamedata.chatlog(user, "<drops "+cmd[1]+">");
	}
	else if (cmd[0] === "/get") {
		let msg = gamedata.getItem(user, cmd[1]);
		msg.forEach(m => sysmessage.push(m));
		if (msg.length === 0)  gamedata.chatlog(user, "<picks-up "+cmd[1]+">");
	}
	else {
		sysmessage.push("unknown command: "+cmd[0]);
	}
	// return errors
	return sysmessage;
}

// export
module.exports = {
	cmd: handlemessage
};
