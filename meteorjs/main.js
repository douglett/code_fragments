"use strict";

function main() {
	console.log("load");
	game.load();
}

var game = {
	canvas: null,
	ctx: null,
	dpad: { },
	level: 0,
	score: 0,
	rot: 0,
	rotspeed: 3,
	firespeed: 15,
	bullets: [ ],
	meteors: [ ],
	load: function() {
		var canvas = this.canvas = document.getElementById("mycanvas");
		canvas.width = 320;
		canvas.height = 240;
		this.shipx = canvas.width/2|0;
		this.shipy = canvas.height/2|0;
		var ctx = this.ctx = canvas.getContext("2d");
		this.qbfont = document.querySelector("img[src='qbfont.png']");
		this.ship = this.makeShip();
		this.meteor = this.makeMeteor();
		window.onkeydown = this.onkey.bind(this);
		window.onkeyup   = this.onkey.bind(this);
		this.nextlevel();
		this.frame();
	},
	frame: function() {
		//-- init and clear
		var canvas = this.canvas, ctx = this.ctx;
		ctx.fillStyle = "black";
		ctx.strokeStyle = "white";
		ctx.fillRect(0, 0, canvas.width, canvas.height);
		// ctx.drawImage(this.qbfont, 0, 0);
		this.drawText("meteor 0.1!", 10, 10);
		this.drawText("by doug!", 10, 20);
		this.drawText("score: "+this.score, 220, 10);
		//-- calculate
		this.rot += this.rotspeed * (this.dpad.xaxis||0);
		this.firett--;
		//-- player actions
		if (this.dpad.fire) {
			if ((this.firett||0) <= 0) {
				this.firett = this.firespeed;
				this.bullets.push({
					x: this.canvas.width/2|0,
					y: this.canvas.height/2|0,
					rot: this.rot,
					vel: 1
				});
			}
		}
		//-- move meteors
		this.meteors = this.meteors.filter(m => {
			m.x += Math.sin(m.rot * Math.PI/180) * m.vel;
			m.y += -Math.cos(m.rot * Math.PI/180) * m.vel;
			m.x = (m.x + canvas.width) % canvas.width;
			m.y = (m.y + canvas.height) % canvas.height;
			if (m.dead)  return false;
			return true;
		});
		//-- next level
		if (this.meteors.length === 0)  this.nextlevel();
		//-- move bullets
		this.bullets = this.bullets.filter(b => {
			b.x += Math.sin(b.rot * Math.PI/180) * b.vel;
			b.y += -Math.cos(b.rot * Math.PI/180) * b.vel;
			if (b.dead)  return false;
			return ( b.x>=0 && b.y>=0 && b.x<this.canvas.width && b.y<this.canvas.height );
		});
		//-- collide
		var pdead = 0;
		this.meteors.forEach(function(m) {
			if (pdead || this.cintersect(m.x, m.y, this.meteor.width, this.meteor.height, this.shipx, this.shipy, this.ship.width, this.ship.height)) {
				pdead = 1;
				return;
			}
			this.bullets.forEach(function(b) {
				if (this.cintersect(b.x, b.y, 2, 2, m.x, m.y, this.meteor.width, this.meteor.height)) {
					b.dead = m.dead = true;
					this.score += 10*this.level/2|0;
				}
			}.bind(this));
		}.bind(this));
		if (pdead) {
			this.bullets = [];
			this.meteors = [];
			this.level = 0;
			this.score = 0;
		}
		//-- paint
		ctx.save();
			ctx.translate(this.shipx, this.shipy);
			ctx.fillStyle = "red";
			ctx.rotate(this.rot * Math.PI/180);
			// ctx.fillRect(-50, -50, 100, 100);
			ctx.drawImage( this.ship, -this.ship.width/2|0, -this.ship.height/2|0 );
		ctx.restore();
		this.meteors.forEach( m => {
			ctx.save();
				ctx.translate( m.x, m.y );
				ctx.drawImage( this.meteor, -this.meteor.width/2|0, -this.meteor.height/2|0 );
			ctx.restore();
		});
		ctx.fillStyle = "white";
		this.bullets.forEach( b => ctx.fillRect( b.x, b.y, 2, 2 ) );
		// next frame
		window.requestAnimationFrame( this.frame.bind(this) );
	},
	makeShip: function() {
		var c = document.createElement("canvas");
		var w = c.width = 30;
		var h = c.height = 30;
		var ctx = c.getContext("2d");
		ctx.fillStyle = "black";
		ctx.strokeStyle = "white";
		// draw
		ctx.moveTo( h/2|0, 0 );
		ctx.lineTo( w, h );
		ctx.lineTo( w/2|0, h*3/4|0 );
		ctx.lineTo( 0, h );
		ctx.closePath();
		ctx.fill();
		ctx.stroke();
		return c;
	},
	makeMeteor: function() {
		var c = document.createElement("canvas");
		var w = c.width = 30;
		var h = c.height = 30;
		var ctx = c.getContext("2d");
		ctx.fillStyle = "black";
		ctx.strokeStyle = "white";
		// draw
		ctx.moveTo( w/3|0, 0 );
		ctx.lineTo( w/3*2|0, 0 );
		ctx.lineTo( w, h/3|0 );
		ctx.lineTo( w, h/3*2|0 );
		ctx.lineTo( w/3*2|0, h );
		ctx.lineTo( w/3|0, h );
		ctx.lineTo( 0, h/3*2|0 );
		ctx.lineTo( 0, h/3|0 );
		ctx.closePath();
		ctx.fill();
		ctx.stroke();
		return c;
	},
	drawText: function(str, x, y) {
		var ctx = this.ctx, qbf = this.qbfont, sx, sy;
		for (var i=0; i<str.length; i++) {
			sx = (str.charCodeAt(i) % 16) * 8;
			sy = (str.charCodeAt(i) / 16 | 0) * 8;
			ctx.drawImage( qbf, sx, sy, 8, 8, x+i*8, y, 8, 8 );
		}
	},
	cintersect: function(cx1, cy1, w1, h1, cx2, cy2, w2, h2) {
		if (cx1 - w1/2 > cx2 + w2/2 || cx1 + w1/2 < cx2 - w2/2)  return false;
		if (cy1 - h1/2 > cy2 + h2/2 || cy1 + h1/2 < cy2 - h2/2)  return false;
		return true;
	},
	nextlevel: function() {
		this.level++;
		while (this.meteors.length < this.level) {
			var m = {
				x: Math.random()*this.canvas.width|0,
				y: Math.random()*this.canvas.height|0,
				rot: Math.random()*360|0,
				vel: 0.5
			};
			if (!this.cintersect(
				this.shipx, this.shipy, this.ship.width, this.ship.height, 
				m.x, m.y, this.meteor.width, this.meteor.height
			)) {
				this.meteors.push(m);
			}
		}
	},
	onkey: function(e) {
		// console.log(arguments)
		var kdir = ( e.type === "keydown" ? 1 : 0 );
		switch (e.key) {
			case "ArrowUp":     break;
			case "ArrowDown":   break;
			case "ArrowLeft":   this.dpad.left  = kdir;  break;
			case "ArrowRight":  this.dpad.right = kdir;  break;
			case " ":           this.dpad.fire  = kdir;  break;
			// default:  console.log(e.key, e);  break;
		}

		// this.rot -= this.rotspeed;
		this.dpad.xaxis = (this.dpad.right||0 ) - (this.dpad.left||0);
		this.dpad.yaxis = (this.dpad.down||0)   - (this.dpad.up||0);
		// console.log(this.dpad.xaxis)
	}
};
