<?php

$text = '&#124;';
// convert &#nnn; entities into characters
$text = preg_replace('/&#([0-9])+;/e', "chr('\\1')", $text);
print "$text\n";
?>
<?= $n = 14; // short_open_tag ?>
<?= $n = 0xFF; ?>
<?= "foo\n"; ?>
<?= 0755 . PHP_EOL . +010 . PHP_EOL; // 493 and 8 ?>
<?php echo ~(int)base_convert(str_repeat('1', 31), 2, 10); ?>
<?php //print "$n\n";?>

<?php
$x = 10;
if (is_int($x)) { print "$x is an integer.\n"; }
/*
if (is_float($x)) {}
if (is_string($x)) {}
if (is_bool($x)) {}
if (is_null($x)) {}
if ($a == $b) { echo "a and b are equal"; }
if (int($a * 1000) == int($b * 1000)) {} // numbers equal to 3 decimal places
*/
/* A comment begins here
?>
<p>Some stuff you want to be HTML.</p>
<?= $n = 14; ?>
*/
# Literals:
//0xFE 'th' "th" 1.43 true TRUE True null Null NULL 2001

#==Arrays==
$person[0] = "Edison";
$person[1] = "Wankel";
$person[2] = "Crapper";
$creator['Light bulb'] = "Edison";
$creator['Rotary Engine'] = "Wankel";
$creator['Toilet'] = "Crapper";
// The array( ) construct creates an array:
$person = array('Edison', 'Wankel', 'Crapper');
$creator = array('Light bulb'    => 'Edison',
		 'Rotary Engine' => 'Wankel',
		 'Toilet'        => 'Crapper');
foreach ($person as $name) {
echo "Hello, $name\n";
}
foreach ($creator as $invention => $inventor) {
echo "$inventor created the $invention\n";
}

sort($person);
// $person is now array('Crapper', 'Edison', 'Wankel')
asort($creator);
//if (is_array($x)) {}

##==Objects==
class Person {
  var $name = '';

  function name ($newname = NULL) {
    if (! is_null($newname)) {
      $this->name = $newname;
    }
    return $this->name;
  }
}
$ed = new Person;
$ed->name('Edison');
printf("Hello, %s\n", $ed->name);
$tc = new Person;
$tc->name('Crapper');
printf("Look out below %s\n", $tc->name);
//if (is_object($x)) {}

##==Resources==
/*
$res = database_connect(); // fictitious function
database_query($res);
$res = "boo"; // database connection automatically closed
*/
function search () {
  $res = database_connect();
  $database_query($res);
}
//if (is_resource($x)) {}

#==Variables==
if ($uninitialized_variable === NULL) { echo "Yes!\n"; }

# Variable variables:
$foo = 'bar';
$$foo = 'baz';
print "foo = $foo; $bar = $bar\n";
?>
