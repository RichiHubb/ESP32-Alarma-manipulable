<?php

require 'config.php';

$obj = new BD_PDO();

// Se verifica que se hayan enviado los parámetros 
if (isset($_POST['movimiento'])) {
    $mov = $_POST['movimiento']; 

    // Se usa consulta segura con parámetros preparados
    $sql = "INSERT INTO movimiento(movimiento) VALUES(?)";
    $obj->Ejecutar_Instruccion($sql, [$mov]);

    echo "Movimiento registrado correctamente.";
} elseif (isset($_GET['disp'])) {
    // Aquí puedes manejar la lógica para GET si la necesitas
}

?>
