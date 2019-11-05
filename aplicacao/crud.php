<?php
    require_once 'conexao.php';
    function cadastrar($id, $nome, $a, $b, $c) {
        $link = abreConexao();
        $query = "insert into Identificadores values ('$id', '$nome', $a, $b, $c)";
        var_dump($query);
        if(mysqli_query($link, $query)) {
            return true;
        }
        return false;
    }
?>