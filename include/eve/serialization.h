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

#include "platform.h"
#include <string>
#include <stdexcept>

/** \addtogroup Lib
  * @{
  */

  #define _eve_field(f) eve::detail::field(#f, & serialized_class :: f),

/** TODO add documentation. */
#define eve_serializable(Class, ...)\
  struct serialization_info : public eve::detail::serialization_info_base\
  {\
    typedef Class serialized_class;\
    serialization_info() : eve::detail::serialization_info_base(#Class)\
    {\
      static const eve::detail::field s_fields[] =\
      {\
        eve_pp_map(_eve_field, __VA_ARGS__)\
      };\
      m_fields = eve::range<const eve::detail::field*>(s_fields, s_fields + sizeof(s_fields) / sizeof(eve::detail::field));\
    }\
  };

#define eve_decl_serializable\
  struct serialization_info : public eve::detail::serialization_info_base\
  {\
    serialization_info();\
  };

#define eve_def_serializable(Class, ...)\
  Class :: serialization_info::serialization_info() : eve::detail::serialization_info_base(#Class)\
  {\
    typedef Class serialized_class;\
    static const eve::detail::field s_fields[] =\
    {\
      eve_pp_map(_eve_field, __VA_ARGS__)\
    };\
    m_fields = eve::range<const eve::detail::field*>(s_fields, s_fields + sizeof(s_fields) / sizeof(eve::detail::field));\
  }

namespace eve {

/** This error is thrown by the deserializer on an invalid source or state. */
class serialization_error : public std::runtime_error
{
public:
  serialization_error(const std::string& file, eve::size line, eve::size column, const std::string& message) throw();
private:
  std::string m_file;
  eve::size m_line;
  eve::size m_column;
  std::string m_message;
};

namespace serialization
{

/** Utility class used for deserialization, it parses the source file providing methods for easy access. */
class parser
{
public:
  enum token_type
  {
    EOS,
    SYMBOL,
    NUMBER,
    STRING,
    CHARACTER
  };

  parser(std::istream* source, const std::string& file);
  void scan();
  const std::string& filename() const { return m_file; }
  eve::size line() const { return m_line; }
  eve::size column() const { return m_column; }
  token_type lookahead() const { return m_tokentype; }
  double number() const { return m_number; }
  std::string& token() { return m_token; }
  const std::string& token() const { return m_token; }
  bool is_char(char c) const;
  bool accept(char c);
  void check(token_type type) const;
  void check(char c) const;
  void expect(token_type type) { check(type); scan(); }
  void expect(char c) { check(c); scan(); }

private:
  bool is_symbol_char(bool firstchar) const;
  void scan_symbol(token_type incoming);
  void add_and_next();
  void next();
  void lexical_error(const std::string& diagnostic = "") const;
  void syntax_error(const std::string& diagnostic = "") const;
  void semanic_error(const std::string& diagnostic) const;

  std::istream* m_source;
  std::string m_file;
  eve::size m_line;
  eve::size m_column;
  token_type m_tokentype;
  double m_number;
  std::string m_token;
  char m_currchar;
};

} // eve::serialization

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Serializes in textual format the instance @p value into the stream @p output. */
template <typename T>
void serialize_as_text(const T& value, std::ostream& output);

/** Deserializes @p input in the textual format into the instance @p value. */
template <typename T>
void deserialize_as_text(std::istream& input, T& value);

/** Specialize this template class make new . */
template <class T>
class text_serializer
{
public:
  static void serialize(const T& instance, std::ostream& output, const std::string& tab);
  static void deserialize(serialization::parser& parser, T& instance);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Use this class to expose a new linear container type (like vector or list) to the serializer.
  * It requires the class to be iterable and have the "push_back" method (TODO fix this). */
template <class T>
class text_linear_container_serializer
{
public:
  static void serialize(const T& instance, std::ostream& output, const std::string& tab);
  static void deserialize(serialization::parser& parser, T& instance);
};

} // eve

#include "serialization/detail.inl"

/** }@*/
