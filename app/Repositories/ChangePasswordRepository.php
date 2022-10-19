<?php

namespace App\Repositories;

class ChangePasswordRepository implements ChangePasswordRepositoryInterface
{


    public function queryUpdatePassword($conn, $request){

        $query = $conn->prepare("UPDATE users SET `password` = MD5(?) WHERE email = ?");
        $query->execute([$request->new_pass, $request->email]);

        return response()->json(['Password changed', 200]);
    }
}