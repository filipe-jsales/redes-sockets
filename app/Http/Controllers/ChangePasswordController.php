<?php

namespace App\Http\Controllers;
// namespace App\Repositories;

use App\Http\Requests\ChangePasswordRequest;
use App\Services\ChangePasswordService;

//controle de rotas
class ChangePasswordController extends Controller
{
    protected $change_password_service;

    public function __construct(ChangePasswordService $change_password_service)
    {
        $this->change_password_service = $change_password_service;
    }
    

    public function changePassword(ChangePasswordRequest $request)
    {
        return $this->change_password_service->changePassword($request);
    }

}
