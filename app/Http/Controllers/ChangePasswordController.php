<?php

namespace App\Http\Controllers;
// namespace App\Repositories;

use App\Http\Requests\ChangePasswordRequest;
use App\Repositories\ChangePasswordRepository;
use PDO;

class ChangePasswordController extends Controller
{
    
    public function update(ChangePasswordRequest $request)
    {

        $changePasswordRepository = new ChangePasswordRepository($request);
        //database connection 
        $changePasswordRepository->connectDataBase($request);

        //query select user by email
        $querySelectUser = $changePasswordRepository->querySelectUser($request);
        
        $dados = $querySelectUser->fetch(PDO::FETCH_OBJ);

        if(!isset($dados->id))
        {
            return response()->json(['Email not found'], 422);
        }
        return $changePasswordRepository->queryUpdatePassword($request);
    }

}
