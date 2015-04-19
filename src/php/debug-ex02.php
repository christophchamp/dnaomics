<?php
$i = 0;
while ( $i < 10 ) {
// Write some output
fwrite(STDOUT, $i."\n");
sleep(1);
$i++;
}
?>
