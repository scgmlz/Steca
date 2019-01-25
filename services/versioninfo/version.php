<?php

// Get and echo version string.
//
// This program is to be installed on a web server, at a location given
// by the variable STECA2_VERSION_URL from manifest.h.
// 
// The program queries GitHub, and extracts the latest relase version tag,
// without leading 'v'. So it just writes something like "2.1.1".
//
// This solution was chosen because of some difficulty using ssl under Windows.
// So Steca calls http: here, and here we call https: to GitHub.

// version from GitHub

$c = curl_init();
curl_setopt($c, CURLOPT_URL, 'https://github.com/scgmlz/Steca/releases/latest');
curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
curl_setopt($c, CURLOPT_HEADER, true);
curl_setopt($c, CURLOPT_NOBODY, true);
$page = curl_exec($c);
curl_close($c);

//preg_match("/<title>Release v(\d+\.\d+\S*)\s/", $page, $m);
preg_match("/Location: http.*v(\d+\S*)\s/", $page, $m);
echo $m[1];

// log this call

$addr = @$_SERVER['REMOTE_ADDR'];

$dir = dirname(__FILE__);
$log = $dir.'/.version.php';

$now = date('y.m.d H:i:s');

$txt = $now . "\t" . $addr . "\t" . urldecode(@$_SERVER['QUERY_STRING']) . "\n";
file_put_contents($log, $txt, FILE_APPEND | LOCK_EX);

// eof
