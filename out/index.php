<? 
/** 
* Change the path to your folder. 
* This must be the full path from the root of your 
* web space. If you're not sure what it is, ask your host. 
* 
* Name this file index.php and place in the directory. 
*/ 
    // Define the full path to your folder from root 
    $path = "out/"; 

    // Open the folder 
    $dir_handle = @opendir($path) or die("Unable to open $path"); 

    // Loop through the files 
    while ($file = readdir($dir_handle)) { 

    if($file == "." || $file == ".." || $file == "index.php" ) 

        continue; 
        $files[]=$file;
        //echo "<a href=\"$file\">$file</a><br />"; 

    } 

    // Close 

    closedir($dir_handle); 

    natsort($files);
?>
<table>
<?
    foreach ($files as $key => $value) {
        $p=$path.$value;
        echo "<tr><td><a href=\"".$p."\">http://ahkclan.cz/gtv/$p</a></td><td>". sprintf("%6.1f", filesize($p)/1024 ) . " kB</td><td>". date ("F d Y H:i:s.", filemtime($p)) ."</td></tr>\n"; 
    }
?>
</table>