<?php

require 'config.php';

$obj = new BD_PDO();

// se verifica que se hayan enviado los parametros 
$mov = $_POST['movimiento'];



if(isset($_POST['movimiento']))
{
	// Esta sección sería si se desea enviar los datos por el método POST
	$obj->Ejecutar_Instruccion("Insert into movimiento(movimiento) values($mov)");
}
elseif (isset($_GET['disp'])) 
{
	
}
?>
