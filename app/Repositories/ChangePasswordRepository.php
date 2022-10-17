<?php

namespace App\Repositories;

use App\Http\Controllers\ChangePasswordController;
use PDO;
use PDOException;
use Illuminate\Http\Request;
use App\Http\Requests\ChangePasswordRequest;

class ChangePasswordRepository implements ChangePasswordRepositoryInterface
{
    protected $request;

    public function __construct(ChangePasswordRequest $request)
    {
        $this->$request = $request;
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

    public function querySelectUser($request){
        $connection = $this->connectDataBase($request);

        $user = $connection->prepare('SELECT * FROM wp_users WHERE user_email = :user_email');
        $user->bindValue(':user_email', $request->email);
        $user->execute();

        return $user;
    }
}