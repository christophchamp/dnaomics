<?php

class Person {
	// Properties:
	var $name;

	// NOTE: You can assign default values to properties, but those default 
	// values must be simple constants:
	var $city = 'Seattle';	// works
	var $age = 0;		// works
	#var $day = 60*60*24;	// doesn't work!

	// Methods:
	function get_name () {
		return $this->name;
	}

	function set_name ($new_name) {
		$this->name = $new_name;
	}

}
$f = new Person();
$f->set_name('Soren');
printf("name = %s\n", $f->get_name());
$b = new Person();
$b->name = "Stine";
printf("name = %s\n", $b->get_name());

//== Inheritance ==
// To inherit the properties and methods from another class, use the extends 
// keyword in the class definition, followed by the name of the base class:
class Employee extends Person {
	// The Employee class contains the $position and $salary properties, as
	// well as the $name, $city, and $age properties inherited from the 
	// Person class.
	var $position, $salary;
}

//== Constructors ==
// A constructor is a function with the same name as the class in which it is 
// defined.  Here is a constructor for the Person class:
class Person2 {
	function Person2 ($name, $age) {
		$this->name = $name;
		$this->age = $age;
	}
}

class HTML_Stuff {
	function start_table () {
		echo "<table border='1'>\n";
	}
	function end_table () {
		echo "</table>\n";
	}
}
$html = new HTML_Stuff();
$html->start_table();
$html->end_table();

$json_code = '{"Id":{"N":"2"},"Name":{"S":"Alice"},"Email":{"S":"alice@somewhere.com"},"Password":{"S":"alicepasswd123"}}';
$array = json_decode($json_code);
foreach ($array as $key => $jsons) { // This will search in the 2 jsons
     foreach($jsons as $key => $value) {
         echo $value . PHP_EOL; // This will show jsut the value f each key like "var1" will print 9
                       // And then goes print 16,16,8 ...
    }
}
?>
