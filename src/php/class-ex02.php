<?php
/*
Simply put, :: is for class-level properties, and -> is for object-level properties.
- If the property belongs to the class, use ::
- If the property belongs to an instance of the class, use ->
*/
class Tester {
	public $foo;
	const BLAH="lala";

	public static function bar() {
		echo "Tester\n";
	}
}
$t = new Tester;
$t->foo;
Tester::bar();
printf("%s\n", Tester::BLAH);


/* When you declare a class, it is by default 'static'. You can access any method in that class using the :: operator, and in any scope. This means if I create a lib class, I can access it wherever I want and it doesn't need to be globaled:
*/
class lib {
	static function foo () {
		echo "Hello\n";
	}
}
lib::foo();

/* Now, when you create an instance of this class by using the new keyword, you use -> to access methods and values, because you are referring to that specific instance of the class. You can think of -> as inside of. (Note, you must remove the static keyword) IE:
*/
class lib2 {
	function foo () {
		echo "Hello2\n";
	}
}
$class = new lib2;
$class->foo();
