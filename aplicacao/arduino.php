<?php
    require_once 'crud.php';
    session_start();
    var_dump($_GET);
    $x = cadastrar($_GET["arduino_id"], $_GET["arduino_nome"], $_GET["arduino_h"], $_GET["arduino_w"], $_GET["arduino_s"]);
    var_dump($x);
?>