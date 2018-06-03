// room info and descriptions
let roomtypes = {
	"palmForest": {
		look: "A forest path. Palm trees and ferns line the sides of the wide animal trail."
	},
	"simpleHut": {
		look: "A simple hut, with earthen floor and straw roof. Simple, but idyllic."
	},
	"simpleHutUnused": {
		look: "A simple hut. The roof has partially blown off, straw is littered about the place."
	}
};

// map floor plan
let map = {
	map: [
		"X..X.",
		"JXXXX",
		"..X..",
		"..I.."
	],
	key: {
		"X": "palmForest",
		"I": "simpleHut",
		"J": "simpleHutUnused"
	},
	warp: {
		"hut": "I",
		"hut2": "J"
	},
	tileat: function(x, y) {
		x |= 0, y |= 0;  // force numbers
		if (y < 0 || y >= map.map.length)  return ".";
		if (x < 0 || x >= map.map[y].length)  return ".";
		return map.map[y].substr(x,1);
	},
	exits: function(roomid) {
		let room = roomid.split(".");
		let x = room[1]|0, y = room[2]|0;  // change coords to numbers
		let msg = [];
		if (map.tileat(x, y-1)   !== ".")  msg.push("n");
		if (map.tileat(x+1, y-1) !== ".")  msg.push("ne");
		if (map.tileat(x+1, y)   !== ".")  msg.push("e");
		if (map.tileat(x+1, y+1) !== ".")  msg.push("se");
		if (map.tileat(x, y+1)   !== ".")  msg.push("s");
		if (map.tileat(x-1, y+1) !== ".")  msg.push("sw");
		if (map.tileat(x-1, y)   !== ".")  msg.push("w");
		if (map.tileat(x-1, y-1) !== ".")  msg.push("nw");
		return msg;
	}
};

//
module.exports = {
	// find warp location and return new x,y
	warp: function(warpid) {
		let warpchar = map.warp[warpid];
		if (!warpchar) return null;
		for (let y=0; y<map.map.length; y++)
		for (let x=0; x<map.map[y].length; x++)
			if (map.map[y].substr(x, 1) === warpchar) {
				let name = map.key[warpchar];
				return [name, x, y];
			}
		return null;
	},
	// get system message: room look description, exits
	look: function(roomid) {
		let room = roomid.split(".");
		let msg = [ roomtypes[room[0]].look ];
		msg.push("exits: "+map.exits(roomid).join(", "));
		return msg;
	},
	// walk in direction. return new roomid
	walk: function(roomid, dir) {
		let room = roomid.split(".");
		let x = room[1]|0, y = room[2]|0;  // change coords to numbers
		let msg = [];
		// check exit exists
		if (map.exits(roomid).indexOf(dir) === -1)  return null;
		// move
		switch (dir) {
			case "n" :  y--;       break;
			case "ne":  y--, x++;  break;
			case "e":   x++;       break;
			case "se":  y++, x++;  break;
			case "s":   y++;       break;
			case "sw":  y++, x--;  break;
			case "w":   x--;       break;
			case "nw":  y--, x--;  break;
		}
		// return new room
		return [ map.key[map.tileat(x, y)], x, y ].join(".");
	},
	directionName: function(dir) {
		switch (dir) {
			case "n" :  return "north";
			case "ne":  return "north-east";
			case "e":   return "east";
			case "se":  return "south-east";
			case "s":   return "south";
			case "sw":  return "south-west";
			case "w":   return "west";
			case "nw":  return "north-west";
			default:    return "widdershins";
		}
	}
};