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
	$characters[] = $i;
}
$cwd = getcwd();
foreach($fonts as $font)
{
	foreach($characters as $c)
	{
		@mkdir($c);
		$cmd  = "convert -fill \"#000000\" -font {$font} -pointsize 350 label:\"{$c}\" -trim -resize \"29x29!\" -type BiLevel";
		$font = strtolower($font);
		$cmd.= " {$cwd}\\{$c}\\{$font}.png";
		echo $cmd."\n";
	}
}
?>