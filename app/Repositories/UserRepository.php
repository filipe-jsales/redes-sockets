<?php

namespace App\Repositories;

use App\Repositories\UserRepositoryInterface;
use PDO;

class UserRepository implements UserRepositoryInterface
{
    
    public function querySelectUser($connection, $request) {
        $user = $connection->prepare('SELECT * FROM users WHERE email = :email');
        $user->bindValue(':email', $request->email);
        $user->execute();

        $dados = $user->fetch(PDO::FETCH_OBJ);
        return $dados;
    }

}