<?php
$white = "snow";
$black =& $white;
print "\$white = $white\n";
print "\$black = $black\n";
unset($white);
print "\$white = $white\n";
print "\$black = $black\n";
?>
