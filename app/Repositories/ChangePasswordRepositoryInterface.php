<?php

namespace App\Repositories;

interface ChangePasswordRepositoryInterface
{
   
    public function queryUpdatePassword($conn, $request);
}
