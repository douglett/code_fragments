<?php

$body = file_get_contents('php://input');
mkdir("files");

if ($_GET["compiled"]=="true") {
	$handle = fopen("files/test.cpp", "w") or die("error opening save file");
	fwrite($handle, $body);
	echo("compiled cpp write ok");
}
elseif ($_GET["data"]=="true") {
	$handle = fopen("files/test.json", "w") or die("error opening save file");
	fwrite($handle, $body);
	echo("json write ok");
} 
else {
	echo("unknown save type:" . $_GET["compiled"]);
}

?>