<?php

// Get and echo version string.
// It cannot be easily done by Steca itself, because ssl on Windows is problematic.
// Therefore Steca calls http: here and here calls https: to github.

// version from GitHub

$c = curl_init();
curl_setopt($c, CURLOPT_URL, 'https://raw.githubusercontent.com/scgmlz/steca2/master/VERSION');
curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
$ver = curl_exec($c);
curl_close($c);

$ver = trim(trim($ver), '"');
echo $ver;

// log this call

$addr = @$_SERVER['REMOTE_ADDR'];

$dir = dirname(__FILE__);
$log = $dir.'/.version.php';

$now = date('y.m.d H:i:s');

$txt = $now . "\t" . $addr . "\t" . urldecode(@$_SERVER['QUERY_STRING']) . "\n";
file_put_contents($log, $txt, FILE_APPEND | LOCK_EX);

// eof
