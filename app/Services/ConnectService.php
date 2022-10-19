<?php

namespace App\Services;

use PDO;
use PDOException;

//logica e validação dos dados
class ConnectService
{
    public function connectDataBase($request)
    {
        try {
            $conn = new PDO("mysql:host=$request->db_host;dbname=$request->db_name", $request->db_username, $request->password);
        } catch (PDOException) {
            return response()->json(['Could not connect'], 432);
        }
        return $conn;
    }
}

