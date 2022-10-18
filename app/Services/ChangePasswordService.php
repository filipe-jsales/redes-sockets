<?php

namespace App\Services;

use PDO;
use PDOException;
use App\Repositories\ChangePasswordRepositoryInterface;


//logica e validação dos dados
class ChangePasswordService 
{
    protected $repository;

    public function __construct(ChangePasswordRepositoryInterface $repository)
    {
        $this->$repository = $repository;
    }

    public function connectDataBase($request)
    {
        try {
            $connection = new PDO("mysql:host=$request->db_host;dbname=$request->db_name", $request->db_username, $request->db_pass);
        } catch (PDOException) {
            return response()->json(['Could not connect'], 432);
        }
        return $connection;
    }

    public function querySelectUser($request){
        $connection = $this->connectDataBase($request);

        $user = $connection->prepare('SELECT * FROM wp_users WHERE user_email = :user_email');
        $user->bindValue(':user_email', $request->email);
        $user->execute();

        return $user;
    }

        public function queryUpdatePassword($request){

        $connection = $this->connectDataBase($request);

        $query = $connection->prepare("UPDATE wp_users SET user_pass = ? WHERE user_email = ?");
        $query->execute([$request->new_pass, $request->email]);

        return response()->json(['Password changed', 200]);
    }
}