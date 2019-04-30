<?php
/**
 * IPP project 2019
 * 1. úloha
 * parse.php
 * Nikolas Masica <xmasic00>
 */

/***************************************************************/
/****************Pole pro instrukce****************************/
/**************************************************************/

$instructions = array(   //pole vsech instrukci
    "MOVE",
    "CREATEFRAME",
    "PUSHFRAME",
    "POPFRAME",
    "DEFVAR",
    "CALL",
    "RETURN",
    "PUSHS",
    "POPS",
    "ADD",
    "SUB",
    "MUL",
    "IDIV",
    "LT",
    "GT",
    "EQ",
    "AND",
    "OR",
    "NOT",
    "INT2CHAR",
    "STRI2INT",
    "READ",
    "WRITE",
    "CONCAT",
    "STRLEN",
    "GETCHAR",
    "SETCHAR",
    "TYPE",
    "LABEL",
    "JUMP",
    "JUMPIFEQ",
    "JUMPIFNEQ",
    "EXIT",
    "DPRINT",
    "BREAK");

$instruction_single = array(        //pole instrukci bez paramentru
    "CREATEFRAME",
    "PUSHFRAME",
    "POPFRAME",
    "RETURN",
    "BREAK");

$instruction_label = array(         //pole instrukci s parametrem label
    "CALL",
    "LABEL",
    "JUMP");

$instruction_symbol = array(        //pole instrukci s paramentrem <symb>
    "PUSHS",
    "WRITE",
    "DPRINT",
    "EXIT");

$instruction_variable = array(          //pole instrukci s parametrem <var>
    "DEFVAR",
    "POPS");

$instruction_variable_symbol = array(       //....
    "LALAL",
    "MOVE",
    "INT2CHAR",
    "STRLEN",
    "TYPE",
    "NOT");

$instruction_variable_type = array("READ");  // ...

$instruction_variable_symbol_symbol = array(  // ...
    "ADD",
    "SUB",
    "MUL",
    "IDIV",
    "LT",
    "GT",
    "EQ",
    "AND",
    "OR",
    "STRI2INT",
    "CONCAT",
    "GETCHAR",
    "SETCHAR");

$instruction_label_symbol_symbol = array("JUMPIFEQ", "JUMPIFNEQ");

/*************************************************************/
/************************kontrola argumentu*****************/
/************************************************************/
$longopts = array("help", "stats:", "loc", "comments", "labels", "jumps");
$options = getopt("", $longopts);
$stats = 'no';
if (count($argv) == 2)
{
    if($argv[1] === "--help")
    {
        printf("Skript typu filtr(parse.php v jazyce PHP7.3) načte ze standardního vstupu zdrojový kód v IPP-code19(viz sekce 6), zkontroluje lexikální a syntaktickou správnost kódu a vypíše na standardní výstup XML reprezentaci programu dle specifikace v sekci 3.1.\n");
        printf("Options:\n");
        printf("--help - vypíše nápovědu\n");
        exit(0);
    }
    else
    {
        fprintf(STDERR, "ERROR: 10 (function: Check arguments) Please use --help. Non existing argument\n");
        exit(10);
    }
}
if (count($argv) > 2 && count($argv) < 4)
{
    if(preg_grep('/--stats=.*/', $argv))
    {
        $file = $options["stats"];
        printf($file);
        $statfile = fopen($file, "w");

        if($argv[2] == '--loc'){
            $stats = 'loc';
        }
        elseif($argv[2] == '--comments'){
            $stats = 'com';
        }
        elseif($argv[2] == '--jumps'){
            $stats = 'jump';
        }
        elseif($argv[2] == '--labels'){
            $stats = 'label';
        }
        else
        {
            fprintf(STDERR,"ERROR: Wrong parameter\n");
            exit(10);
        }
    }else{
        fprintf(STDERR,"ERROR: missing stats file\n");
        exit(10);
    }
}
if(count($argv) >= 4)
{
    exit(10);
}



/***************************************************************/
/**************************funkce na kontrolu typu************/
/**************************************************************/

function get_type($value)
{
    if(preg_match('/^(GF|LF|TF|nil)@/', $value))
    {
        return "var";
    }
    elseif(preg_match('/^int@/', $value))
    {
        return "int";
    }
    elseif(preg_match('/^bool@/', $value))
    {
        return "bool";
    }
    elseif(preg_match('/^string@/', $value))
    {
        return "string";
    }
    elseif(preg_match('/^(string|int|bool|)$/i', $value))
    {
        return "type";
    }
    elseif (preg_match('/@/', $value))
    {
        fprintf(STDERR, "ERROR: 23, incorrect type of constant\n");
        exit(23);
    }
    else
    {
        return "label";
    }
}

/**************************************************************/
/*************Funkce pro kontrolu hodnoty proměnnej***********/
/**************************************************************/

function check_value($name, $type)
{

    /******regularne vyraze***************/

    $typeregex = '/^(int|string|bool|nil)/';
    $booleanregex = '/^(true|false)/';
    $variableregex = '~^(LF|TF|GF)@[a-zA-Z_\-$&%*\?\!][a-zA-Z0-9_\-$&%*\?\!]*$~';
    $labelregex = '~^[a-zA-Z_\-$&%*][a-zA-Z0-9_\-$&%*]*$~';
    $integerregex = '~^([+-]?[0-9]+)$~';
    $nilregex = '~^(nil)@(nil)~';

    /***************************************/

    if($type == 'var')
    {
        if(preg_match($variableregex, $name) || preg_match($nilregex,$name))
        {
            return $name;
        }
        else
        {
            fprintf(STDERR,"Error: 23 (function: check_value) Message: Incorrect argument of type var\n");
            exit(23);
        }
    }
    elseif ($type == 'label')
    {
        if(preg_match($labelregex, $name))
        {
            return $name;
        }
        else
        {
            fprintf(STDERR,"Error: 23 (function: check_value) Message: Incorrect argument of type label\n");
            exit(23);
        }
    }
    elseif ($type == 'int')
    {
        if(preg_match($integerregex, $name))
        {
            return $name;
        }
        else
        {
            fprintf(STDERR,"Error: 23 (function: check_value) Message: Incorrect argument of type int\n");
            exit(23);
        }
    }
    elseif ($type == 'string')
    {
        if(preg_match("~^~", $name) && !preg_match("~(\\\\[0-9]{0,2}($|\p{L}|\p{M}|\p{S}|\p{P}\p{Z}|\p{C}| )|\\\\[0-9]{4,})~u", $name))
        {
            return $name;
        }
        else
        {
            fprintf(STDERR,"Error: 23 (function: check_value) Message: Incorrect argument of type string\n");
            exit(23);
        }
    }
    elseif ($type == 'bool')
    {
        if(preg_match($booleanregex, $name))
        {
            return $name;
        }
        else
        {
            fprintf(STDERR,"Error: 23 (function: check_value) Message: Incorrect argument of type bool\n");
            exit(23);
        }
    }
    elseif ($type == 'type')
    {
        if(preg_match($typeregex, $name))
        {
            return $name;
        }
        else
        {
            fprintf(STDERR,"Error: 23 (function: check_value) Message: Incorrect argument of type type\n");
            exit(23);
        }
    }
}

/***********************************************************/
/**********Funkcia na prevod nepodporovanych znakov XML******/
/************************************************************/

function XMLConversion($specialchar)
{
    if (strpos($specialchar, '&') !== false)
    {
        $specialchar = str_replace("&", "&amp;", $specialchar);
    }
    if (strpos($specialchar, '<') !== false)
    {
        $specialchar = str_replace("<", "&lt;", $specialchar);
    }
    if (strpos($specialchar, '>') !== false)
    {
        $specialchar = str_replace(">", "&gt;", $specialchar);
    }
    return $specialchar;
}

/************************************************************/
/******Funkcia na kontrolu ci prvy riadok je .IPPcode19*******/
/*************************************************************/
function initialized_code($first_line)
{
    $first_line = trim($first_line);
    $first_line = preg_replace('/\s+/', ' ', $first_line);  //odstraneni bilych znaku
    $first_line = strtolower($first_line);
    $init_line = strtolower('.IPPcode19');
    if (strcmp($first_line, $init_line) === 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

$commentregex = '/#.*$/'; // regularni vyraz pro komentare

$file = fopen('php://stdin', "r");#'php://stdin'
$line_number = 0;
$instruction_number = 0;
$comments_number = 0;
$labels_number = 0;
$jumps_number = 0;

if($file)
{
    while (($line = fgets($file)) !== false)
    {
        $line = trim($line);
        $line = preg_replace('/\s+/', ' ', $line);  //odstraneni bilych znaku
        if (preg_match($commentregex, $line))
        {
            $comments_number++;
            $comment_place = strpos($line, "#");
            $line = substr($line, 0, $comment_place); // vymaze komentar
        }
        if (($line_number == 0) && (initialized_code($line) != true))
        {
            fprintf(STDERR, "ERROR: 21, Missing .IPPcode19\n");
            exit(21);
        } elseif (($line_number == 0) && (initialized_code($line) == true))
        {
            $dom = new DomDocument("1.0", "UTF-8");     //inicializace XML
            $program = $dom->createElement('program');
            $program->setAttribute('language', 'IPPcode19');
            $line_number++;
        } elseif ($line == "\n" || $line == '')
        {
            continue;
        } elseif ($line_number > 0)
        {
            $instruction_number++;
            $loadedwords = explode(' ', trim($line)); //ulozenie instrukcii
            $loadedwords[0] = strtoupper($loadedwords[0]);
            if (array_search($loadedwords[0], $instructions) !== false)  //kontrola ci dana instrukcia je v zozname instrukcii
            {
                if(($loadedwords[0] == "JUMP") || ($loadedwords[0] == "JUMPIFEQ") || ($loadedwords[0] == "JUMPIFNEQ"))
                {
                    $jumps_number++;
                }
                $line_number++;
                if (count($loadedwords) == 1 && array_search($loadedwords[0], $instruction_single) !== false) //spracovanie instrukcii z pola instruction_single
                {
                    $instr = $dom->createElement('instruction');
                    $instr->setAttribute('order', $instruction_number);
                    $instr->setAttribute('opcode', $loadedwords[0]);
                    $program->appendChild($instr);
                }
                elseif (count($loadedwords) == 2)   //spracovanie instrukcii z pola instrukction_varibale/symbol/label
                {
                    if (array_search($loadedwords[0], $instruction_variable) !== false)
                    {                                                                                       //zpracovani instrukcii z pole instruction_variable
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);

                        $arg = $dom->createElement('arg1', XMLConversion(check_value($loadedwords[1], "var"))); //prevod specialnych znaku
                        if (get_type($loadedwords[1]) == "var")
                        {
                            $arg->setAttribute('type', 'var');
                        }
                        else
                        {
                            fprintf(STDERR, "ERROR: 23, Expected instruction argument var.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);
                    }
                    elseif (array_search($loadedwords[0], $instruction_symbol) !== false)
                    {
                                                                                                                    //zpracovani instrukcii z pole instruction_symbol
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);
                        if (get_type($loadedwords[1]) == "var")
                        {
                            $arg = $dom->createElement('arg1', XMLConversion(check_value($loadedwords[1], "var")));
                        } else {
                            $value = preg_replace('/^.*?@/', '', $loadedwords[1]);
                            $arg = $dom->createElement('arg1', XMLConversion(check_value($value, get_type($loadedwords[1]))));
                        }
                        $arg->setAttribute('type', get_type($loadedwords[1]));
                        $instr->appendChild($arg);
                    }
                    elseif (array_search($loadedwords[0], $instruction_label) !== false)                //zpracovani instrukcii z pole instruction_label
                    {
                        if($loadedwords[0] == "LABEL")
                        {
                            $labels_number++;
                        }
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);

                        $arg = $dom->createElement('arg1', XMLConversion(check_value($loadedwords[1], "label")));
                        if (get_type($loadedwords[1]) == "label")
                        {
                            $arg->setAttribute('type', 'label');
                        } else {
                            fprintf(STDERR, "ERROR: Expected instruction argument label.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);
                    } else {
                        fprintf(STDERR, "ERROR: Too many arguments for instruction\n");
                        exit(23);
                    }
                }
                elseif (count($loadedwords) == 3)       //spracovanie 3 slovnych instrukcii
                {
                    if (array_search($loadedwords[0], $instruction_variable_symbol) !== false)
                    {                                                                                           //spracovanie instrukcii z pola instruction_variable_symbol
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);

                        $arg = $dom->createElement('arg1', XMLConversion(check_value($loadedwords[1], "var")));
                        if (get_type($loadedwords[1]) == "var")
                        {
                            $arg->setAttribute('type', 'var');
                        }
                        else
                        {
                            fprintf(STDERR, "ERROR: 23, Expected instruction argument var.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);

                        if (get_type($loadedwords[2]) == "var")
                        {
                            $arg = $dom->createElement('arg2', XMLConversion(check_value($loadedwords[2], "var")));
                        }
                        else
                        {
                            $value = preg_replace('/^.*?@/', '', $loadedwords[2]);
                            $arg = $dom->createElement('arg2', XMLConversion(check_value($value, get_type($loadedwords[2]))));
                        }
                        $arg->setAttribute('type', get_type($loadedwords[2]));
                        $instr->appendChild($arg);
                    }
                    elseif (array_search($loadedwords[0], $instruction_variable_type) !== false)
                    {                                                                                   //spracovanie instrukcii z pola instruction_variable_type
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);

                        $arg = $dom->createElement('arg1', XMLConversion(check_value($loadedwords[1], "var")));
                        if (get_type($loadedwords[1]) == "var")
                        {
                            $arg->setAttribute('type', 'var');
                        }
                        else
                        {
                            fprintf(STDERR, "ERROR: 23, Expected instruction argument var.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);

                        $arg = $dom->createElement('arg2', XMLConversion(check_value($loadedwords[2], "type")));
                        if (get_type($loadedwords[2]) == "type")
                        {
                            $arg->setAttribute('type', 'type');
                        }
                        else
                        {
                            fprintf(STDERR, "ERROR: 23, Expected instruction argument type.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);
                    }
                    else
                    {
                        fprintf(STDERR, "ERROR: Too many arguments for instruction\n");
                        exit(23);
                    }
                } elseif (count($loadedwords) == 4) //spracovanie instrukcii s 3 parametrami
                {
                    if (array_search($loadedwords[0], $instruction_variable_symbol_symbol) !== false)
                    {                                                                                                   //spracovanie instrukcii z pola instruction_symbol_symbol
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);

                        $arg = $dom->createElement('arg1', XMLConversion(check_value($loadedwords[1], "var")));
                        if (get_type($loadedwords[1]) == "var")
                        {
                            $arg->setAttribute('type', 'var');
                        }
                        else
                        {
                            fprintf(STDERR, "ERROR: 23, Expected instruction argument var.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);

                        if (get_type($loadedwords[2]) == "var")
                        {
                            $arg = $dom->createElement('arg2', XMLConversion(check_value($loadedwords[2], "var")));
                        }
                        else
                        {
                            $value = preg_replace('/^.*?@/', '', $loadedwords[2]);
                            $arg = $dom->createElement('arg2', XMLConversion(check_value($value, get_type($loadedwords[2]))));
                        }
                        $arg->setAttribute('type', get_type($loadedwords[2]));
                        $instr->appendChild($arg);

                        if (get_type($loadedwords[3]) == "var")
                        {
                            $arg = $dom->createElement('arg3', XMLConversion(check_value($loadedwords[3], "var")));
                        }
                        else
                        {
                            $value = preg_replace('/^.*?@/', '', $loadedwords[3]);
                            $arg = $dom->createElement('arg3', XMLConversion(check_value($value, get_type($loadedwords[3]))));
                        }
                        $arg->setAttribute('type', get_type($loadedwords[3]));
                        $instr->appendChild($arg);
                    }
                    elseif (array_search($loadedwords[0], $instruction_label_symbol_symbol) !== false)              ////spracovanie instrukcii z pola instruction_label_symbol_symbol
                    {
                        $instr = $dom->createElement('instruction');
                        $instr->setAttribute('order', $instruction_number);
                        $instr->setAttribute('opcode', $loadedwords[0]);
                        $program->appendChild($instr);

                        $arg = $dom->createElement('arg1', XMLConversion(check_value(preg_replace('/^.*?@/', '', $loadedwords[1]), "label")));
                        if (get_type($loadedwords[1]) == "label")
                        {
                            $arg->setAttribute('type', 'label');
                        }
                        else
                        {
                            fprintf(STDERR, "ERROR: 23, Expected instruction argument label.\n");
                            exit(23);
                        }
                        $instr->appendChild($arg);

                        if (get_type($loadedwords[2]) == "var")
                        {
                            $arg = $dom->createElement('arg2', XMLConversion(check_value($loadedwords[2], "var")));
                        }
                        else
                        {
                            $value = preg_replace('/^.*?@/', '', $loadedwords[2]);
                            $arg = $dom->createElement('arg2', XMLConversion(check_value($value, get_type($loadedwords[2]))));
                        }
                        $arg->setAttribute('type', get_type($loadedwords[2]));
                        $instr->appendChild($arg);

                        if (get_type($loadedwords[3]) == "var")
                        {
                            $arg = $dom->createElement('arg3', XMLConversion(check_value($loadedwords[3], "var")));
                        }
                        else
                        {
                            $value = preg_replace('/^.*?@/', '', $loadedwords[3]);
                            $arg = $dom->createElement('arg3', XMLConversion(check_value($value, get_type($loadedwords[3]))));
                        }
                        $arg->setAttribute('type', get_type($loadedwords[3]));
                        $instr->appendChild($arg);
                    }
                    else
                    {
                        fprintf(STDERR, "ERROR: 23,Too many arguments for instruction\n");
                        exit(23);
                    }
                }
                else
                {
                    fprintf(STDERR, "ERROR: 23,Too many arguments for instruction\n");
                    exit(23);
                }
            }
            else
            {
                fprintf(STDERR, "ERROR: 23, Undefined instruction\n");
                exit(22);
            }
        }
    }

    if (ftell($file) == false)          //kontrola STDIN
    {
        fprintf(STDERR, "ERROR: 11, Missing STDIN.\n");
        exit(21);
    }
    fclose($file);      //zatvorenie STDIN
    $dom->appendChild($program);
    $dom->formatOutput = true;
    echo $dom->saveXML();       //vypis XML

    if($stats != 'no'){
        if($stats == 'loc')
            fwrite($statfile, "$instruction_number\n");
        if($stats == 'com')
            fwrite($statfile, "$comments_number\n");
        if($stats == 'label')
            fwrite($statfile, "$labels_number\n");
        if($stats == 'jump')
            fwrite($statfile, "$jumps_number\n");
        fclose($statfile);
    }
}
else
{
    fprintf(STDERR,"ERROR: 11,Failed to access STDIN\n");
    exit(11);
}

