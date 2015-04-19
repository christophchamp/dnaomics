<?php
$arr_month = array(
'January' => 1,
'February' => 2,
'March' => 3,
'April' => 4,
'May' => 5,
'June' => 6,
'July' => 7,
'August' => 8,
'September' => 9,
'October' => 10,
'November' => 11,
'December' => 12);
foreach($arr_month as $k => $v) {
    $chk = $arr_month[substr($k,0,3)] = $v;
    echo $chk . "-" . substr($k,0,3) . PHP_EOL;
} // autogen a 3 letter version

//note that the overall size will be 23 because May will only exist once

#$month = 'Jan';
#$month = $arr_month[$month];
#echo $month; // outputs: 1
?>
