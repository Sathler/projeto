<?php include 'conexao.php' ; 
session_start();
$port = fopen("/dev/ttyACM0", "r");
//sleep(3);

?>
<html>
    <head>
        <link rel="stylesheet" href="node_modules/bootstrap/dist/css/bootstrap.min.css">
    </head>
    <body>
        <div class="container">
            <div class="jumbotron" style="background-color: blue">
                <h1 style="color: white">Formulário de cadastro de produto</h1>
            </div>
            <form action="arduino.php" method="get">
                <div class="row">
                <div class="form-group col-6">
                    <label for="arduino_id">ID</label>
                    <input autocomplete="off" type="text" class="form-control" name="arduino_id" value=<?= '"'.fgets($port).'"'?>>
                </div>
                <div class="form-group col-6">
                    <label for="arduino_id">Nome</label>
                    <input autocomplete="off" type="text" class="form-control" name="arduino_nome">
                </div>
                </div>
                <div class="row"><h6>Dimensões</h6></div>
                <div class="row">
                <div class="form-group col-4">
                    <label for="arduino_id">Altura</label>
                    <input autocomplete="off" type="number" class="form-control" name="arduino_h">
                </div>
                <div class="form-group col-4">
                    <label for="arduino_id">Largura</label>
                    <input autocomplete="off" type="number" class="form-control" name="arduino_w" >
                </div>
                <div class="form-group col-4">
                    <label for="arduino_id">Comprimento</label>
                    <input autocomplete="off" type="number" class="form-control" name="arduino_s" >
                </div>
                </div>
                <input autocomplete="off" type="submit" value="Cadastrar">
            </form>            
        </div>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    </body>
</html>

<?php fclose($port); ?>
