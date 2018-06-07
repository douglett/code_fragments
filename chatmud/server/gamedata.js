var fs = require('fs');

// user-data, chatlog-data, and accessors
var gamedata = {
	users: [],
	items: [],
	chat: {
		log: []
	},
	load: function() {
		try {
			let data = fs.readFileSync("chatlog.json");
			this.chat = JSON.parse(data);
		}
		catch(e) { }
	},
	createUser: function(username) {
		this.users.push({
			user: username,
			uniqueid: Math.random()*100000|0,
			timestamp: Date.now(),
			room: "",
			inv: [{ name: "rock" }]
		});
		return this.users[this.users.length-1];
	},
	chatlog: function(user, message) {
		this.chat.log.push({
			user: user.user,
			timestamp: Date.now(),
			room: user.room,
			message: message
		});
		fs.writeFileSync("chatlog.json", JSON.stringify(this.chat)+"\n");
	},
	inRoom: function(roomname) {
		let data = [];
		this.users.forEach(u => { if (u.room === roomname) data.push(u.user); });
		return data;
	},
	inRoomItems: function(roomname) {
		let data = [];
		this.items.forEach(i => { if (i.room === roomname) data.push(i.name); });
		return data;
	},
	dropItem: function(user, itemname) {
		let msg = [];
		let item = user.inv.find(it => it.name === itemname);
		if (!item) {
			msg.push("no item in inv: "+itemname);
			return msg;
		}
		item.room = user.room;  // set location
		this.items.push(item);
		user.inv.splice(user.inv.indexOf(item));
		// msg.push("you dropped " + item.name);
		return msg;
	},
	getItem: function(user, itemname) {
		let msg = [];
		let item = this.items.find(it => it.name === itemname && it.room === user.room);
		if (!item) {
			msg.push("no item found: "+itemname);
			return msg;
		}
		this.items.splice(this.items.indexOf(item), 1);
		delete item.room;  // unset location
		user.inv.push(item);
		// msg.push("you picked up "+item.name);
		return msg;
	}
};

// init
gamedata.load();
// export
module.exports = gamedata;
