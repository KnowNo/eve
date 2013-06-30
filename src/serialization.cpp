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

#include "eve/serialization.h"
#include "eve/debug.h"
#include <istream>
#include <ostream>
#include <algorithm>

using namespace eve::detail;


void eve::detail::field::serialize_as_text(const void* object, std::ostream& output, const std::string& tab) const
{
  // calculate field address from object pointer and offset
  auto ptr = static_cast<const char*>(object) + m_offset;
  m_table->serialize_as_text(ptr, output, tab);
}

void eve::detail::field::deserialize_as_text(serialization::parser& parser, void* object) const
{
  auto ptr = static_cast<char*>(object) + m_offset;
  m_table->deserialize_as_text(parser, ptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const eve::detail::field* eve::detail::serialization_info_base::field(const std::string& name) const
{
  auto field = std::find_if(m_fields.begin(), m_fields.end(), 
    [&name](const eve::detail::field& f)
  {
    return f.name() == name; 
  });
  return field;
}

const eve::detail::field* eve::detail::serialization_info_base::field(eve::size index) const
{
  eve_assert(index < num_fields());
  return &m_fields.begin()[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void eve::detail::serialize_class_as_text
  (const serialization_info_base& info, const void* instance, std::ostream& output, const std::string& tab)
{
  output << "{\n";
  const std::string& newtab = tab + "  ";
  for (auto& field : info.fields())
  {
    output << newtab << field.name() << " = ";
    field.serialize_as_text(instance, output, newtab);
    output <<  '\n';
  }
  output << tab << '}';
}


void eve::detail::deserialize_class_as_text(const serialization_info_base& info, serialization::parser& parser, void* instance)
{
  parser.expect('{');
  eve::size ifield = 0;
  while (!parser.is_char('}') && !parser.lookahead() == parser.EOS)
  {
    const field* field = nullptr;
    if (parser.lookahead() == parser.SYMBOL)
    {
      field = info.field(parser.token());
      if (!field)
        throw serialization_error(parser.filename(), parser.line(), parser.column(), "No field named '" + parser.token() + "' found in class '" + info.name()  +  "'.");
      parser.scan();
      parser.accept('='); // optional '='
    }
    else
    {
      if (ifield >= info.num_fields())
        throw serialization_error(parser.filename(), parser.line(), parser.column(), "Class '" +  info.name() + "' has lesser than " + std::to_string(ifield + 1) +  " fields (field index out of bounds).");
      field = info.field(ifield++);
    }
    field->deserialize_as_text(parser, instance);
    if (!parser.accept(';')) // optional (;|,)
      parser.accept(',');
  }
  parser.expect('}');
}

eve::serialization_error::serialization_error(const std::string& file, eve::size line, eve::size column, const std::string& message)
  : std::runtime_error("in file \"" + file + "\" at " + std::to_string(line) + ":" + std::to_string(column) + ": " + message)
  , m_file(file)
  , m_line(line)
  , m_column(column)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace eve::serialization;

parser::parser(std::istream* source, const std::string& file) 
  : m_source(source)
  , m_file(file)
  , m_line(1)
  , m_column(0)
{
  next();
  scan();
}

void parser::scan()
{
  m_tokentype = EOS;
  m_token = "";

  while (m_currchar != EOF)
  {
    switch (m_currchar)
    {
      case EOF:
        break;

      case ' ':
      case '\r':
      case '\t':
      case '\n':
        next();
        break;

      // Comments
      case '/':
      {
        next();
        switch (m_currchar)
        {
          case '/': // one line comment
            while (m_currchar != '\n')
              next();
            break;

          case '*': // block comment
          {
            unsigned int count = 1;
            while (count > 0)
            {
              next();
              switch (m_currchar)
              {
                case '\n':
                  break;
                case '/':
                  next();
                  if (m_currchar == '*')
                  {
                    ++count;
                    next();
                  }
                  break;
                case '*':
                  next();
                  if (m_currchar == '/')
                  {
                    --count;
                    next();
                  }
                  break;
                case EOF:
                  m_tokentype = EOS;
                  lexical_error("End of file found while skipping multiline comment.");
                  return;
              }
            }
            break;
          }
          default:
            lexical_error();
        }
        break;
      }

      // Numbers
      case '-':
      case '+':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        bool decimal = m_currchar == '.';
        bool numberFound = m_currchar >= '0' && m_currchar <= '9';
        while (true)
        {
          add_and_next();
          if (m_currchar == '.')
          {
            if (decimal)
            {
              lexical_error();
              next();
            } else
              decimal = true;
            continue;
          }
          if (m_currchar >= '0' && m_currchar <= '9')
            numberFound = true;
          else {
            if (numberFound)
            {
              m_tokentype = NUMBER;
              m_number = atof(m_token.c_str());
              // Apply, in case, degree to radians transformation.
              if (m_currchar == '\'')
              {
                next();
                m_number = m_number  / 180.0 * 3.14159265359;
              }
            } else
              m_tokentype = CHARACTER;
            return;
          }
        }
      }

      case '#':
        next();
        scan_symbol(SYMBOL);
        m_tokentype = SYMBOL;
        return;

      case '\'':
      {
        next();
        m_number = m_currchar;
        m_tokentype = NUMBER;
        next();
        if(m_currchar != '\'')
          lexical_error("Was scanning single character '" + std::to_string(m_number) + "'.");
        else
          next();
        return;
      }

      // Strings
      case '"':
      {
        char delimiter = m_currchar;
        bool multiline = false;
        std::string whiteSpacePrefix;
        m_tokentype = STRING;
        next();
        if (m_currchar == delimiter)
        {
          next();
          if (m_currchar == delimiter)
          {
            // Multiline string
            next();
            if (m_currchar == '\r')
              next();
            if (m_currchar != '\n')
              lexical_error("A multiline string must begin with a newline.");
            next();

            while (m_currchar == '\t' || m_currchar == ' ')
            {
              whiteSpacePrefix += m_currchar;
              next();
            }
            multiline = true;
          } else
            return; // empty string
        }

        while (true)
        {
          if (m_currchar == delimiter)
          {
            next();
            if (multiline && m_currchar == delimiter)
            {
              next();
              if (m_currchar == delimiter)
              {
                // End of multiline string
                next();
                return;
              } else
              {
                m_token += delimiter;
                m_token += delimiter;
              }
            } else
            {
              if (multiline)
                m_token += delimiter;
              else
                return;
            }

          } else if (m_currchar == EOF)
          {
            lexical_error("End of source found while parsing a string.");
            return;
          } else if (m_currchar == '\n')
          {
            add_and_next();
            for (size_t i = 0; i < whiteSpacePrefix.size(); ++i)
            {
              if (whiteSpacePrefix[i] == m_currchar)
                next();
              else
                break;
            }
          } else
            add_and_next();
        }
        return;
      }

      default:
        if (is_symbol_char(true))
        {
          add_and_next();
          scan_symbol(SYMBOL);
        } else
        {
          add_and_next();
          m_tokentype = CHARACTER;
        }
        return;
    }
  }
}

bool parser::is_char(char c) const
{
  return m_tokentype == CHARACTER && m_token[0] == c;
}

bool parser::accept(char c) 
{
  if (is_char(c))
  {
    scan();
    return true;
  }
  return false;
}

void parser::check(token_type type) const
{
  if (m_tokentype != type)
  {
    static const std::string kTypeToString[] = {"the end-of-source", "an identifier", "a number", "a string", "a character"};
    syntax_error("It was expected " + kTypeToString[(unsigned)type] + ".");
  }
}

void parser::check(char c) const 
{ 
  if (!is_char(c))
    syntax_error("It was expected '" + std::to_string(c) + "'.");
}

bool parser::is_symbol_char(bool firstchar) const
{
  return (m_currchar >= 'a' && m_currchar <= 'z') ||
    (m_currchar >= 'A' && m_currchar <= 'Z') || m_currchar == '_'    
    || 
    (!firstchar && (
    m_currchar == ':' || m_currchar == '<' || m_currchar == '>' ||
    (m_currchar >= '0' && m_currchar <= '9')));
}

void parser::scan_symbol(token_type incoming)
{
  m_tokentype = incoming;
  while (is_symbol_char(false))
  {
    m_tokentype = SYMBOL;
    add_and_next();
  }
}

void parser::add_and_next()
{
  m_token += m_currchar;
  next();
}

void parser::next()
{
  if (m_currchar == '\n')
  {
    ++m_line;
    m_column = 0;
  } else
    ++m_column;
  m_currchar = m_source->get();
}

void parser::lexical_error(const std::string& diagnostic) const
{
  std::string charString = '\'' + std::string(&m_currchar, 1) + '\'';
  if (m_currchar == EOF)
    charString = "end-of-source";
  throw eve::serialization_error(m_file, m_line, m_column, "unexpected " + charString + "found." + (diagnostic.empty() ? "" : " " + diagnostic));
}

void parser::syntax_error(const std::string& diagnostic) const
{
  std::string tokenString = m_tokentype == EOS ? "end-of-source" : m_token;
  throw eve::serialization_error(m_file, m_line, m_column, "unexpected " + tokenString + " found." + (diagnostic.empty() ? "" : " "));
}

void parser::semanic_error(const std::string& diagnostic) const
{
  throw eve::serialization_error(m_file, m_line, m_column, diagnostic);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void eve::text_serializer<std::string>::serialize(const std::string& instance, std::ostream& output, const std::string& tab)
{
  output << '\"' << instance << '\"';
}

void eve::text_serializer<std::string>::deserialize(eve::serialization::parser& parser, std::string& instance)
{
  parser.check(parser.STRING);
  instance = parser.token();
  parser.scan();
}
