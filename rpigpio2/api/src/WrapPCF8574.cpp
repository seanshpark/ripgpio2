/*
 * Copyright 2023 saehie.park@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "WrapPCF8574.h"
#include "WrapI2C.h"

#include <cassert>
#include <iostream>

namespace rpigpio2
{

void WrapPCF8574::Init(Napi::Env &env, Napi::Object &exports)
{
  // PCF8574 chip
  // clang-format off
  Napi::Function functions = DefineClass(env, "PCF8574",
    {
      InstanceMethod("init", &WrapPCF8574::API_PCF8574_init),
      InstanceMethod("release", &WrapPCF8574::API_PCF8574_release),
      InstanceMethod("write", &WrapPCF8574::API_PCF8574_write),
    }
  );
  // clang-format on

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(functions);
  env.SetInstanceData(constructor);

  exports.Set("PCF8574", functions);
}

//--------------------------------------------------------------------------------------------------

WrapPCF8574::WrapPCF8574(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WrapPCF8574>(info)
{
  std::cout << "WrapPCF8574::WrapPCF8574 " << this << std::endl;
}

Napi::Value WrapPCF8574::API_PCF8574_init(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
    Napi::Error::New(env, "Requre 1 arguments(i2c)").ThrowAsJavaScriptException();

  auto i2cObj = info[0].As<Napi::Object>();
  WrapI2C *wrap_i2c = Napi::ObjectWrap<WrapI2C>::Unwrap(i2cObj);
  std::cout << "!!! init pcf8574 with i2c " << wrap_i2c << std::endl;

  if (!wrap_i2c->i2c().initialized())
    Napi::Error::New(env, "pcf8574: i2c not initialized").ThrowAsJavaScriptException();

  if (!this->pcf8574().init(&wrap_i2c->i2c()))
    Napi::Error::New(env, "pcf8574 init failed").ThrowAsJavaScriptException();

  return Napi::Number::New(env, 0);
}

Napi::Value WrapPCF8574::API_PCF8574_release(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  this->pcf8574().release();

  return Napi::Number::New(env, 0);
}

Napi::Value WrapPCF8574::API_PCF8574_write(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::Error::New(env, "Requre 1 argument(data)").ThrowAsJavaScriptException();
  }

  auto data = info[0].As<Napi::Number>();

  this->pcf8574().write(data.Int32Value() & 0xff);

  return Napi::Number::New(env, 0);
}

} // namespace rpigpio2
