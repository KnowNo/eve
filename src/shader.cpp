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

#include "eve/shader.h"
#include "eve/log.h"

#define GLEW_STATIC
#include <GL/glew.h>

using namespace eve;

static GLenum gl_shader_type[] =
{ GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
  GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER };

static std::string filter_log(text& source, const char* log)
{
  std::string out;
  const char* ch = log;
  bool scanning = false;
  std::string number;
  while (*ch != '\0')
  {
    if (scanning)
    {
      if (*ch >= '0' && *ch <= '9')
        number += *ch;
      else if (*ch == ')' || *ch == ':')
      {
        out += source.translate(atoi(number.c_str()));
        scanning = false;
        out += *ch;
      } else // any other character means we're not scanning
        scanning = false;
    } else
    {
      if (*ch == '(' || *ch == ':')
      {
        number = "";
        scanning = true;
      }
      out += *ch;
    }
    ++ch;
  }

  while (isspace(out.back()))
    out.pop_back();

  return out;
}

shader::stage::stage(type_t type)
  : m_id(0)
  , m_type(type)
{
}

shader::stage::~stage()
{
  unload();
}

void shader::stage::load(std::istream& source)
{
  text::load(source);
  
  if (m_id == 0)
    m_id = glCreateShader(gl_shader_type[(int)m_type]);

  compile();
}

void shader::stage::unload()
{
  text::unload();
  glDeleteShader(m_id);
  m_id = 0;
}

void shader::stage::on_reload()
{
  text::on_reload();
  compile();
}

void shader::stage::compile()
{
  eve_assert(m_id != 0);

  // Has the shader source really changed?
  GLint length;
  glGetShaderiv(m_id, GL_SHADER_SOURCE_LENGTH, &length);
  if (length > 0)
  {
    auto buffer = eve::make_unique_array<char>(length);
    glGetShaderSource(m_id, length, &length, buffer.get());
    if (strcmp(buffer.get(), this->str().data()) == 0)
      return;
  }

  // Set shader source
  const char* csource = this->str().c_str();
  GLsizei size = (GLsizei)this->str().size();
  glShaderSource(m_id, 1, &csource, &size);

  // Try compiling it
  glCompileShader(m_id);

  // Check the result
  GLint result;
  GLsizei infolength = 0;
  glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infolength);

  if (infolength > 0)
  {
    auto buffer = eve::make_unique_array<char>(infolength);
    GLsizei size;
    glGetShaderInfoLog(m_id, infolength, &size, buffer.get());
    if (size > 0)
    {
      std::string log = filter_log(*this, buffer.get());
      if (result == GL_FALSE) // It's an error, not a warning.
        throw std::runtime_error("an error occurred during shader compilation:\n\t" + log);
      else
        eve::log::warning("GLSL shader stage warning:\n\t" + log);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eve_define_serializable_named(eve::shader,
  (m_vertex, "vertex"),
  (m_tesscontrol, "tesscontrol"),
  (m_tesseval, "tesseval"),
  (m_geometry, "geometry"),
  (m_fragment, "fragment"))

shader::shader()
  : m_id(0)
  , m_vertex(this, eve::shader::stage::type_t::vertex)
  , m_tesscontrol(this, eve::shader::stage::type_t::tess_control)
  , m_tesseval(this, eve::shader::stage::type_t::tess_evaluation)
  , m_geometry(this, eve::shader::stage::type_t::geometry)
  , m_fragment(this, eve::shader::stage::type_t::fragment)
{
}

shader::~shader()
{
  unload();
}

void shader::load(std::istream& source)
{
  // Reset stages
  // Load stages
  deserializable_resource::load(source);

  if (!m_vertex)
    throw std::runtime_error("No vertex shader specified.");

  if (!m_fragment)
    throw std::runtime_error("No fragment shader specified.");

  if (m_id)
    return;
  m_id = glCreateProgram();
  
  const stage* stages[k_stages];
  save_stages(stages);

  for (size i = 0; i < k_stages; ++i)
    if (stages[i])
      attach(stages[i]);

  link();
}

void shader::unload()
{
  glDeleteProgram(m_id);
  m_id = 0;
}

void shader::bind() const
{
  glUseProgram(m_id);
}

void shader::on_reload()
{
  const stage* old_stages[k_stages];
  save_stages(old_stages);
  
  m_vertex.reset();
  m_tesscontrol.reset();
  m_tesseval.reset();
  m_geometry.reset();
  m_fragment.reset();

  deserializable_resource::on_reload();

  const stage* new_stages[k_stages];
  save_stages(new_stages);

  bool dirty = false;
  for (size i = 0; i < k_stages; ++i)
  {
    if (old_stages[i] != new_stages[i])
    {
      if (!new_stages[i] && old_stages[i])
        detach(old_stages[i]);
      else
        attach(new_stages[i]);
      dirty = true;
    }
  }

  if (dirty)
    link();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void shader::save_stages(const stage** stages) const
{
  stages[0] = m_vertex.get();
  stages[1] = m_tesscontrol.get();
  stages[2] = m_tesseval.get();
  stages[3] = m_geometry.get();
  stages[4] = m_fragment.get();
}

void shader::attach(const stage* stage)
{
  glAttachShader(m_id, stage->m_id);
}

void shader::detach(const stage* stage)
{
  glDetachShader(m_id, stage->m_id);
}

void shader::link()
{
  glLinkProgram(m_id);

  /* Check program status */
  int success;
  glGetProgramiv(m_id, GL_LINK_STATUS, &success);

  int length = 0;
  glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
  
  if (length > 1)
  {
    auto log = eve::make_unique_array<char>(length);

    GLsizei written;
    glGetProgramInfoLog(m_id, length, &written, &log[0]);

    if (written > 0)
    {
      if (success)
        eve::log::warning("while linking GLSL program: " + std::string(log.get()));
      else
        throw std::runtime_error("GLSL linking error: " + std::string(log.get()));
    }
  }

  int max_attr_name_length;
  int max_uniform_name_length;
  int elements;
    
  glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attr_name_length);
  glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);
  glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &elements);

  auto namebuffer = eve::make_unique_array<char>(std::max(max_attr_name_length, max_uniform_name_length));
    
  /* Read attributes */
  m_attributes.clear();
  for(int i = 0; i < elements; i++)
  {
    int length, size, location;
    GLenum type;
    glGetActiveAttrib(m_id, i, max_attr_name_length, &length, &size, &type, namebuffer.get());
    location = glGetAttribLocation(m_id, namebuffer.get());
    m_attributes[std::string(namebuffer.get())] = shader::location(location, type);
  }

  glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &elements);
  m_uniforms.clear();
  for(int i = 0; i < elements; i++)
  {
    int length, size, location;
    GLenum type;
    glGetActiveUniform(m_id, i, max_uniform_name_length, &length, &size, &type, namebuffer.get());
    location = glGetUniformLocation(m_id, namebuffer.get());
    m_uniforms[std::string(namebuffer.get())] = shader::location(location, type);
  }
}
