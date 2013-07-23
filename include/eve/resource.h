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

#include "memory.h"
#include "serialization.h"
#include <vector>
#include <fstream>

/** \addtogroup Lib
  * @{
  */

namespace eve {

class resource_host
{
public:
  /** \returns host base resource path. By default it returns "". */
  virtual const std::string& path() const;

  /** \returns a range of resource_host dependant on this one.
    * \note the default behaviour is the empty set. */
  virtual range<resource_host**> dependants() { return range<resource_host**>(); }

  /** This method is called automatically when a hosted resource is reloaded. */
  virtual void on_reload() = 0;
};

/** A resource is anything loadable from disk used by the application as read
  * only data. The resource class is an abstract class that is supposed to be
  * extended by concrete resources.
  *
  * TODO resource::get
  * TODO resource::ptr
  * TODO resource::host
  */
class resource : public resource_host
{
public:
  template <class T, class Param = nulltype>
  struct helper
  {
    Param param;
    helper(const Param& param) : param(param) { }
    T* create_resource();
  };

  template <class T, class Param = nulltype>
  class ptr
  {
  public:
    ptr();
    ptr(resource_host* host);
    ptr(const Param& param);
    ptr(resource_host* host, const Param& param);
    ptr(const ptr& rhs);
    ptr(ptr&& rhs);
    ~ptr();
    const resource_host* host() const { return m_host; }
    void load(const std::string& path);
    void force_reload() { m_resource->reload(); }
    operator const T*() const { return m_resource; }
    const T* operator->() const { return m_resource; }
    ptr& operator=(const ptr& rhs);
    ptr& operator=(ptr&& rhs);

  private:
    void reset(T* res);

    resource_host* m_host;
    T* m_resource;
    helper<T, Param> m_helper;
  };

  class source
  {
  public:
    source();
    source(const std::string& path);
    void open(const std::string& path);
    std::istream& operator*() { return *m_source; }

  private:
    std::istream* m_source;
    std::ifstream m_file;
  };

public:
  resource();
  virtual ~resource();

  const std::string& path() const override { return m_path; }
  range<resource_host**> dependants() override;
  void reload();

protected:
  virtual void load(std::istream& source) = 0;
  virtual void unload();
  virtual void on_reload() override = 0;

private:
  /** \return the resource at @p path if alredy loaded, nullptr otherwise. */
  static resource* find(const std::string& path);

  /** Loads @p res, a just created instance, and inserts it in the resource library. */
  static resource* process_new(eve::unique_ptr<resource>::type res);

  void add_dependant(resource_host* dependant);
  void remove_dependant(resource_host* dependant);

  eve::size m_references;
  bool m_valid;
  std::string m_path;
  std::vector<resource_host*> m_dependants;

  template <class, class> friend class ptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class deserializable_resource : public resource
{
public:
  void load(std::istream& source) override;
  void on_reload() override;
};

} // eve

/** }@ */

#include "detail/resource.inl"
