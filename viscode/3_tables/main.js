var blocks = [
	{
		id: "hello",
		type: "function",
		args: [ "string& a", "string& b" ],
		body: [
			"if_a"
		]
	},{
		id: "hello:if_a",
		type: "condition",
		body: []
	}
];


function main() {
	var pd = document.querySelector("#progdiv");
	pd.innerHTML = "";
	console.log("here");

	blocks.forEach(function(b, i) {
		var table, tr, td;
		// table
		table = document.createElement("table");
		pd.appendChild(table);
		table.setAttribute("border", 2);
		// header
		tr = document.createElement("tr");
		table.appendChild(tr);
		tr.innerHTML = "<th>"+b.id+"  ("+b.type+")</th>";
		// body
		tr = document.createElement("tr");
		table.appendChild(tr);
		td = document.createElement("td");
		tr.appendChild(td);
		// fill body
		b.body.forEach(function(ln, i) {
			td.innerHTML += "<p>"+ln+"</p>";
		});
	});
}