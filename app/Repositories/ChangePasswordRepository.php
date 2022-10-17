<?php

namespace App\Repositories;
use PDO;
use PDOException;

class ChangePasswordRepository implements ChangePasswordRepositoryInterface
{
    protected $model;

    public function __construct($model)
    {
        $this->$model = $model;
    }

    public function connectDataBase($request)
    {
        try {
            $conn = new PDO("mysql:host=$request->db_host;dbname=$request->db_name", $request->db_username, $request->db_pass);
        } catch (PDOException) {
            return response()->json(['Could not connect'], 432);
        }
        return $conn;
    }

}