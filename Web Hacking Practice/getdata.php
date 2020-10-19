<?php
$txt=$_GET['c'];
$myfile = fopen("flag.txt", "a") or die("Unable to open file!");
fwrite($myfile, "\r\n");
fwrite($myfile, $txt);
fclose($myfile);
echo "ok";
?>