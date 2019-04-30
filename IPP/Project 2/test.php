<?php
$parse = "./parse.php";
$interpret = "./interpret.py";
$testFailed = 0;
$testPassed = 0;
$html = "";
$path = ".";
$intonly = false;
$parseonly = false;
$recursive = false;
$longopts = array("help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only");
$arguments = getopt("", $longopts);

if (count($arguments) != $argc-1)
{
    printf(STDERR, "10: Zle argumenty. Pouzijte --help pre napovedu.\n");
    exit(10);
}

if ($argc != 1)             //kontrola argumentu
{
    if (isset($arguments["help"]))
    {
        if ($argc > 2)
        {
            printf(STDERR, "10: Neplatné argumenty. Zobrazte nápovedu pomocou parametra --help.\n");
            exit(10);
        }
        echo "Napoveda:\n";
        echo "Execution: ./test.php [--help] [--directory=path] [--recursive] [--parse-script=file] [--int-script=file] [parse-only] [int-only]\n";
        echo "--help              -- zobrazí nápovedu\n";
        echo "--directory=path    -- testy bude hledat v zadaném adresáři (chybí-li tento parametr, tak skript prochází aktuální adresář)\n";
        echo "--recursive         -- testy bude hledat nejen v zadaném adresáři, ale i rekurzivně ve všech jeho podadresářích\n";
        echo "--parse-script=file -- soubor se skriptem v PHP 7.3 pro analýzu zdrojového kódu v IPPcode19 (chybí-li tento parametr, tak implicitní hodnotou je parse.php uložený v aktuálním adresáři) \n";
        echo "--int-script=file   -- soubor se skriptem v Python 3.6 pro interpret XML reprezentace kódu v IPPcode19 (chybí-li tento parametr, tak implicitní hodnotou je interpret.py uložený v aktuálním adresáři)\n";
        echo "--parse-only        -- bude testován pouze skript pro analýzu zdrojového kódu v IPPcode19 (tento parametr se nesmí kombinovat s parametrem --int-script), výstup s referenčním výstupem porovnávejte nástrojem A7Soft JExamXML\n";
        echo "--int-only          -- bude testován pouze skript pro interpret XML reprezentace kódu v IPPcode19 (tento parametr se nesmí kombinovat s parametrem --parse-script)\n";
        exit(0);
    }
    if (isset($arguments["parse-script"]))      //nastaveni hodnot podle argumentu
    {
        $parse = $arguments["parse-script"];
    }
    if (isset($arguments["int-script"]))
    {
        $interpret = $arguments["int-script"];
    }
    if (isset($arguments["directory"]))
    {
        $path = $arguments["directory"];
    }
    if (isset($arguments["recursive"]))
    {
        $recursive = true;
    }
    if (isset($arguments["parse-only"]))
    {
        $parseonly = true;
    }
    if (isset($arguments["int-only"]))
    {
        $intonly = true;
    }


}


if($recursive)          //ak bol zadany argument recursive tak skript prehladava rekurzivne subory
{
    $iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path));
    foreach ($iterator as $filename)
    {
        if (is_dir($filename) == true)
        {
            continue;
        }
        $tests[] = $filename->getPathname();
        $tests = array_diff($tests, [".",".."]);
    }
}
else
{
    $directory = glob($path.'/*.*');
    foreach($directory as $filename)
    {
        $tests[] = $filename;
    }
}
foreach ($tests as $src)
{
    if(preg_match('/.+.src$/', $src))
    {
        $sourceFiles[] = $src;
    }
}
if (empty($sourceFiles))
{
    echo "11: Subory pre testovanie neboli najdene.\n";
    exit(11);
}

for ($i=0; $i < count($sourceFiles); $i++)
{
    $testName[$i] = preg_replace('/.src$/', "", $sourceFiles[$i]);
}

$testNumber = count($sourceFiles);

foreach ($sourceFiles as $test)
{
    $testName = str_replace(".src","", $test);              //do premennej ulozi nazvy testov postupne
    if (in_array($testName . ".rc", $tests))        //hlada ak k testu je .rc subor ak nie je tak vytvori novy a defaultne tam zapise 0
    {
        $returnCode = file_get_contents($testName . ".rc", $test);
    }
    else
    {
        $rcFile = fopen($testName . ".rc", "w");
        if (!$rcFile)
        {
            exit(11);
        }
        fwrite($rcFile, "0\n");
        $returnCode = 0;
        fclose($rcFile);
    }

    if (in_array($testName . ".in", $tests))                //hlada jednotlive subory podla nazvu testu
    {
        $input = fopen($testName . ".in", "c+");
        if (!$input)
        {
            exit(11);
        }
    }
    else{
        $input = fopen($testName . ".in", "c+");
        if (!$input)
        {
            exit(11);
        }
    }

    if (in_array($testName . ".out", $tests))
    {
        $output = fopen($testName . ".out", "c+");
        if (!$output)
        {
            exit(11);
        }
    }
    else
    {
        $output = fopen($testName . ".out", "c+");
        if (!$output)
        {
            exit(11);
        }
    }

    fclose($input);
    fclose($output);
    $ExpectedInterpretOutput = $testName.".tmp";
    $interpretInput = $testName.".src";

    if($parseonly == true)          //ak bol zadany argument pre parser iba
    {
        if(file_exists($parse) == false)
        {
            echo "11: Skript parse.php nebol najdeny.\n";
            exit(11);
        }
        //$interpretInput = $testName . ".in";
        exec("php7.3 $parse < $test", $ParseOutput, $ParseReturnCode);

        if ($returnCode != $ParseReturnCode)
        {                                                       //ulozenie html kodu do premennej pre neskorsie vypisanie
            $testFailed++;
            $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">     
                      <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                      <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                      <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                      <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                      <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                      <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                      <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$ParseReturnCode</td>
                      <td style=\"text-align:center;vertical-align:middle;width:100px;color:#ff0000;\">&#x2716</td></tr></tbody></table>";/*&#x2716*/
        }
        else
        {
            $testPassed++;
            $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                      <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                      <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                      <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                      <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                      <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                      <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                      <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$ParseReturnCode</td>
                      <td style=\"text-align:center;vertical-align:middle;width:100px;color:#00ff00;\">&#10004</td></tr></tbody></table>";
        }
    }
    if($intonly == true)            //argumetn iba pre interpret
    {

        exec("python3.6 " . $interpret . " --source=$interpretInput" . " > " . $ExpectedInterpretOutput, $interpretOutput, $interpretReturnCode);
        $interpretOutput = implode("\n", $interpretOutput);
        $temp = tmpfile();
        fwrite($temp, $interpretOutput);
        fclose($temp);
        if ($interpretReturnCode != $returnCode)
        {
            $testFailed++;
            $html .= "<table  style=\"border-collapse: collapse; border-color: black; ;margin-left: auto; margin-right: auto;\" border=\"border: 1px solid black;\">
                      <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                        <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                        <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                        <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                           <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                        <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                        <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                        <td style=\"text-align:center;vertical-align: middle;width:100px;color:#ff0000;\">&#x2716</td></tr></tbody></table>";//&#x2716
        }
        else
        {
            if ($interpretReturnCode == 0)
            {
                $output = $testName . ".out";
                exec("diff $ExpectedInterpretOutput $output", $Diff, $returnCodeDiff);
                if (count($Diff) == 0)
                {
                    $testPassed++;
                    $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\" border=\"border: 1px solid black;\">
                                <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                                <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                                <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                                <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                                <td style=\"text-align:center;vertical-align:middle;width:100px;color:#00ff00;\">&#10004</td></tr></tbody></table>";//&#10004
                }
                else
                {
                    $testFailed++;
                    $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                                <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                                <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                                <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                                <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                                <td style=\"text-align:center;vertical-align:middle;width:100px;color:#ff0000;\">&#x2716</td></tr></tbody></table>";//&#x2716
                }
            }
            else
            {
                $testPassed++;
                $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                            <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                            <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                            <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                            <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                            <td style=\"text-align:center;vertical-align:middle;width:100px;color:#00ff00;\">&#10004</td></tr></tbody></table>";//&#10004
            }
        }
    }
    if(($parseonly == false) && ($intonly == false))                //spusti postupne testovanie skriptov
    {
        if(file_exists($parse) == false)
        {
            echo "11: Skript parse.php nebol najdeny.\n";
            exit(11);
        }
        exec("php7.3 $parse < $test", $ParseOutput, $ParseReturnCode);
        if ($ParseReturnCode != 0) {
            if ($returnCode != $ParseReturnCode)
            {
                $testFailed++;
                $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                            <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                            <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                            <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                            <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$ParseReturnCode</td>
                            <td style=\"text-align:center;vertical-align:middle;width:100px;color:#ff0000;\">&#x2716</td></tr></tbody></table>";/*&#x2716*/
            }
            else
            {
                $testPassed++;
                $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                            <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                            <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td>
                             <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                             <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$ParseReturnCode</td>
                             <td style=\"text-align:center;vertical-align:middle;width:100px;color:#00ff00;\">&#10004</td></tr></tbody></table>";
            }
        }
        else
        {
            if (file_exists($interpret) == false)
            {
                echo "11: Skript interpret.py nebol najdeny.\n";
                exit(11);
            }
            $ParseOutput = implode("\n", $ParseOutput);
            $temp = tmpfile();
            fwrite($temp, $ParseOutput);
            exec("python3.6 " . $interpret . " --source=" . stream_get_meta_data($temp)['uri'] . " < " . $interpretInput . " > " . $ExpectedInterpretOutput, $interpretOutput, $interpretReturnCode);
            fclose($temp);
            if ($interpretReturnCode != $returnCode)
            {
                $testFailed++;
                $html .= "<table  style=\"border-collapse: collapse; border-color: black; ;margin-left: auto; margin-right: auto;\" border=\"border: 1px solid black;\">
                            <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                            <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                            <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                            <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                            <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                            <td style=\"text-align:center;vertical-align: middle;width:100px;color:#ff0000;\">&#x2716</td></tr></tbody></table>";//&#x2716
            }
            else
            {
                if ($interpretReturnCode == 0)
                {
                    $output = $testName . ".out";
                    exec("diff $ExpectedInterpretOutput $output", $Diff, $returnCodeDiff);
                    if (count($Diff) == 0)
                    {
                        $testPassed++;
                        $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\" border=\"border: 1px solid black;\">
                                    <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                                    <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                                    <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                                    <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                                    <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                                    <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                                    <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                                    <td style=\"text-align:center;vertical-align:middle;width:100px;color:#00ff00;\">&#10004</td></tr></tbody></table>";//&#10004
                    }
                    else
                    {
                        $testFailed++;
                        $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                                    <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                                    <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                                    <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                                    <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                                    <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                                    <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                                    <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                                    <td style=\"text-align:center;vertical-align:middle;width:100px;color:#ff0000;\">&#x2716</td></tr></tbody></table>";//&#x2716
                    }
                }
                else
                {
                    $testPassed++;
                    $html .= "<table  style=\"border-collapse: collapse; border-color: black;margin-left: auto; margin-right: auto;\"border=\"border: 1px solid black;\">
                                <tbody><tr><th style=\"text-align: center; vertical-align: middle;width: 1000px;\">Directory</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 200px;\">Expected Output</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 100px;\">My Output</th>
                                <th style=\"text-align: center; vertical-align: middle;width: 100px;\">Result</th></tr>
                                <tr><td style=\"text-align: center; vertical-align: middle;width: 100px;\">$test</td> 
                                <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$returnCode</td>
                                <td style=\"text-align: center; vertical-align: middle;width: 100px;\">$interpretReturnCode</td>
                                <td style=\"text-align:center;vertical-align:middle;width:100px;color:#00ff00;\">&#10004</td></tr></tbody></table>";//&#10004
                }
            }
            unlink($ExpectedInterpretOutput);
        }
    }
}


if(1)               //vypis HTML kodu
{
    echo "<!DOCTYPE html>
    </html>
      <head>
         <title>TESTS RESULTS FOR IPPcode19 PARSE AND INTERPRET</title>
         <meta charset=\"UTF-8\">
         <style type=\"text/css\">
            body{overflow-x: hidden;width: 1600px;margin: 0 auto;background-color: lightgrey;}
            h1{text-align: center;font-size: 50px;background-color:black;color: #ffffff}
            .content{background-color: darkgrey;padding: 1px;border-radius: 10px;margin-top: 10px; box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);}
            .result {text-align: center;font-size: 24px;}
         </style></head><body>
        <div class=\"content\">
         <h1>TESTS RESULTS FOR IPPcode19 PARSE AND INTERPRET</h1>
         <div class=\"result\">
         <span style=\"margin-right: 25px;\"><span style=\"color:#00ff00;\">SUCCESSFUL TESTS &#10004</span>: $testPassed</span>
         <span style=\"margin-right: 25px;\"><span style=\"color:#ff0000;\">FAILED TESTS &#x2716</span>: $testFailed</span>
         <span style=\"margin-right: 25px;\">NUMBER OF TESTS: $testNumber</span>
         </div>
       <hr>";
    echo "$html";
    echo "</div></body></html>";
}
