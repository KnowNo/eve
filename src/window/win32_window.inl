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

#include "eve/window.h"
#include "eve/math/detail/vec2.h"

#include <GL/glew.h>
#include <GL/wglew.h>

using namespace eve;

static const TCHAR k_classname[50] = TEXT("EVE_WINDOW_CLASS");
static struct window_info
{
  HINSTANCE module;
  WNDCLASS wndclass;

  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB_proc;
  PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB_proc;
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB_proc;
  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT_proc;

} s_window_info;

// Declare main Win32 callbacks.
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////////////////////////

void hide_system_cursor(bool hide)
{
  auto flag = hide ? TRUE : FALSE;
  while (ShowCursor(flag) > -1)
    ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace eve {

void initialize_window()
{
  // Retrieve current module of this process
  s_window_info.module = GetModuleHandle(nullptr);

  // Initialize the eve application window class
  WNDCLASSEX winclass;
  winclass.cbSize = sizeof(WNDCLASSEX);
  winclass.lpszClassName = k_classname;
  winclass.lpfnWndProc   = (WNDPROC)WndProc;
  winclass.hInstance     = s_window_info.module;
  winclass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  winclass.hIcon         = LoadIcon(nullptr, IDI_WINLOGO);
  winclass.hbrBackground = nullptr;
  winclass.lpszMenuName  = nullptr;
  winclass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; 
  winclass.cbClsExtra    = 0;
  winclass.cbWndExtra    = 0;
  winclass.hIconSm       = nullptr;

  // Try registering this class.
  if (!RegisterClassEx(&winclass))
    throw std::runtime_error("Could not register the Win32 Window class.");

  // Preliminary stuff: needed only to obtain a preliminary context & initialize wgl functions.
  HWND handle = CreateWindowEx(
    WS_EX_APPWINDOW,
    k_classname,
    "", 
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    0, 0, 1, 1,
    nullptr, // Parent window
    nullptr, // menu
    s_window_info.module,
    nullptr);

  if (!handle)
    throw std::runtime_error("Could not create the preliminary window. Error code: " + std::to_string(GetLastError()) + ".");

  // Retrieve the device context from the window
  HDC hDC = GetDC(handle);

  if (!hDC)
    throw std::runtime_error("Could not obtain a device context for the preliminary window.");
    
  // Try setting an empty pixel format
  PIXELFORMATDESCRIPTOR pfd;
  if (!SetPixelFormat(hDC, 1, &pfd))
    throw std::runtime_error("Could not set the preliminary pixel format.");

  // Remarks: are we shure wglCreateContext will create a context of the latest version of OpenGL?
  HGLRC context = wglCreateContext(hDC);
  if (!context)
    throw std::runtime_error("Could not create the preliminary opengl context.");

  // Activate the preliminary context
  if (!wglMakeCurrent(hDC, context))
    throw std::runtime_error("Could not acivate the preliminary opengl context.");

  // Finally retrieve version of OpenGL available on this machine.
  int glmajor, glminor;
  glGetIntegerv(GL_MAJOR_VERSION, &glmajor);
  glGetIntegerv(GL_MINOR_VERSION, &glminor);

  const auto required_major = eve::window::config::glmajor;
  const auto required_minor = eve::window::config::glminor;

  if (required_major > glmajor 
    || (required_major == glmajor && required_minor > glminor))
    throw std::runtime_error("This system does not support requested OpenGL " 
      + std::to_string(required_major) + "." + std::to_string(required_minor) + ".");

  // Retrieve a few foundamental functions
  s_window_info.wglChoosePixelFormatARB_proc = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  s_window_info.wglGetPixelFormatAttribivARB_proc = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
  s_window_info.wglCreateContextAttribsARB_proc = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  s_window_info.wglSwapIntervalEXT_proc = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

  // Destroy the preliminary window/cotext
  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(context);
  ReleaseDC(handle, hDC);
  DestroyWindow(handle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void terminate_window()
{
  UnregisterClass(k_classname, s_window_info.module);
  s_window_info.module = nullptr;
  hide_system_cursor(false);
}

} // eve

////////////////////////////////////////////////////////////////////////////////////////////////////

class window_impl;

struct user_data
{
  eve::window::event* event;
  window_impl* window;
};

class window_impl
{
public:
  eve::vec2i old_cursor;

  void open(const eve::window::config& config, const char* title)
  {
    // Calculate window dimensions
    uint16 x = (GetSystemMetrics(SM_CXSCREEN) - config.width) / 2;
    uint16 y = (GetSystemMetrics(SM_CYSCREEN) - config.height) / 2;

    RECT winrect;
    winrect.left   = x;
    winrect.right  = x + config.width;
    winrect.top    = y;
    winrect.bottom = y + config.height;

    // Show the cursor by default
    ShowCursor(TRUE);

    // Setup window style
    DWORD windstyle;
    DWORD extstyle;
    windstyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    extstyle  = WS_EX_APPWINDOW;

    // Setup window dimensions and style
    AdjustWindowRectEx(&winrect, windstyle, FALSE, extstyle);

    // Adjust style flags to fullscreen and resizable values.
    if (config.fullscreen)
      windstyle |= WS_POPUP | WS_VISIBLE;
    else
    {
      windstyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
      if (config.resizable)
      {
        windstyle |= WS_MAXIMIZEBOX | WS_SIZEBOX;
        extstyle |= WS_EX_WINDOWEDGE;
      }
    }

    // If the window has to be opened in fullscreen we need to call ChangeDisplaySettings accordingly.
    if (config.fullscreen)
    {
      DEVMODE dm;
      ZeroMemory(&dm, sizeof(dm));
      dm.dmSize = sizeof(dm);
      dm.dmPelsWidth  = config.width;
      dm.dmPelsHeight  = config.height;
      dm.dmBitsPerPel  = config.bpp;
      dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
      
      if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        throw std::runtime_error("Could not set the selected display resolution "
          + std::to_string(config.width) + "x" + std::to_string(config.height) + ".");

      x = 0;
      y = 0;
    }

    // Now create the real window.
    AdjustWindowRectEx(&winrect, windstyle, FALSE, extstyle);
    
    //Adjust for adornments
    int window_width = winrect.right - winrect.left;
    int window_height = winrect.bottom - winrect.top;

    m_handle = CreateWindowEx(extstyle, k_classname, title,
                                  windstyle,
                                  x, y, window_width, window_height,
                                  nullptr, // Parent window
                                  nullptr, // menu
                                  s_window_info.module, nullptr); // pass this to WM_CREATE

    if (!m_handle)
      throw std::runtime_error("Could not create the window");

    // Retrieve the window device context
    m_DC = GetDC(m_handle);
    if (!m_DC)
      throw std::runtime_error("Could not obtain a device context for the window");
    
    int pixAttribs[] = { WGL_SUPPORT_OPENGL_ARB, 1,
                         WGL_DRAW_TO_WINDOW_ARB, 1,
                         WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
                         WGL_RED_BITS_ARB,       (int)config.bpp / 4,
                         WGL_GREEN_BITS_ARB,     (int)config.bpp / 4,
                         WGL_BLUE_BITS_ARB,      (int)config.bpp / 4,
                         WGL_ALPHA_BITS_ARB,     (int)config.bpp / 4,
                         WGL_DEPTH_BITS_ARB,     (int)config.depth_bits,
                         WGL_STENCIL_BITS_ARB,   (int)config.stencil_bits,
                         WGL_DOUBLE_BUFFER_ARB,   GL_TRUE,
                         WGL_SAMPLE_BUFFERS_ARB, (config.fsaa) ? 1 : 0,
                         WGL_SAMPLES_ARB,        (int)config.fsaa,
                         WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 0};
    
    int pixelformat = -1;
    UINT num_pixel_format = 0;
    s_window_info.wglChoosePixelFormatARB_proc(m_DC, pixAttribs, nullptr, 1, &pixelformat, &num_pixel_format);

    // If pixelformat < 0 it means that it couldnt obtain a pixel format compatible to requested one.
    if (pixelformat < 0)
      throw std::runtime_error("Could not obtain a pixel format compatible to requested parameters.");

    int attrib[] = { WGL_SAMPLES_ARB };
    int num_results = 0;
    s_window_info.wglGetPixelFormatAttribivARB_proc(m_DC, pixelformat, 0, 1, attrib, &num_results);

    /*if (num_results != config.fsaa)
      BI_LOG_WARNING(SB << "Asked for " << config.fsaa << "x fsaa, but got " << num_results << "x");*/

    PIXELFORMATDESCRIPTOR pfd;
    if (!SetPixelFormat(m_DC, pixelformat, &pfd))
      throw std::runtime_error("Failed to set the pixel format.");

    // Prepare the attributes used for creating the GL context
    GLint attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, config.glmajor,
                        WGL_CONTEXT_MINOR_VERSION_ARB, config.glminor,
                        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                        WGL_CONTEXT_FLAGS_ARB,
                        #ifndef EVE_RELEASE
                        WGL_CONTEXT_DEBUG_BIT_ARB,
                        #else
                        0,
                        #endif
                        0};

    m_context = s_window_info.wglCreateContextAttribsARB_proc(m_DC, 0, attribs);
    if (!m_context)
      throw std::runtime_error("Could not create the OpenGL context.");
    
    if (!wglMakeCurrent(m_DC, m_context))
      throw std::runtime_error("Could not acivate the OpenGL context.");

    if (config.vsync)
      s_window_info.wglSwapIntervalEXT_proc(1);

    ShowWindow(m_handle, SW_SHOW);
    SetForegroundWindow(m_handle);
    SetFocus(m_handle);

    #ifndef HID_USAGE_PAGE_GENERIC
    #define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
    #endif
    #ifndef HID_USAGE_GENERIC_MOUSE
    #define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
    #endif

    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = m_handle;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    SetWindowLongPtr(m_handle, GWLP_USERDATA, 0);

    if (glewInit() != GLEW_OK)
      throw std::runtime_error("Failed to initialize GL extensions.");

    m_flags.set(flags::open);
    m_flags.set(flags::fullscreen, config.fullscreen);
  }

  void title(const std::string& title)
  {
    SetWindowText(m_handle, title.c_str());
  }

  void poll(eve::window::event& e)
  {
    user_data data;
    data.event = &e;
    data.window = this;
    e.type = e.NONE;
    SetWindowLongPtr(m_handle, GWLP_USERDATA, (LONG)&data);
    MSG msg;
    do 
    {
      if (PeekMessage(&msg, m_handle, 0, 0, PM_REMOVE) == FALSE)
        return;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } while(e.type == e.NONE);
  }

  void activate()
  {
    wglMakeCurrent(m_DC, m_context);
  }

  void display()
  {
    SwapBuffers(m_DC);
  }

  void close()
  {
    if (!m_handle)
      return;

    if (fullscreen())
      ChangeDisplaySettings(nullptr, 0);

    if (cursor_hidden())
      hide_system_cursor(false);

    if (m_context)
    {
      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(m_context);
      m_context = nullptr;
    }

    if(m_DC)
    {
      ReleaseDC(m_handle, m_DC);
      m_DC = nullptr;
    }

    if (m_handle)
    {
      DestroyWindow(m_handle);
      m_handle = nullptr;;
    }
    
    m_flags.reset();
  }

  bool opened() const
  {
    return m_flags.isset(flags::open);
  }

  bool fullscreen() const
  {
    return m_flags.isset(flags::fullscreen);
  }

  bool cursor_hidden() const
  {
    return m_flags.isset(flags::cursor_hidde);
  }

private:
  enum class flags : eve::uint8
  {
    open = eve_bit(0),
    cursor_hidde = eve_bit(1),
    fullscreen = eve_bit(2),
  };

  HWND m_handle;
  HDC m_DC;
  HGLRC m_context;
  eve::flagset<flags> m_flags; // open? cursor captured? fullscreen?
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef DELETE

/** Translates a Windows key to the corresponding key */
static eve::key translate_key(WPARAM wParam, LPARAM lParam)
{
  using namespace eve;

  // Simulate always on numpad.
  if ((HIWORD(lParam) & 0x100) == 0)
  {
    switch (MapVirtualKey(HIWORD(lParam) & 0xFF, 1))
    {
      case VK_INSERT:   return key::NUMPAD_0;
      case VK_END:      return key::NUMPAD_1;
      case VK_DOWN:     return key::NUMPAD_2;
      case VK_NEXT:     return key::NUMPAD_3;
      case VK_LEFT:     return key::NUMPAD_4;
      case VK_CLEAR:    return key::NUMPAD_5;
      case VK_RIGHT:    return key::NUMPAD_6;
      case VK_HOME:     return key::NUMPAD_7;
      case VK_UP:       return key::NUMPAD_8;
      case VK_PRIOR:    return key::NUMPAD_9;
      case VK_DIVIDE:   return key::NUMPAD_DIVIDE;
      case VK_MULTIPLY: return key::NUMPAD_MULTIPLY;
      case VK_SUBTRACT: return key::NUMPAD_SUBTRACT;
      case VK_ADD:      return key::NUMPAD_ADD;
      case VK_DELETE:   return key::NUMPAD_DECIMAL;
      default:          break;
    }
  }

  // Special handling for system keys
  switch (wParam)
  {
    case VK_ESCAPE:        return key::ESCAPE;
    case VK_TAB:           return key::TAB;
    case VK_BACK:          return key::BACKSPACE;
    case VK_HOME:          return key::HOME;
    case VK_END:           return key::END;
    case VK_PRIOR:         return key::PAGE_UP;
    case VK_NEXT:          return key::PAGE_DOWN;
    case VK_INSERT:        return key::INSERT;
    case VK_DELETE:        return key::DELETE;
    case VK_LEFT:          return key::LEFT;
    case VK_UP:            return key::UP;
    case VK_RIGHT:         return key::RIGHT;
    case VK_DOWN:          return key::DOWN;
    case VK_F1:            return key::F1;
    case VK_F2:            return key::F2;
    case VK_F3:            return key::F3;
    case VK_F4:            return key::F4;
    case VK_F5:            return key::F5;
    case VK_F6:            return key::F6;
    case VK_F7:            return key::F7;
    case VK_F8:            return key::F8;
    case VK_F9:            return key::F9;
    case VK_F10:           return key::F10;
    case VK_F11:           return key::F11;
    case VK_F12:           return key::F12;
    case VK_F13:           return key::F13;
    case VK_F14:           return key::F14;
    case VK_F15:           return key::F15;
    case VK_F16:           return key::F16;
    case VK_F17:           return key::F17;
    case VK_F18:           return key::F18;
    case VK_F19:           return key::F19;
    case VK_F20:           return key::F20;
    case VK_F21:           return key::F21;
    case VK_F22:           return key::F22;
    case VK_F23:           return key::F23;
    case VK_F24:           return key::F24;
    case VK_NUMLOCK:       return key::NUM_LOCK;
    case VK_CAPITAL:       return key::CAPS_LOCK;
    case VK_SNAPSHOT:      return key::PRINT_SCREEN;
    case VK_SCROLL:        return key::SCROLL_LOCK;
    case VK_PAUSE:         return key::PAUSE;
    case VK_LWIN:          return key::LEFT_SUPER;
    case VK_RWIN:          return key::RIGHT_SUPER;
    case VK_APPS:          return key::MENU;

      // Numpad
    case VK_NUMPAD0:       return key::NUMPAD_0;
    case VK_NUMPAD1:       return key::NUMPAD_1;
    case VK_NUMPAD2:       return key::NUMPAD_2;
    case VK_NUMPAD3:       return key::NUMPAD_3;
    case VK_NUMPAD4:       return key::NUMPAD_4;
    case VK_NUMPAD5:       return key::NUMPAD_5;
    case VK_NUMPAD6:       return key::NUMPAD_6;
    case VK_NUMPAD7:       return key::NUMPAD_7;
    case VK_NUMPAD8:       return key::NUMPAD_8;
    case VK_NUMPAD9:       return key::NUMPAD_9;
    case VK_DIVIDE:        return key::NUMPAD_DIVIDE;
    case VK_MULTIPLY:      return key::NUMPAD_MULTIPLY;
    case VK_SUBTRACT:      return key::NUMPAD_SUBTRACT;
    case VK_ADD:           return key::NUMPAD_ADD;
    case VK_DECIMAL:       return key::NUMPAD_DECIMAL;

    // Special keys
    case VK_SHIFT:
    {
      const DWORD scancode = MapVirtualKey(VK_RSHIFT, 0);
      if ((DWORD) ((lParam & 0x01ff0000) >> 16) == scancode)
        return key::RIGHT_SHIFT;
      return key::LEFT_SHIFT;
    }

    case VK_CONTROL:
    {
      MSG next;
      DWORD time;

      if (lParam & 0x01000000)
        return key::RIGHT_CTRL;

      // Use GLFW trick: "Alt Gr" sends LCTRL, then RALT. We only
      // want the RALT message, so we try to see if the next message
      // is a RALT message. In that case, this is a false LCTRL!
      time = GetMessageTime();

      if (PeekMessage(&next, NULL, 0, 0, PM_NOREMOVE))
      {
        if (next.message == WM_KEYDOWN || next.message == WM_SYSKEYDOWN 
          || next.message == WM_KEYUP || next.message == WM_SYSKEYUP)
        {
          if (next.wParam == VK_MENU && (next.lParam & 0x01000000) && next.time == time)
          {
            // Next message is a RALT down message, which
            // means that this is not a proper LCTRL message
            return key::INVALID;
          }
        }
      }

      return key::LEFT_CTRL;
    }

    case VK_MENU:
      if (lParam & 0x01000000)
        return key::RIGHT_ALT;
      return key::LEFT_ALT;

      // The ENTER keys require special handling
    case VK_RETURN:
      if (lParam & 0x01000000)
        return key::NUMPAD_ENTER;
      return key::ENTER;

    // Printable keys
    case VK_SPACE: return key::SPACE;
    case 0x30: return key::KEY_0;
    case 0x31: return key::KEY_1;
    case 0x32: return key::KEY_2;
    case 0x33: return key::KEY_3;
    case 0x34: return key::KEY_4;
    case 0x35: return key::KEY_5;
    case 0x36: return key::KEY_6;
    case 0x37: return key::KEY_7;
    case 0x38: return key::KEY_8;
    case 0x39: return key::KEY_9;
    case 0x41: return key::A;
    case 0x42: return key::B;
    case 0x43: return key::C;
    case 0x44: return key::D;
    case 0x45: return key::E;
    case 0x46: return key::F;
    case 0x47: return key::G;
    case 0x48: return key::H;
    case 0x49: return key::I;
    case 0x4A: return key::J;
    case 0x4B: return key::K;
    case 0x4C: return key::L;
    case 0x4D: return key::M;
    case 0x4E: return key::N;
    case 0x4F: return key::O;
    case 0x50: return key::P;
    case 0x51: return key::Q;
    case 0x52: return key::R;
    case 0x53: return key::S;
    case 0x54: return key::T;
    case 0x55: return key::U;
    case 0x56: return key::V;
    case 0x57: return key::W;
    case 0x58: return key::X;
    case 0x59: return key::Y;
    case 0x5A: return key::Z;
    case 0xBD: return key::MINUS;
    case 0xBB: return key::EQUAL;
    case 0xDB: return key::LEFT_BRACKET;
    case 0xDD: return key::RIGHT_BRACKET;
    case 0xDC: return key::BACKSLASH;
    case 0xBA: return key::SEMICOLON;
    case 0xDE: return key::APOSTROPHE;
    case 0xC0: return key::GRAVE_ACCENT;
    case 0xBC: return key::COMMA;
    case 0xBE: return key::PERIOD;
    case 0xBF: return key::SLASH;
    case 0xDF: return key::WORLD_1;
    case 0xE2: return key::WORLD_2;
    default: break;
  }

  // No matching translation was found
  return key::UNKNOWN;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM  wParam, LPARAM lParam)
{
  auto data = reinterpret_cast<user_data*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  eve::window::event* e = nullptr;
  
  if (!data || !(e = data->event))
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  
  switch(uMsg)
  {
    case WM_CREATE:
    {
      CREATESTRUCT* cs = (CREATESTRUCT*) lParam;
      SetWindowLongPtr(hWnd, 0, (LONG_PTR) cs->lpCreateParams);
      break;
    }

    case WM_CLOSE:
      data->event->type = eve::window::event::QUIT;
      break;

    case WM_SYSCOMMAND:
    {
      switch (wParam & 0xfff0)
      {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
        {
          if (data->window->fullscreen())
          {
              // We are running in fullscreen mode, so disallow
              // screen saver and screen blanking
              return 0;
          }
          else
              break;
        }

        // User trying to access application menu using ALT?
        case SC_KEYMENU:
            return 0;
      }
      break;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      e->type = window::event::KEYDOWN;
      e->key.repetitions = lParam & 0x7FFF;
      e->key.code = translate_key(wParam, lParam);
      e->key.scancode = (lParam >> 16) & 0xff;
      if (e->key.code == eve::key::INVALID)
        e->type = window::event::NONE;
      break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
      e->type = window::event::KEYUP;
      e->key.repetitions = 0;
      e->key.code = translate_key(wParam, lParam);
      e->key.scancode = (lParam >> 16) & 0xff;
       if (e->key.code == eve::key::INVALID)
        e->type = window::event::NONE;
      break;

    case WM_MOUSEMOVE:
    {
      eve::vec2i new_cursor;
      new_cursor.x = LOWORD(lParam);
      new_cursor.y = HIWORD(lParam);
      
      if (new_cursor != data->window->old_cursor)
      {
        eve::vec2i cursor;

        if (data->window->cursor_hidden())
        {
          //if (_glfw.focusedWindow != window)
          //    return 0;
          cursor = new_cursor - data->window->old_cursor;
        }
        else cursor = new_cursor;

        data->window->old_cursor = new_cursor;
        //window->win32.cursorCentered = GL_FALSE;

        e->type = window::event::MOUSEMOTION;
        e->mouse.cursor = new_cursor;
      }
      return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    {
      SetCapture(hWnd);
      e->type = window::event::MOUSEDOWN;

      if (uMsg == WM_LBUTTONDOWN)
        e->mouse.button = mouse::button::LEFT;
      else if (uMsg == WM_RBUTTONDOWN)
        e->mouse.button = mouse::button::RIGHT;
      else if (uMsg == WM_MBUTTONDOWN)
        e->mouse.button = mouse::button::MIDDLE;
      else
      {
        if (HIWORD(wParam) == XBUTTON1)
          e->mouse.button = mouse::button::BUTTON_4;
        else if (HIWORD(wParam) == XBUTTON2)
          e->mouse.button = mouse::button::BUTTON_5;
        return TRUE;
      }
      return 0;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
      ReleaseCapture();
      e->type = window::event::MOUSEUP;

      if (uMsg == WM_LBUTTONUP)
        e->mouse.button = mouse::button::LEFT;
      else if (uMsg == WM_RBUTTONUP)
        e->mouse.button = mouse::button::RIGHT;
      else if (uMsg == WM_MBUTTONUP)
        e->mouse.button = mouse::button::MIDDLE;
      else
      {
        if (HIWORD(wParam) == XBUTTON1)
          e->mouse.button = mouse::button::BUTTON_4;
        else if (HIWORD(wParam) == XBUTTON2)
          e->mouse.button = mouse::button::BUTTON_5;
        return TRUE;
      }
      return 0;
    }

    case WM_SIZE:
      e->size.width = LOWORD(lParam);
      e->size.height = HIWORD(lParam);
      break;

    case WM_EXITSIZEMOVE:
      e->type = window::event::SIZE;
      break;

    default: break;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
