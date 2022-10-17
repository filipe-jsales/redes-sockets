<?php

namespace App\Repositories;

use App\Http\Requests\ChangePasswordRequest;

interface ChangePasswordRepositoryInterface
{
    public function __construct(ChangePasswordRequest $request);
    public function connectDataBase($request);
    public function querySelectUser($request);
    public function queryUpdatePassword($request);
}
