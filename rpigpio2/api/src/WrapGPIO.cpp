/*
 * Copyright 2022 saehie.park@gmail.com
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

#include "WrapGPIO.h"

#include <cassert>
#include <iostream>

#include <unistd.h> // usleep

namespace rpigpio2
{

void WrapGPIO::Init(Napi::Env &env, Napi::Object &exports)
{
  std::cout << "WrapGPIO::Init" << std::endl;

  // clang-format off
  Napi::Function funcGPIO = DefineClass(env, "GPIO",
    {
      InstanceMethod("init", &WrapGPIO::API_GPIO_init),
      InstanceMethod("release", &WrapGPIO::API_GPIO_release),
      InstanceMethod("pin", &WrapGPIO::API_GPIO_pin),
      InstanceMethod("set", &WrapGPIO::API_GPIO_set),
      InstanceMethod("delay", &WrapGPIO::API_GPIO_delay)
    }
  );
  // clang-format on

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(funcGPIO);
  env.SetInstanceData(constructor);

  // GPIO
  exports.Set("GPIO", funcGPIO);

  Napi::Object obj_def = Napi::Object::New(env);
  obj_def.Set(Napi::String::New(env, "IN"), Napi::Number::New(env, 1));   // GPIO::PIN::IN));
  obj_def.Set(Napi::String::New(env, "ALT0"), Napi::Number::New(env, 3)); // GPIO::PIN::ALT0));
  obj_def.Set(Napi::String::New(env, "OUT"), Napi::Number::New(env, 2));  // GPIO::PIN::OUT));

  // DEF for GPIO
  exports.Set("DEF", obj_def);
}

//--------------------------------------------------------------------------------------------------

WrapGPIO::WrapGPIO(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WrapGPIO>(info)
{
  std::cout << "WrapGPIO::WrapGPIO " << this << std::endl;
}

Napi::Value WrapGPIO::API_GPIO_init(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (!this->gpio().init())
    Napi::Error::New(env, "gpio init failed").ThrowAsJavaScriptException();

  return Napi::Number::New(env, 0);
}

Napi::Value WrapGPIO::API_GPIO_release(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  this->gpio().release();

  return Napi::Number::New(env, 0);
}

// pin(number, attributes): Set pin attributes
//  number: port number
//  attributes: bitwise values
//    direction: IN or OUT
Napi::Value WrapGPIO::API_GPIO_pin(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 2)
  {
    Napi::Error::New(env, "Requre 2 arguments(port, value)").ThrowAsJavaScriptException();
  }

  auto port = info[0].As<Napi::Number>();
  auto value = info[1].As<Napi::Number>();

  std::cout << "!!! pin: " << port.Int32Value() << ": " << value.Int32Value() << std::endl;

  this->gpio().cfg(port.Int32Value(), static_cast<GPIO::PIN>(value.Int32Value()));

  return Napi::Number::New(env, 0);
}

Napi::Value WrapGPIO::API_GPIO_set(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 2)
  {
    Napi::Error::New(env, "Requre 2 arguments(port, value)").ThrowAsJavaScriptException();
  }

  auto port = info[0].As<Napi::Number>();
  auto value = info[1].As<Napi::Boolean>();

  std::cout << "!!! set: " << port.Int32Value() << ": " << (value.Value() ? "T" : "F") << std::endl;

  this->gpio().set(port.Int32Value(), value.Value());

  return Napi::Number::New(env, 0);
}

Napi::Value WrapGPIO::API_GPIO_delay(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::Error::New(env, "Requre 2 arguments(value)").ThrowAsJavaScriptException();
  }

  auto usec = info[0].As<Napi::Number>();

  usleep(usec.Int32Value());

  return Napi::Number::New(env, 0);
}

} // namespace rpigpio2
