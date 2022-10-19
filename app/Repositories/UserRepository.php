<?php

namespace App\Repositories;

use App\Repositories\UserRepositoryInterface;
use PDO;

class UserRepository implements UserRepositoryInterface
{
    
    public function querySelectUser($conn, $request) {
        $user = $conn->prepare('SELECT * FROM users WHERE email = :email');
        $user->bindValue(':email', $request->email);
        $user->execute();

        $dados = $user->fetch(PDO::FETCH_OBJ);
        return $dados;
    }

}