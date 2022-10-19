<?php

namespace App\Http\Requests;

use Illuminate\Foundation\Http\FormRequest;

class ChangePasswordRequest extends FormRequest
{
    /**
     * Determine if the user is authorized to make this request.
     *
     * @return bool
     */
    public function authorize()
    {
        return true;
    }

    /**
     * Get the validation rules that apply to the request.
     *
     * @return array<string, mixed>
     */
    public function rules()
    {
        return [
            'db_host' => 'required',
            'db_name' => 'required',
            'db_username' => 'required',
            'password' => 'required',
            'email' => 'required',
            'new_pass' => 'required'
        ];
    }
}
