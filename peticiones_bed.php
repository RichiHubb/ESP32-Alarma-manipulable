<?php
require 'config.php';

header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: X-API-KEY, Origin, X-Requested-With, Content-Type, Accept, Access-Control-Request-Method");
header("Access-Control-Allow-Methods: GET, POST");
header("Allow: GET, POST");

$obj = new BD_PDO();

// Verificar si se enviaron los parámetros por POST o GET
if (isset($_POST['movimiento'])) {
    // Si se enviaron por POST
    $mov = $_POST['movimiento'];
    $obj->Ejecutar_Instruccion("INSERT INTO movimientos(deteccion) VALUES ($mov)");
} elseif (isset($_GET['movimiento'])) {
    // Si se enviaron por GET
    $mov = $_GET['movimiento'];
    $obj->Ejecutar_Instruccion("INSERT INTO movimientos(deteccion) VALUES ($mov)");
} else {
    // Si no se enviaron los parámetros
    echo "Error: Parámetro 'movimiento' no encontrado.";
}
?>
