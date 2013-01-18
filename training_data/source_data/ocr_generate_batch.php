<?php
$fonts = array(
	'Georgia',
	'Palatino-Linotype',
	'Times-New-Roman',
	'Helvetica',
	'Arial',
	'Comic-Sans-MS',
	'Impact',
	'Lucida-Sans-Unicode',
	'Tahoma',
	'Trebuchet-MS',
	'Verdana',
	'Courier-New',
	'Lucida-Console',
);
$characters = array();
for($i = '0'; ord($i) <= ord('9'); $i = chr(ord($i)+1))
{
	$characters[$i] = $i;
}
for($i = 'a'; ord($i) <= ord('z'); $i = chr(ord($i)+1))
{
	$characters['small_'.$i] = $i;
}
for($i = 'A'; ord($i) <= ord('Z'); $i = chr(ord($i)+1))
{
	$characters['capital_'.$i] = $i;
}
$cwd = getcwd();
foreach($fonts as $font)
{
	foreach($characters as $folder=>$c)
	{
		@mkdir($folder);
		$cmd  = "convert -fill \"#000000\" -font {$font} -pointsize 350 label:\"{$c}\" -trim -resize \"29x29!\" -type BiLevel";
		$font = strtolower($font);
		$cmd.= " {$cwd}\\{$folder}\\{$font}.png";
		echo $cmd."\n";
	}
}
?>