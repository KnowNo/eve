/******************************************************************************\
* This source file is part of the 'eve' framework.                             *
* (A linear, elegant, modular engine for rapid game development)               *
*                                                                              *
* The MIT License (MIT)                                                        *
*                                                                              *
* Copyright (c) 2013                                                           *
*                                                                              *
* Permission is hereby granted, free of charge, to any person obtaining a copy *
* of this software and associated documentation files (the "Software"), to deal*
* in the Software without restriction, including without limitation the rights *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    *
* copies of the Software, and to permit persons to whom the Software is        *
* furnished to do so, subject to the following conditions:                     *
*                                                                              *
* The above copyright notice and this permission notice shall be included in   *
* all copies or substantial portions of the Software.                          *
*                                                                              *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,*
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    *
* THE SOFTWARE.                                                                *
\******************************************************************************/

#pragma once

#include "text.h"
#include "storage.h"

namespace eve {

class shader : public eve::deserializable_resource<shader>
{
public:
  typedef eve::resource::ptr<shader> ptr;

  class stage : public eve::text
  {
  public:
    typedef enum class type
    {
      vertex = 0,
      tess_control,
      tess_evaluation,
      geometry,
      fragment
    } type_t;
    
    typedef eve::resource::ptr<stage, type_t> ptr;

    stage(type type);
    ~stage();

    type type() const { return m_type; }

    void load(std::istream& source) override;
    void unload() override;

  protected:
    void on_reload() override;

  private:
    void compile();

    id m_id;
    type_t m_type;
    friend class shader;
  };

public:
  shader();
  ~shader();

  void load(std::istream& source) override;
  void unload() override;

  void bind() const;
  void uniform(const std::string& name, float value);
  
protected:
  void on_reload() override;

private:
  void save_stages(eve_out const stage** stages) const;
  void attach(const stage* stage);
  void detach(const stage* stage);
  void link();

  struct location
  {
    eve::id id;
    eve::id type;
    location() : id(0), type(0) { }
    location(eve::id id, eve::id type)
      : id(id), type(type) { }
  };

  eve::id m_id;
  std::unordered_map<std::string, location> m_attributes;
  std::unordered_map<std::string, location> m_uniforms;
  stage::ptr m_vertex;
  stage::ptr m_tesscontrol;
  stage::ptr m_tesseval;
  stage::ptr m_geometry;
  stage::ptr m_fragment;
  static const size k_stages = 5;

public:
  eve_declare_serializable
};

} // eve

