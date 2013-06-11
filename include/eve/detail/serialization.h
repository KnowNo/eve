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

#include "../platform.h"
#include "../type_traits.h"
#include "../range.h"
#include "../singleton.h"
#include <ostream>

namespace eve {

namespace detail {

class field
{
public:
  template<class T, class Q>
  field(const std::string name, Q T::*member)
    : m_name(name)
  {
    static const calltable s_table = {
      (size)&(((T*)nullptr)->*member), // offset of member in class
      &field::serialize_as_text<Q>,
      &field::deserialize_as_text<Q>
    };
    m_table = &s_table;
  }

  const std::string& name() const { return m_name; }

  void serialize_as_text(const void* object, std::ostream& output, const std::string& tab) const;
  void deserialize_as_text(serialization::parser& parser, void* object) const;

private:
  struct calltable
  {
    size offset;
    void (*serialize_as_text)(const void* ptr, std::ostream& output, const std::string&);
    void (*deserialize_as_text)(serialization::parser& parser, void* object);
  };

  template<class Q>
  static void serialize_as_text(const void* ptr, std::ostream& output, const std::string& tab)
  {
    auto& instance = *static_cast<const Q*>(ptr);
    eve::text_serializer<Q>::serialize(instance, output, tab);
  }

  template<class Q>
  static void deserialize_as_text(serialization::parser& parser, void* ptr)
  {
    auto& instance = *static_cast<Q*>(ptr);
    eve::text_serializer<Q>::deserialize(parser, instance);
  }

  std::string m_name;
  const calltable* m_table;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

eve_gen_has_member_type(serialization_info)

struct serialization_info_base
{
public:
  typedef range<const detail::field*> fields_range;
  serialization_info_base(const std::string& name) : m_name(name) { }
  const std::string& name() const { return m_name; }
  const fields_range& fields() const { return m_fields; }
  eve::size num_fields() const { return m_fields.end() - m_fields.begin(); }
  const detail::field* field(const std::string& name) const;
  const detail::field* field(eve::size index) const;

protected:
  fields_range m_fields;

private:
  std::string m_name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void eve_dllexport serialize_class_as_text(const serialization_info_base& info, const void* instance
                                           , std::ostream& output, const std::string& tab);

void eve_dllexport deserialize_class_as_text(const serialization_info_base& info, serialization::parser& parser,
                                             void* instance);

template <class T, bool t_is_arithmetic>
struct text_serializer_helper
{
  static_assert(has_serialization_info<T>::value, "eve error: T is not serializable.");
  static void serialize(const T& instance, std::ostream& output, const std::string& tab)
  {
    serialize_class_as_text(
      eve::singleton<T::serialization_info>::ref(), &instance, output, tab);
  }

  static void deserialize(serialization::parser& parser, T& instance)
  {
    deserialize_class_as_text(
      eve::singleton<T::serialization_info>::ref(), parser, &instance);
  }
};

// Arithmetic type serialization
template <class T>
struct text_serializer_helper<T, true>
{
  static void serialize(const T& instance, std::ostream& output, const std::string& tab)
  {
    output << instance;
  }

  static void deserialize(serialization::parser& parser, T& instance)
  {
    parser.check(serialization::parser::NUMBER);
    instance = static_cast<T>(parser.number());
    parser.scan();
  }
};

} // detail

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void text_serializer<T>::serialize(const T& instance, std::ostream& output, const std::string& tab)
{
  eve::detail::text_serializer_helper<T, std::is_arithmetic<T>::value>::serialize(instance, output, tab);
}

template <class T>
void text_serializer<T>::deserialize(serialization::parser& parser, T& instance)
{
  eve::detail::text_serializer_helper<T, std::is_arithmetic<T>::value>::deserialize(parser, instance);
}

template <typename T>
void serialize_as_text(const T& instance, std::ostream& output)
{
  eve::text_serializer<T>::serialize(instance, output, "");
}

template <typename T>
void deserialize_as_text(std::istream& input, T& value)
{
  eve::serialization::parser parser(&input, "stream");
  eve::text_serializer<T>::deserialize(parser, value);
}


} // eve
