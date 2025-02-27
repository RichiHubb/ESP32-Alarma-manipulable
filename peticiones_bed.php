<?php

require 'config.php';

header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: X-API-KEY, Origin, X-Requested-With, Content-Type, Accept, Access-Control-Request-Method");
header("Access-Control-Allow-Methods: GET, POST");
header("Allow: GET, POST");

$obj = new BD_PDO();

// se verifica que se hayan enviado los parametros 
$mov = $_POST['movimiento'];



if(isset($_POST['movimiento']))
{
	// Esta sección sería si se desea enviar los datos por el método POST
	$obj->Ejecutar_Instruccion("Insert into movimientos(deteccion) values($mov)");
}
elseif (isset($_GET['movimiento'])) 
{
	$mov = $_GET['movimiento'];

	$obj->Ejecutar_Instruccion("Insert into movimientos(deteccion) values($mov)");
}
?>
