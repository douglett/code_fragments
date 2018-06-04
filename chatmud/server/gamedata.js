var fs = require('fs');

// user-data, chatlog0-data, and accessors
var gamedata = {
	users: [],
	chat: {
		log: []
	},
	load: function() {
		let chat;
		try {
			let data = fs.readFileSync("chatlog.json");
			this.chat = JSON.parse(data);
		}
		catch(e) { }
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
	inroom: function(roomname) {
		let data = [];
		this.users.forEach(u => { if (u.room === roomname) data.push(u.user); });
		return data;
	}
};

// init
gamedata.load();
// export
module.exports = gamedata;
