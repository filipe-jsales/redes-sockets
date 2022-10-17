<?php

namespace App\Http\Controllers;
// namespace App\Repositories;

use App\Http\Requests\ChangePasswordRequest;
use App\Repositories\ChangePasswordRepository;
use Illuminate\Http\Request;
use PDO;
use PDOException;

class ChangePasswordController extends Controller
{
    
    public function update(ChangePasswordRequest $request)
    {
        // //database connection
        // $connection = ChangePasswordController::connectDataBase($request);

        // //query select user by email        
        // $querySelectUser = ChangePasswordController::querySelectUser($request);

        // $dados = $querySelectUser->fetch(PDO::FETCH_OBJ);

        // if(!isset($dados->id))
        // {
        //     return response()->json(['Email not found'], 422);
        // }

        // return ChangePasswordController::queryUpdatePassword($request);

        // ------ teste

        $changePasswordRepository = new ChangePasswordRepository($request);


    }

    //new PDO to test connection to database
    // public function connectDataBase($request)
    // {
    //     try {
    //         $conn = new PDO("mysql:host=$request->db_host;dbname=$request->db_name", $request->db_username, $request->db_pass);
    //     } catch (PDOException) {
    //         return response()->json(['Could not connect'], 432);
    //     }
    //     return $conn;
    // }

    // //Executes a query to select user by email according to the body sent
    // public function querySelectUser($request){
    //     $connection = ChangePasswordController::connectDataBase($request);

    //     $user = $connection->prepare('SELECT * FROM wp_users WHERE user_email = :user_email');
    //     $user->bindValue(':user_email', $request->email);
    //     $user->execute();

    //     return $user;
    // }

    //Executes a query to update the password according to the body sent
    // public function queryUpdatePassword($request){

    //     $connection = ChangePasswordController::connectDataBase($request);

    //     $query = $connection->prepare("UPDATE wp_users SET user_pass = ? WHERE user_email = ?");
    //     $query->execute([$request->new_pass, $request->email]);

    //     return response()->json(['Password changed', 200]);
    // }
}
