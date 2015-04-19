<?php
// SEE: http://www.php.net/manual/en/language.namespaces.definition.php
//declare(encoding='UTF-8');
//define('FOO', 'foo');

//== Global namespace ==
// The following code will define the constant "MESSAGE" in the global 
// namespace (i.e. "\MESSAGE").
namespace test1 {
define('MESSAGE', 'test1: Hello world!');
echo MESSAGE . PHP_EOL;
}

//== Local namespace ==
// The following code will define two constants in the "test" namespace.
namespace test2 {
define('test2\HELLO', 'test2: Hello world!');
define(__NAMESPACE__ . '\GOODBYE', 'test2: Goodbye cruel world!');
echo HELLO . PHP_EOL;
echo GOODBYE . PHP_EOL;
#echo FOO . PHP_EOL;
echo MESSAGE . PHP_EOL; // 'test1' namespace still valid
}

namespace { // global code
#session_start();
$a = test2\GOODBYE;
echo $a . PHP_EOL;
}
#use Aws\Common\Enum\ClientOptions as Options;
?>
