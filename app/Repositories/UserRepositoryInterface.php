<?php

namespace App\Repositories;
use App\Http\Requests\ChangePasswordRequest;

interface UserRepositoryInterface
{
    public function querySelectUser($conn, $request);
}
