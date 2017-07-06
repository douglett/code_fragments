<?php

$body = file_get_contents('php://input');
$handle = fopen("test.cpp", "w") or die("error opening save file");
fwrite($handle, $body);
echo("Write ok");

?>