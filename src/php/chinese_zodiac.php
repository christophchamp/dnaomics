<?php
function getChineseZodiac($year){

    switch ($year % 12) :
        case  0: return 'Monkey';  // Years 0, 12, 1200, 2004...
        case  1: return 'Rooster';
        case  2: return 'Dog';
        case  3: return 'Boar';
        case  4: return 'Rat';
        case  5: return 'Ox';
        case  6: return 'Tiger';
        case  7: return 'Rabit';
        case  8: return 'Dragon';
        case  9: return 'Snake';
        case 10: return 'Horse';
        case 11: return 'Lamb';
    endswitch;
}

echo getChineseZodiac(2016);
?>
