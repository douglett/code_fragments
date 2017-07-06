<?php

$body = file_get_contents('php://input');
mkdir("files");

if ($_GET["compiled"]) {
	$handle = fopen("files/test.cpp", "w") or die("error opening save file");
	fwrite($handle, $body);
	echo("compiled cpp write ok");
}
elseif ($_GET["data"]) {
	$handle = fopen("files/test.json", "w") or die("error opening save file");
	fwrite($handle, $body);
	echo("json write ok");
}
elseif ($_GET["getdata"]) {
	$data = file_get_contents("files/test.json");
	echo($data);
}
else {
	echo("unknown save type:" . $_GET["compiled"]);
}

?>