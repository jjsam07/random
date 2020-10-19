<html>
<head>
<title>Ping Service</title>
</head>
<body>
<form method="POST" action="index.php">
        <input type="text" name="ip" placeholder="127.0.0.1">
        <input type="submit">
</form>
<pre>
<?php 
$flag = "Comma@nd_1nJec7ion_Fl@9_1337_Th3_G@m3!!!";

if(isset($_POST["ip"]) && !empty($_POST["ip"])){
        $ip = @preg_replace("/[\\\$|`;&<>]/", "", $_POST["ip"]);
        $response = @shell_exec("timeout 5 bash -c 'ping -c 3 ".$ip."'");
        $receive = @preg_match("/3 packets transmitted, (.*) received/s",$response,$out);

        if ($out[1]=="3") 
        {
                echo "Ping OK";
        }
        elseif ($out[1]=="0")
        {
                echo "Ping NOK";
        }
        else
        {
                echo "Syntax Error";
        }
}
?>
</pre>
</body>
</html>