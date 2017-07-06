// var headers = [ "iostream", "string", "cstdint", "cassert", "vector", "map", "fstream", "sstream" ];
var default_header = [
	"#include <iostream>", 
	"#include <string>", 
	"#include <cstdint>", 
	"#include <cassert>", 
	"#include <vector>", 
	"#include <map>", 
	"#include <fstream>", 
	"#include <sstream>",
	"",
	"typedef int32_t  i32;",
	"typedef uint32_t u32;",
	"",
	"using namespace std;"
];

var methods = [
	{ sig: "<header>", 
		body: []},
	{ sig: "int main()", 
		body: [
			'printf("hello world\\n");'
		]},
	{ sig: "std::string hello()",
		body: [
			'return "hello";'
		]}
];

function main() {
	console.log("hello");
	$("#method_body").on("input", method_body_save);
	$("#method_head").on("input", method_head_save);
	method_list_update();
}

function method_list_update() {
	var mlist = $("#method_list");
	mlist.empty();
	// add rest
	methods.forEach(function(v) {
		var div = $("<div/>").text(v.sig)
			.click(function() {
				$("#method_list > div").removeClass("highlight");
				$(this).addClass("highlight");
				method_body_load();
			});
		// if (v.sig!=="<header>")  div.attr("contenteditable", "true");
		mlist.append(div);
	});
	$("#method_list :first-child").click();
}

function method_current() {
	var c = $("#method_list div.highlight").text();
	var m = methods.find(function(m) {
		if (m.sig===c)  return m;
	});
	return m;
}

function method_body_load() {
	var m = method_current();
	var mb = $("#method_body").empty();
	// add each item
	m.body.forEach(function(ln) {
		mb.append( $("<p/>").text(ln) );
	});
	// need at least one <p> element
	if (mb.children().length===0)  mb.append("</p>");
	// set header
	var h = $("#method_head").text(m.sig);
	if    (m.sig==="<header>")  h.removeAttr("contenteditable");
	else  h.attr("contenteditable", "true");
}

function method_head_save() {
	var m = method_current();
	m.sig = $("#method_head").text();
	$(".maintable div.highlight").text(m.sig);
}

function method_body_save() {
	if ($("#method_head").text()==="<compiled>")  return;
	var m = method_current();
	m.body.splice(0, m.body.length);
	$("#method_body").children().each(function(k, el) {
		el = $(el);
		m.body.push(el.text());
	});
}

function compile() {
	var prog      = [ ];
	var fndef     = [ ];
	var seperator = [ "", "/* ***** */", "" ];

	methods.forEach(function(m) {
		if (m.sig=="<header>")  return;
		fndef.push(m.sig + ";");
	});

	methods.forEach(function(m) {
		if (m.sig=="<header>")  return;
		prog.push(m.sig + " {");
		m.body.forEach(txt => prog.push("\t" + txt));
		prog.push("}", "");
	});

	prog=[].concat(default_header, [""], methods[0].body, seperator, fndef, seperator, prog);

	// prog.forEach(v => console.log(v));
	$("#method_list > div").removeClass("highlight");
	$("#method_body").empty().text(prog.join("\n"));
	$("#method_head").text("<compiled>").removeAttr("contenteditable");
}

function send() {
	if ($("#method_head").text()!=="<compiled>") {
		alert("Please compile before saving");
		return;
	}
	fetch("compile.php?compiled=true", {
		method: "POST",
		body: $("#method_body").text()
	})
	.then(response => response.text())
	.then(text => {
		console.log(text);
	});
}

function save_json() {
	fetch("compile.php?data=true", {
		method: "POST",
		body: JSON.stringify(methods)
	})
	.then(response => response.text())
	.then(text => {
		console.log(text);
	});
}



// function save_file() {
// 	// var blob = new Blob([$("#method_body").text()], {type: "text/plain;charset=utf-8"});
// 	// saveAs(blob, "test.cpp");

// 	var iframe = document.createElement("iframe");
// 	window.iframe = iframe;
// 	document.body.appendChild(iframe);
// 	iframe.contentWindow.open('data:text/csv;charset=utf-8,' + $("#method_body").text());
// }