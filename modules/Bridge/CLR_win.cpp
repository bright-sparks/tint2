#include <v8.h>
#include <node.h>
#include <nan.h>
#include <stdio.h>
#include <vcclr.h>
#include <vector>
#include <msclr/marshal.h>
#include <windows.h>
#include <Dwmapi.h>
#include <Shellapi.h>
#include "../AutoLayoutPanel.cpp"

#using <mscorlib.dll>
#using <System.dll>
#using <System.Core.dll>
#using <System.Xaml.dll>
#using <WPF/WindowsBase.dll>
#using <WPF/PresentationCore.dll>
#using <WPF/PresentationFramework.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#include <msclr/marshal.h>
 
using namespace msclr::interop;
using namespace v8;
using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace System::Reflection::Emit;
using namespace System::Runtime::InteropServices;
using namespace System::Threading::Tasks;
using namespace System::Threading;
using namespace Microsoft::Win32;

DWORD v8Thread;

// This keeps counts of how many objects the CLR is creating and
// subsequently the callbacks (weak) to destroy them. It tests whether
// V8 is actually releasing the C++ wrappers around pinned CLR objects,
// it however, does not test if the CLR garbage collector is reclaiming
// them.

// #define GC_DEBUG 1

extern "C" void uv_run_nowait();

#ifndef SHGSI_ICON
typedef struct _SHSTOCKICONINFO
{
    DWORD cbSize;
    HICON hIcon;
    int   iSysImageIndex;
    int   iIcon;
    WCHAR szPath[MAX_PATH];
} SHSTOCKICONINFO;

#define SHGSI_ICONLOCATION      0 // you always get the icon location
#define SHGSI_ICON              SHGFI_ICON
#define SHGSI_SYSICONINDEX      SHGFI_SYSICONINDEX
#define SHGSI_LINKOVERLAY       SHGFI_LINKOVERLAY
#define SHGSI_SELECTED          SHGFI_SELECTED
#define SHGSI_LARGEICON         SHGFI_LARGEICON
#define SHGSI_SMALLICON         SHGFI_SMALLICON
#define SHGSI_SHELLICONSIZE     SHGFI_SHELLICONSIZE

//  Shell icons
typedef enum SHSTOCKICONID
{
    SIID_DOCNOASSOC = 0,          // document (blank page), no associated program
    SIID_DOCASSOC = 1,            // document with an associated program
    SIID_APPLICATION = 2,         // generic application with no custom icon
    SIID_FOLDER = 3,              // folder (closed)
    SIID_FOLDEROPEN = 4,          // folder (open)
    SIID_DRIVE525 = 5,            // 5.25" floppy disk drive
    SIID_DRIVE35 = 6,             // 3.5" floppy disk drive
    SIID_DRIVEREMOVE = 7,         // removable drive
    SIID_DRIVEFIXED = 8,          // fixed (hard disk) drive
    SIID_DRIVENET = 9,            // network drive
    SIID_DRIVENETDISABLED = 10,   // disconnected network drive
    SIID_DRIVECD = 11,            // CD drive
    SIID_DRIVERAM = 12,           // RAM disk drive
    SIID_WORLD = 13,              // entire network
    SIID_SERVER = 15,             // a computer on the network
    SIID_PRINTER = 16,            // printer
    SIID_MYNETWORK = 17,          // My network places
    SIID_FIND = 22,               // Find
    SIID_HELP = 23,               // Help
    SIID_SHARE = 28,              // overlay for shared items
    SIID_LINK = 29,               // overlay for shortcuts to items
    SIID_SLOWFILE = 30,           // overlay for slow items
    SIID_RECYCLER = 31,           // empty recycle bin
    SIID_RECYCLERFULL = 32,       // full recycle bin
    SIID_MEDIACDAUDIO = 40,       // Audio CD Media
    SIID_LOCK = 47,               // Security lock
    SIID_AUTOLIST = 49,           // AutoList
    SIID_PRINTERNET = 50,         // Network printer
    SIID_SERVERSHARE = 51,        // Server share
    SIID_PRINTERFAX = 52,         // Fax printer
    SIID_PRINTERFAXNET = 53,      // Networked Fax Printer
    SIID_PRINTERFILE = 54,        // Print to File
    SIID_STACK = 55,              // Stack
    SIID_MEDIASVCD = 56,          // SVCD Media
    SIID_STUFFEDFOLDER = 57,      // Folder containing other items
    SIID_DRIVEUNKNOWN = 58,       // Unknown drive
    SIID_DRIVEDVD = 59,           // DVD Drive
    SIID_MEDIADVD = 60,           // DVD Media
    SIID_MEDIADVDRAM = 61,        // DVD-RAM Media
    SIID_MEDIADVDRW = 62,         // DVD-RW Media
    SIID_MEDIADVDR = 63,          // DVD-R Media
    SIID_MEDIADVDROM = 64,        // DVD-ROM Media
    SIID_MEDIACDAUDIOPLUS = 65,   // CD+ (Enhanced CD) Media
    SIID_MEDIACDRW = 66,          // CD-RW Media
    SIID_MEDIACDR = 67,           // CD-R Media
    SIID_MEDIACDBURN = 68,        // Burning CD
    SIID_MEDIABLANKCD = 69,       // Blank CD Media
    SIID_MEDIACDROM = 70,         // CD-ROM Media
    SIID_AUDIOFILES = 71,         // Audio files
    SIID_IMAGEFILES = 72,         // Image files
    SIID_VIDEOFILES = 73,         // Video files
    SIID_MIXEDFILES = 74,         // Mixed files
    SIID_FOLDERBACK = 75,         // Folder back
    SIID_FOLDERFRONT = 76,        // Folder front
    SIID_SHIELD = 77,             // Security shield. Use for UAC prompts only.
    SIID_WARNING = 78,            // Warning
    SIID_INFO = 79,               // Informational
    SIID_ERROR = 80,              // Error
    SIID_KEY = 81,                // Key / Secure
    SIID_SOFTWARE = 82,           // Software
    SIID_RENAME = 83,             // Rename
    SIID_DELETE = 84,             // Delete
    SIID_MEDIAAUDIODVD = 85,      // Audio DVD Media
    SIID_MEDIAMOVIEDVD = 86,      // Movie DVD Media
    SIID_MEDIAENHANCEDCD = 87,    // Enhanced CD Media
    SIID_MEDIAENHANCEDDVD = 88,   // Enhanced DVD Media
    SIID_MEDIAHDDVD = 89,         // HD-DVD Media
    SIID_MEDIABLURAY = 90,        // BluRay Media
    SIID_MEDIAVCD = 91,           // VCD Media
    SIID_MEDIADVDPLUSR = 92,      // DVD+R Media
    SIID_MEDIADVDPLUSRW = 93,     // DVD+RW Media
    SIID_DESKTOPPC = 94,          // desktop computer
    SIID_MOBILEPC = 95,           // mobile computer (laptop/notebook)
    SIID_USERS = 96,              // users
    SIID_MEDIASMARTMEDIA = 97,    // Smart Media
    SIID_MEDIACOMPACTFLASH = 98,  // Compact Flash
    SIID_DEVICECELLPHONE = 99,    // Cell phone
    SIID_DEVICECAMERA = 100,      // Camera
    SIID_DEVICEVIDEOCAMERA = 101, // Video camera
    SIID_DEVICEAUDIOPLAYER = 102, // Audio player
    SIID_NETWORKCONNECT = 103,    // Connect to network
    SIID_INTERNET = 104,          // Internet
    SIID_ZIPFILE = 105,           // ZIP file
    SIID_SETTINGS = 106,          // Settings
    // 107-131 are internal Vista RTM icons
    // 132-159 for SP1 icons
    SIID_DRIVEHDDVD = 132,        // HDDVD Drive (all types)
    SIID_DRIVEBD = 133,           // BluRay Drive (all types)
    SIID_MEDIAHDDVDROM = 134,     // HDDVD-ROM Media
    SIID_MEDIAHDDVDR = 135,       // HDDVD-R Media
    SIID_MEDIAHDDVDRAM = 136,     // HDDVD-RAM Media
    SIID_MEDIABDROM = 137,        // BluRay ROM Media
    SIID_MEDIABDR = 138,          // BluRay R Media
    SIID_MEDIABDRE = 139,         // BluRay RE Media (Rewriable and RAM)
    SIID_CLUSTEREDDRIVE = 140,    // Clustered disk
    // 160+ are for Windows 7 icons
    SIID_MAX_ICONS = 174,
} SHSTOCKICONID;

#define SIID_INVALID ((SHSTOCKICONID)-1)

SHSTDAPI SHGetStockIconInfo(SHSTOCKICONID siid, UINT uFlags, __inout SHSTOCKICONINFO *psii);
#endif

v8::Local<v8::Value> MarshalCLRToV8(System::Object^ netdata);
System::String^ exceptionV82stringCLR(v8::Handle<v8::Value> exception);


/**
 ** Supporting WPF / Windows Functions, Methods and Classes
 **/


namespace TintInterop {

  public ref class WinApi
  {
  public:
      [DllImport("shell32.Dll", CharSet = CharSet::Unicode)]
      static bool Shell_NotifyIcon(unsigned int cmd, NOTIFYICONDATA% data);

      [DllImport("USER32.DLL", EntryPoint = "CreateWindowExW", SetLastError = true)]
      static IntPtr CreateWindowEx(int dwExStyle, LPWSTR lpClassName,
          LPWSTR lpWindowName, int dwStyle, int x, int y,
          int nWidth, int nHeight, IntPtr hWndParent, IntPtr hMenu, IntPtr hInstance,
          IntPtr lpParam);

      [DllImport("USER32.DLL")]
      static IntPtr DefWindowProc(IntPtr hWnd, unsigned int msg, IntPtr wparam, IntPtr lparam);

      [DllImport("User32.Dll", EntryPoint = "RegisterWindowMessageW")]
      static unsigned int RegisterWindowMessage(LPWSTR lpString);

      [DllImport("USER32.DLL", SetLastError = true)]
      static bool DestroyWindow(IntPtr hWnd);

      [DllImport("USER32.DLL")]
      static bool SetForegroundWindow(IntPtr hWnd);

      [DllImport("user32.dll", CharSet = CharSet::Auto, ExactSpelling = true)]
      static int GetDoubleClickTime();

      [DllImport("USER32.DLL", SetLastError = true)]
      static bool GetPhysicalCursorPos(Point% lpPoint);

      [DllImport("USER32.DLL", SetLastError = true)]
      static bool GetCursorPos(Point% lpPoint);
  };

  public ref class TintDataGridColumn : System::Windows::Controls::DataGridBoundColumn
  {
  protected:
    virtual FrameworkElement^ GenerateElement(System::Windows::Controls::DataGridCell^ cell, System::Object^ dataItem) override
    {
      System::String^ name = (System::String^)((System::Windows::Data::Binding^)this->Binding)->Path->Path;
      System::Collections::Generic::IDictionary<System::String^,System::Object^>^ obj = (System::Collections::Generic::IDictionary<System::String^,System::Object^>^)dataItem;
      return (FrameworkElement ^)obj->default[name];
    }

    virtual FrameworkElement^ GenerateEditingElement(System::Windows::Controls::DataGridCell^ cell, System::Object^ dataItem) override
    {
      System::String^ name = (System::String^)((System::Windows::Data::Binding^)this->Binding)->Path->Path;
      System::Collections::Generic::IDictionary<System::String^,System::Object^>^ obj = (System::Collections::Generic::IDictionary<System::String^,System::Object^>^)dataItem;
      return (FrameworkElement ^)obj->default[name];
    }
  };

  public ref class Dwm {
    public:
    static int Glass(IntPtr hwnd, int margin) {
      MARGINS margins = {margin,margin,margin,margin};
      return ::DwmExtendFrameIntoClientArea((HWND)hwnd.ToPointer(), &margins);
    } 
  };

  public ref class WPFAnimator 
  {
  public:
      WPFAnimator(UIElement^ element_, DependencyProperty^ property_, double from_, double to_, double duration_) {
          this->element = element_;
          this->property = property_;
          this->from = from_;
          this->to = to_;
          this->duration = duration_;
          this->passed = gcnew System::Diagnostics::Stopwatch();
          this->passed->Start();
      }
      void RenderCallback(Object^ sender, EventArgs^ e) {
          double diff = ((double)this->passed->ElapsedMilliseconds)/(this->duration);
          if(diff >= 1) {
              this->passed->Stop();
              this->element->SetValue(this->property, this->to);
              System::Windows::Media::CompositionTarget::Rendering -= this->selfRef;
          } else {
              double target = ((this->to - this->from) * diff ) + this->from;
              this->element->SetValue(this->property, target);
          }
      }
      static void Render(UIElement^ element_, DependencyProperty^ property_, double from_, double to_, double duration_) {
        WPFAnimator^ an = gcnew WPFAnimator(element_, property_, from_, to_, duration_);
        an->selfRef = gcnew EventHandler(an, &WPFAnimator::RenderCallback);
        System::Windows::Media::CompositionTarget::Rendering += an->selfRef;
      }
      EventHandler ^selfRef;
  private:
      UIElement^ element;
      DependencyProperty^ property;
      double to;
      double from;
      double duration;
      System::Diagnostics::Stopwatch^ passed;
  };

  public ref class Wpf32Window : System::Windows::Forms::IWin32Window
  {
  public:
    property System::IntPtr Handle {
      virtual System::IntPtr get() { return _handle; };
      void set(IntPtr h) { _handle = h; };
    }

    Wpf32Window(System::Windows::Window^ wpfWindow)
    {
        this->Handle = (gcnew System::Windows::Interop::WindowInteropHelper(wpfWindow))->Handle;
    }
  private:
    IntPtr _handle;
  };

  public ref class Shell
  {
  public:

    static System::String^ GetIconFromFile(System::String^ file, int index) {
      try {
        HICON hIcon[] = { 0, 0, 0, 0 };
        UINT n = ExtractIconEx((const char*)(void*)Marshal::StringToHGlobalAnsi(file), index, hIcon, NULL, 1);
        if(n == 0) {
          n = ExtractIconEx((const char*)(void*)Marshal::StringToHGlobalAnsi(file), index, NULL, hIcon, 1);
        }
        if(n != 0) {
          System::Drawing::Icon^ icon = System::Drawing::Icon::FromHandle(System::IntPtr((void *)hIcon[0]));
          if(icon == nullptr || icon->Handle == System::IntPtr::Zero) {
            return nullptr;
          }
          System::Windows::Media::Imaging::BitmapSource^ source = System::Windows::Interop::Imaging::CreateBitmapSourceFromHIcon(
            icon->Handle,
            System::Windows::Int32Rect(0,0,icon->Width,icon->Height),
            System::Windows::Media::Imaging::BitmapSizeOptions::FromEmptyOptions()
          );
          
          System::Windows::Media::Imaging::PngBitmapEncoder^ encoder = gcnew System::Windows::Media::Imaging::PngBitmapEncoder();
          encoder->Frames->Add( System::Windows::Media::Imaging::BitmapFrame::Create(source) );

          System::IO::MemoryStream^ stream = gcnew System::IO::MemoryStream();
          encoder->Save(stream);
          return System::Convert::ToBase64String(stream->ToArray());
        } else {
          return nullptr;
        }
      } catch (System::Exception ^e) {
        return nullptr;
      }
    }

    static System::String^ GetSystemIcon(int Id) 
    {
      SHSTOCKICONINFO stock = {0};
      stock.cbSize = sizeof(SHSTOCKICONINFO);
      HRESULT r = SHGetStockIconInfo((SHSTOCKICONID)Id, (SHGSI_ICON | SHGSI_LARGEICON), &stock);
      if(r == S_OK)
      {
        System::Drawing::Icon^ icon = System::Drawing::Icon::FromHandle(System::IntPtr((void *)stock.hIcon));

        System::Windows::Media::Imaging::BitmapSource^ source = System::Windows::Interop::Imaging::CreateBitmapSourceFromHIcon(icon->Handle,
          System::Windows::Int32Rect(0,0,icon->Width,icon->Height),
          System::Windows::Media::Imaging::BitmapSizeOptions::FromEmptyOptions());
        
        System::Windows::Media::Imaging::PngBitmapEncoder^ encoder = gcnew System::Windows::Media::Imaging::PngBitmapEncoder();
        encoder->Frames->Add( System::Windows::Media::Imaging::BitmapFrame::Create(source) );

        System::IO::MemoryStream^ stream = gcnew System::IO::MemoryStream();
        encoder->Save(stream);
        return System::Convert::ToBase64String(stream->ToArray());
      } else {
        return nullptr;
      }
    }

    static System::String^ GetIconForFile(System::String^ file) 
    {
      try {
        SHFILEINFO shfi;
        SHGetFileInfo((const char*)(void*)Marshal::StringToHGlobalAnsi(file), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO), 
          SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON);
        
        System::Drawing::Icon^ icon = System::Drawing::Icon::FromHandle(System::IntPtr((void *)shfi.hIcon));
        System::Windows::Media::Imaging::BitmapSource^ source = System::Windows::Interop::Imaging::CreateBitmapSourceFromHIcon(icon->Handle,
          System::Windows::Int32Rect(0,0,icon->Width,icon->Height),
          System::Windows::Media::Imaging::BitmapSizeOptions::FromEmptyOptions());
        
        System::Windows::Media::Imaging::PngBitmapEncoder^ encoder = gcnew System::Windows::Media::Imaging::PngBitmapEncoder();
        encoder->Frames->Add( System::Windows::Media::Imaging::BitmapFrame::Create(source) );

        System::IO::MemoryStream^ stream = gcnew System::IO::MemoryStream();
        encoder->Save(stream);

        return System::Convert::ToBase64String(stream->ToArray());
      } catch (System::Exception ^e) {
        return nullptr;
      }
    }
  };

  public ref class CommonDialogExtensions
  {
  public:
    static System::Windows::Forms::DialogResult^ ShowDialog(System::Windows::Forms::CommonDialog^ dialog, System::Windows::Window^ parent)
    {
        return dialog->ShowDialog(gcnew Wpf32Window(parent));
    }
  };

  public ref class AsyncEventDelegate {
  public:
    AsyncEventDelegate(System::Object^ target, MethodInfo^ method, cli::array<System::Object^>^ cshargs)
    {
      this->target = target;
      this->method = method;
      this->cshargs = cshargs;
    }

    void DoWorkHandler(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
      try {
        System::Object^ rtn = method->Invoke(target, cshargs);
      } catch (System::Exception^ e) {
        Console::WriteLine(e);
        abort();
      }
    }

  private:
    System::Object^ target;
    MethodInfo^ method;
    cli::array<System::Object^>^ cshargs;
  };
}

#define DISPID_BEFORENAVIGATE2              250           // hyperlink clicked on
#define DISPID_NEWWINDOW2                   251
#define DISPID_NAVIGATECOMPLETE2            252           // UIActivate new document
#define DISPID_ONQUIT                       253
#define DISPID_ONVISIBLE                    254           // sent when the window goes visible/hidden
#define DISPID_ONTOOLBAR                    255           // sent when the toolbar should be shown/hidden
#define DISPID_ONMENUBAR                    256           // sent when the menubar should be shown/hidden
#define DISPID_ONSTATUSBAR                  257           // sent when the statusbar should be shown/hidden
#define DISPID_ONFULLSCREEN                 258           // sent when kiosk mode should be on/off
#define DISPID_DOCUMENTCOMPLETE             259           // new document goes ReadyState_Complete
#define DISPID_ONTHEATERMODE                260           // sent when theater mode should be on/off
#define DISPID_ONADDRESSBAR                 261           // sent when the address bar should be shown/hidden
#define DISPID_WINDOWSETRESIZABLE           262           // sent to set the style of the host window frame
#define DISPID_WINDOWCLOSING                263           // sent before script window.close closes the window 
#define DISPID_WINDOWSETLEFT                264           // sent when the put_left method is called on the WebOC
#define DISPID_WINDOWSETTOP                 265           // sent when the put_top method is called on the WebOC
#define DISPID_WINDOWSETWIDTH               266           // sent when the put_width method is called on the WebOC
#define DISPID_WINDOWSETHEIGHT              267           // sent when the put_height method is called on the WebOC 
#define DISPID_CLIENTTOHOSTWINDOW           268           // sent during window.open to request conversion of dimensions
#define DISPID_SETSECURELOCKICON            269           // sent to suggest the appropriate security icon to show
#define DISPID_FILEDOWNLOAD                 270           // Fired to indicate the File Download dialog is opening
#define DISPID_NAVIGATEERROR                271           // Fired to indicate the a binding error has occured
#define DISPID_PRIVACYIMPACTEDSTATECHANGE   272           // Fired when the user's browsing experience is impacted
#define DISPID_NEWWINDOW3                   273
#define DISPID_VIEWUPDATE                   281           // Fired when the contents of a shell browser window change
#define DISPID_SETPHISHINGFILTERSTATUS      282           // Fired by the Phishing Filter API to signal what state the analysis is in
#define DISPID_WINDOWSTATECHANGED           283           // Fired to indicate that the browser window's visibility or enabled state has changed

/**
 ** Begin CLR Bridge Code
 **/
#ifdef GC_DEBUG
 static int CppClassCount = 0;
 static int CppCollectCount = 0;
#endif
void gchandle_cleanup(char *data, void *hint) {
#ifdef GC_DEBUG
  CppClassCount--;
  CppCollectCount++;
#endif
  GCHandle handle = GCHandle::FromIntPtr(IntPtr(data));
  handle.Free();
}

// Extracts a C string from a V8 Utf8Value.
// static const char* ToCString(const v8::String::Utf8Value& value) {
//	return *value ? *value : "<string conversion failed>";
// }

// This does not seem to be executed but is the call back for when we have .NET objects,
// when have properties that are pointers, that those pointers are passed back to JS (for example)
// WPF Window .NET CLR object has a property called HWND which is the native win32
// void * pointer to the HWND. We need to both recollect HWND, the WPF Window object (handled
// by CppClass), and the V8 reference to all of it (handled by V8.), we'll hope that the 
// pointer property was not allocated but is a persistant memory need and leave it up to 
// .NET to determine if it needs to be collected.  In otherwords, do nothing.
void wrap_cb(char *data, void *hint) { }

System::String^ stringV82CLR(Handle<v8::String> text)
{
  v8::String::Utf8Value utf8text(text);
  if (*utf8text)
      return gcnew System::String(*utf8text, 0, utf8text.length(), System::Text::Encoding::UTF8);
  else
      return System::String::Empty;
}

Handle<v8::String> stringCLR2V8(System::String^ text)
{
  Nan::EscapableHandleScope scope;
  if (text->Length > 0)
  {
    marshal_context ^ context = gcnew marshal_context();
    const char* str = context->marshal_as<const char*>(text);
    v8::Local<v8::String> v8str = Nan::New<v8::String>(str).ToLocalChecked();
    delete context;
    return scope.Escape(v8str);
  }
  else
    return scope.Escape(v8::String::Empty(v8::Isolate::GetCurrent()));
}

System::String^ exceptionV82stringCLR(v8::Handle<v8::Value> exception)
{
  Nan::HandleScope scope;
  if (exception->IsObject())
  {
    Handle<Value> stack = exception->ToObject()->Get(Nan::New<v8::String>("stack").ToLocalChecked());
    if (stack->IsString())
      return gcnew System::String(stringV82CLR(stack->ToString()));
  }
  return gcnew System::String(stringV82CLR(Handle<v8::String>::Cast(exception)));
}

Local<Value> throwV8Exception(Local<Value> exception)
{
  Nan::HandleScope scope;
  Nan::ThrowError(exception);
  return exception;
}

v8::Local<v8::Value> MarshalCLRToV8(System::Object^ netdata)
{
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Value> jsdata;

  if (netdata == nullptr) {
	  return Nan::Null();
  }
  System::Type^ type = netdata->GetType();
  if (type == System::String::typeid)         jsdata = stringCLR2V8((System::String^)netdata);
  else if (type == System::Char::typeid)      jsdata = stringCLR2V8(((System::Char^)netdata)->ToString());
  else if (type == bool::typeid)              jsdata = Nan::New<v8::Boolean>((bool)netdata);
  else if (type == System::Guid::typeid)      jsdata = stringCLR2V8(netdata->ToString());
  else if (type == System::DateTime::typeid)
  {
    System::DateTime ^dt = (System::DateTime^)netdata;
    if (dt->Kind == System::DateTimeKind::Local)
        dt = dt->ToUniversalTime();
    else if (dt->Kind == System::DateTimeKind::Unspecified)
        dt = gcnew System::DateTime(dt->Ticks, System::DateTimeKind::Utc);
    long long MinDateTimeTicks = 621355968000000000; // new DateTime(1970, 1, 1, 0, 0, 0).Ticks;
    long long value = ((dt->Ticks - MinDateTimeTicks) / 10000);
    delete dt;
    jsdata = v8::Date::New(v8::Isolate::GetCurrent(),(double)value);
  }
  else if (type == System::DateTimeOffset::typeid)  jsdata = stringCLR2V8(netdata->ToString());
  else if (type == int::typeid)                     jsdata = Nan::New<v8::Integer>((int)netdata);
  else if (type == System::Int64::typeid)           jsdata = Nan::New<v8::Number>(((System::IConvertible^)netdata)->ToDouble(nullptr));
  else if (type == double::typeid)                  jsdata = Nan::New<v8::Number>((double)netdata);
  else if (type == float::typeid)                   jsdata = Nan::New<v8::Number>((float)netdata);
  else if (type == cli::array<byte>::typeid)
  {
    cli::array<byte>^ buffer = (cli::array<byte>^)netdata;
    v8::Local<v8::Value> slowBuffer = Nan::NewBuffer(buffer->Length).ToLocalChecked();
    if (buffer->Length > 0)
    {
      pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
      memcpy(node::Buffer::Data(slowBuffer), pinnedBuffer, buffer->Length);
    }
    (v8::Handle<v8::Object>::Cast(slowBuffer))->Set(Nan::New<v8::String>("array").ToLocalChecked(), Nan::New<v8::Boolean>(true));
    jsdata = slowBuffer;
  } 
  else {
#ifdef GC_DEBUG
    CppClassCount++;
#endif
    GCHandle handle = GCHandle::Alloc(netdata);
    void *ptr = GCHandle::ToIntPtr(handle).ToPointer();
    jsdata = Nan::NewBuffer((char *)(ptr), sizeof(ptr), gchandle_cleanup, NULL).ToLocalChecked();
    if(type == System::IntPtr::typeid) {
      void *rawptr = ((System::IntPtr ^)netdata)->ToPointer();
      v8::Local<v8::Object> bufptr = Nan::NewBuffer((char *)rawptr, sizeof(rawptr), wrap_cb, NULL).ToLocalChecked();
      (v8::Handle<v8::Object>::Cast(jsdata))->Set(Nan::New<v8::String>("rawpointer").ToLocalChecked(), bufptr);
    }
  }
  return scope.Escape(jsdata);
}

Handle<v8::Object> MarshalCLRObjectToV8(System::Object^ netdata)
{
  Nan::EscapableHandleScope scope;
  Local<v8::Object> result = Nan::New<v8::Object>();
  System::Type^ type = netdata->GetType();

  // Avoid stack overflow due to self-referencing reflection elements
  if (0 == System::String::Compare(type->FullName, "System.Reflection.RuntimeMethodInfo"))
    return scope.Escape(result);

  for each (FieldInfo^ field in type->GetFields(BindingFlags::Public | BindingFlags::Instance))
    result->Set(stringCLR2V8(field->Name), MarshalCLRToV8(field->GetValue(netdata)));

  for each (PropertyInfo^ property in type->GetProperties(BindingFlags::GetProperty | BindingFlags::Public | BindingFlags::Instance))
  {
    MethodInfo^ getMethod = property->GetGetMethod();
    if (getMethod != nullptr && getMethod->GetParameters()->Length <= 0)
      result->Set(stringCLR2V8(property->Name), MarshalCLRToV8(getMethod->Invoke(netdata, nullptr)));
  }
  return scope.Escape(result);
}

System::Object^ MarshalV8ToCLR(v8::Handle<v8::Value> jsdata)
{
  Nan::HandleScope scope;
  if (jsdata->IsArray())
  {
    Handle<v8::Array> jsarray = Handle<v8::Array>::Cast(jsdata);
    cli::array<System::Object^>^ netarray = gcnew cli::array<System::Object^>(jsarray->Length());
    for (unsigned int i = 0; i < jsarray->Length(); i++) {
      netarray[i] = MarshalV8ToCLR(jsarray->Get(i));
    }
    return netarray;
  }
  else if (jsdata->IsDate())
  {
    Handle<v8::Date> jsdate = Handle<v8::Date>::Cast(jsdata);
    long long  ticks = (long long)jsdate->NumberValue();
    long long MinDateTimeTicks = 621355968000000000;// (new DateTime(1970, 1, 1, 0, 0, 0)).Ticks;
    System::DateTime ^netobject = gcnew System::DateTime(ticks * 10000 + MinDateTimeTicks, System::DateTimeKind::Utc);
    return netobject;
  }
  else if (jsdata->IsString())      return stringV82CLR(Handle<v8::String>::Cast(jsdata));
  else if (jsdata->IsBoolean())     return jsdata->BooleanValue();
  else if (jsdata->IsInt32())       return jsdata->Int32Value();
  else if (jsdata->IsUint32())      return jsdata->Uint32Value();
  else if (jsdata->IsNumber())      return jsdata->NumberValue();
  else if (jsdata->IsUndefined() || 
    jsdata->IsNull())               return nullptr;
  else if (node::Buffer::HasInstance(jsdata) && 
    (v8::Handle<v8::Object>::Cast(jsdata))->Get(Nan::New<v8::String>("array").ToLocalChecked())->BooleanValue()) 
  {
    Handle<v8::Object> jsbuffer = jsdata->ToObject();
    cli::array<byte>^ netbuffer = gcnew cli::array<byte>((int)node::Buffer::Length(jsbuffer));
    if (netbuffer->Length > 0) 
    {
      pin_ptr<byte> pinnedNetbuffer = &netbuffer[0];
      memcpy(pinnedNetbuffer, node::Buffer::Data(jsbuffer), netbuffer->Length);
    }
    return netbuffer;
  }
  else if (node::Buffer::HasInstance(jsdata)) 
  {
    void *data = (void *)node::Buffer::Data(jsdata.As<v8::Object>());
    GCHandle handle = GCHandle::FromIntPtr(IntPtr(data));
    return handle.Target;
  }
  else if (jsdata->IsObject()) 
  {
    System::Collections::Generic::IDictionary<System::String^,System::Object^>^ netobject = gcnew System::Dynamic::ExpandoObject();
    Handle<v8::Object> jsobject = Handle<v8::Object>::Cast(jsdata);
    Handle<v8::Array> propertyNames = jsobject->GetPropertyNames();
    for (unsigned int i = 0; i < propertyNames->Length(); i++)
    {
      Handle<v8::String> name = Handle<v8::String>::Cast(propertyNames->Get(i));
      v8::String::Utf8Value utf8name(name);
      System::String^ netname = gcnew System::String(*utf8name);
      System::Object^ netvalue = MarshalV8ToCLR(jsobject->Get(name));
      netobject->Add(netname, netvalue);
    }
    return netobject;
  }
  else
    throw gcnew System::Exception("Unable to convert V8 value to CLR value.");
}

v8::Handle<v8::Value> MarshalCLRExceptionToV8(System::Exception^ exception)
{
  Nan::EscapableHandleScope scope;
  Local<v8::Object> result;
  Local<v8::String> message;
  Local<v8::String> name;

  if (exception == nullptr)
  {
    result = Nan::New<v8::Object>();
    message = Nan::New<v8::String>("Unrecognized exception thrown by CLR.").ToLocalChecked();
    name = Nan::New<v8::String>("InternalException").ToLocalChecked();
  }
  else
  {
    // Remove AggregateException wrapper from around singleton InnerExceptions
    if (System::AggregateException::typeid->IsAssignableFrom(exception->GetType()))
    {
      System::AggregateException^ aggregate = (System::AggregateException^)exception;
      if(aggregate->InnerExceptions->Count == 1)
        exception = aggregate->InnerExceptions[0];
    }
    else if (System::Reflection::TargetInvocationException::typeid->IsAssignableFrom(exception->GetType())
      && exception->InnerException != nullptr)
    {
      exception = exception->InnerException;
    }

    result = MarshalCLRObjectToV8(exception);
    message = stringCLR2V8(exception->GetType()->FullName + " " + exception->Message + "\n" + exception->StackTrace);
    name = stringCLR2V8(exception->GetType()->FullName);
  }   
      
  // Construct an error that is just used for the prototype - not verify efficient
  // but 'typeof Error' should work in JavaScript
  result->SetPrototype(v8::Exception::Error( message));
  result->Set(Nan::New<v8::String>("message").ToLocalChecked(), message);
  
  // Recording the actual type - 'name' seems to be the common used property
  result->Set(Nan::New<v8::String>("name").ToLocalChecked(), name);

  return scope.Escape(result);
}

static int countFound = 0;

void clr_callback(uv_async_t* work);

public ref class CLREventHandler {
public:
  
  CLREventHandler() : callback(NULL) {
    cppobject = new gcroot<CLREventHandler ^>(this);
    countFound = countFound + 12;
    id = (gcnew System::Random())->Next(countFound);
    loop = uv_default_loop();
    async = new uv_async_t;
    uv_async_init(loop, async, clr_callback);
    uv_ref((uv_handle_t *)async);
    sem = gcnew Semaphore(0,1);
  }
  ~CLREventHandler() {
    uv_unref((uv_handle_t *)async);
    Delete();
    delete cppobject;
  }
  void SetCallback(v8::Local<v8::Function> cb) {
    callback = new Nan::Callback(cb);
  }
  void *GetReference() {
    return cppobject;
  }
  void PassThru(... cli::array<System::Object^>^ args) {
    Nan::HandleScope scope;
    std::vector<v8::Local<v8::Value>> argv;
    v8::TryCatch try_catch;

    for(int i=0; i < args->Length; i++) 
      argv.push_back(MarshalCLRToV8(args[i]));

    if (this->callback == NULL || this->callback->IsEmpty()) {
      throw gcnew System::Exception("CLR Fatal (PassThru): Callback has been garbage collected.");
      exit(1);
    } else {
      // invoke the registered callback function
	   callback->Call(args->Length, argv.data());
    }
    if (try_catch.HasCaught()) {
      try_catch.ReThrow();
    }
  }

  void Delete() {
    if(callback) {
      delete callback;
      callback = NULL;
      delete this;
    }
  }

  void EventHandlerOnMain(System::Object^ sender, System::EventArgs^ e) {
    if(GetCurrentThreadId() == v8Thread) {
      _sender = nullptr;
      _eventargs = nullptr;
      this->EventHandler(sender, e);
      return;
    }
    _sender = sender;
    _eventargs = e;
    handle = GCHandle::Alloc(this);
    void *data = (void *)GCHandle::ToIntPtr(handle).ToPointer();
    async->data = data;
    uv_async_send(async);
    sem->WaitOne();
  }

  void EventHandlerOnMainCallback() {
    this->EventHandler(this->_sender, this->_eventargs);
  }

  void EventHandler(System::Object^ sender, System::EventArgs^ e) {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[2];

    argv[0] = MarshalCLRToV8(sender);
    argv[1] = MarshalCLRToV8(e);

    v8::TryCatch try_catch;

    if (this->callback == NULL || this->callback->IsEmpty()) {
      throw gcnew System::Exception("CLR Fatal (EventHandler): Callback has been garbage collected.");
      exit(1);
    } else {
      // invoke the registered callback function
      this->callback->Call(2, argv);
      if(_sender != nullptr) {
        sem->Release();
      }
    }
    if (try_catch.HasCaught()) {
      throw gcnew System::Exception(exceptionV82stringCLR(try_catch.Exception()));
      exit(1);
    }
  }
  int Name() {
    return id;
  }
private:
  System::Object^ _sender;
  System::EventArgs^ _eventargs;
  Semaphore^ sem;
  GCHandle handle;
  uv_loop_t *loop;
  uv_async_t *async;
  Nan::Callback *callback;
  gcroot<CLREventHandler ^> * cppobject;
  int id;
};

void clr_callback(uv_async_t* work) {
  CLREventHandler^ handle = (CLREventHandler^)GCHandle::FromIntPtr(System::IntPtr(work->data)).Target;
  handle->EventHandlerOnMainCallback();
}

void CLREventHandleCleanupJS(Persistent<Value> object, void *parameter) {
  CLREventHandler ^n = *((gcroot<CLREventHandler ^> *)parameter);
  n->Delete();
}

class CLR {
  CLR() { }

public:
  // deprecated
  static NAN_METHOD(CreateClass) {
    try {
      System::String^ name = stringV82CLR(info[0]->ToString());
      System::Type^ base = (System::Type ^)MarshalV8ToCLR(info[1]);
      cli::array<System::Object^>^ interfaces = (cli::array<System::Object ^>^)MarshalV8ToCLR(info[2]);
      cli::array<System::Object^>^ abstracts = (cli::array<System::Object ^>^)MarshalV8ToCLR(info[3]);

      System::AppDomain^ domain = System::AppDomain::CurrentDomain;
      AssemblyName^ aName = gcnew AssemblyName(name);
      AssemblyBuilder^ ab = domain->DefineDynamicAssembly(aName, AssemblyBuilderAccess::RunAndCollect);
      ModuleBuilder^ mb = ab->DefineDynamicModule(aName->Name, aName->Name + ".dll");
      TypeBuilder^ tb = mb->DefineType(name,  TypeAttributes::Public | 
                                              TypeAttributes::Class | 
                                              TypeAttributes::AnsiClass |
                                              TypeAttributes::AutoLayout | 
                                              TypeAttributes::AutoClass, 
                                              base);
      info.GetReturnValue().Set(MarshalCLRToV8(tb));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }
  // deprecated
  static NAN_METHOD(AddConstructor) {
    try {
      TypeBuilder^ tb = (TypeBuilder ^)MarshalV8ToCLR(info[0]);
      System::String^ accessibility = stringV82CLR(info[1]->ToString());
      bool defaultConst = ((System::Boolean ^)MarshalV8ToCLR(info[2]))->CompareTo(true);
      cli::array<System::Type ^>^ types = (cli::array<System::Type ^>^)MarshalV8ToCLR(info[3]);
      v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[4]);

      CLREventHandler ^handle = gcnew CLREventHandler();
      handle->SetCallback(callback);

      MethodAttributes attr = MethodAttributes::Public;
      if(accessibility == "private") 
        attr = MethodAttributes::Private;

      ConstructorBuilder^ ctor0;

      if(types->Length == 0) {
        ctor0 = tb->DefineConstructor(attr, CallingConventions::Standard, System::Type::EmptyTypes);
      } else {
        ctor0 = tb->DefineConstructor(attr, CallingConventions::Standard, (cli::array<System::Type^>^)types);
      }

      ILGenerator ^il = ctor0->GetILGenerator();
      
      // Push args
      for(unsigned short i=0; i < types->Length; i++) {
        il->Emit(OpCodes::Ldarg_S,i);
      }

      // Call method CLREventHandler->PassThru
      il->EmitCall(OpCodes::Call,
        handle->GetType()->GetMethod("PassThru"),
        types);

      // Return back.
      il->Emit(OpCodes::Ret);

      info.GetReturnValue().Set(Nan::Undefined());
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }
  // deprecated
  static NAN_METHOD(AddMethodToClass) {
    try {
      TypeBuilder^ tb = (TypeBuilder ^)MarshalV8ToCLR(info[0]);
      System::String^ name = stringV82CLR(info[1]->ToString());
      System::String^ accessibility = stringV82CLR(info[2]->ToString());
      bool staticDef = ((bool)MarshalV8ToCLR(info[3]));
      bool overrideDef = ((bool)MarshalV8ToCLR(info[4]));
      System::Type^ rettype = (System::Type ^)MarshalV8ToCLR(info[5]);
      cli::array<System::Object ^>^ objects = (cli::array<System::Object ^>^)MarshalV8ToCLR(info[6]);
      cli::array<System::Type ^>^ types = gcnew cli::array<System::Type ^>(objects->Length);
      for(int i=0; i < objects->Length; i++) {
        types[i] = (System::Type ^)objects[i];
      }
      v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[7]);

      CLREventHandler ^handle = gcnew CLREventHandler();
      handle->SetCallback(callback);

      MethodAttributes attr = MethodAttributes::Public;
      if(accessibility == "private") attr = MethodAttributes::Private;
      if(staticDef) attr = attr | MethodAttributes::Static;

      MethodBuilder^ m0;

      if(types->Length == 0) {
        m0 = tb->DefineMethod(name, attr, rettype, System::Type::EmptyTypes);
      } else {
        m0 = tb->DefineMethod(name, attr, rettype, (cli::array<System::Type^>^)types);
      }
/*
      ILGenerator ^il = m0->GetILGenerator();
      MethodInfo^ target_mh = (MethodInfo^)handle->GetType()->GetMethod("PassThru");

      if(!target_mh->IsStatic) {
        il->DeclareLocal(handle->GetType());
        il->Emit(OpCodes::Ldarg_0);
        //il->Emit(OpCodes::Ldc_I4_0);
        //il->Emit(OpCodes::Ldelem_Ref);

        //il->Emit(OpCodes::Ldarg_0);
        //il->Emit(OpCodes::Castclass, target_mh->DeclaringType);
        
        il->Emit(OpCodes::Castclass, handle->GetType());
      }

      if (target_mh->IsVirtual && !target_mh->IsFinal) {
        il->EmitCall(OpCodes::Callvirt, target_mh, nullptr);
      } else {
        il->EmitCall(OpCodes::Call, target_mh, nullptr);
      }
      if (target_mh->ReturnType->IsValueType) {
        il->Emit(OpCodes::Box, target_mh->ReturnType);
      }
      il->Emit(OpCodes::Ret); 
*/
      //il->Emit(OpCodes::Ldarg_0);
      // Push args
      //for(unsigned short i=0; i < types->Length; i++) {
      //  il->Emit(OpCodes::Ldarg_S, i+1);
      //}
      //il->Emit(OpCodes::Arglist);
      //il->Emit(OpCodes::Call, System::ArgIterator::GetType()->GetConstructor(new Type[] { RuntimeArgumentHandle->GetType() }));
      // Call method CLREventHandler->PassThru
      //MethodInfo^ mh = handle->GetType()->GetMethod("PassThru");
      //if(mh == nullptr) {
      //  Console::WriteLine("Fatal: cannot execute method info of null.");
      //  exit(1);
      //}
      //il->EmitCall(OpCodes::Call, mh, types); //, types
      // Return back.

      // If we'd like to override the method in the process.
      MethodInfo^ info = (MethodInfo^)tb->BaseType->GetMethod(name);
      if(overrideDef && info != nullptr) {
        tb->DefineMethodOverride(m0,info);
      }
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }
  // deprecated
  static NAN_METHOD(AddPropertyToClass) {
    try {
      TypeBuilder^ tb = (TypeBuilder ^)MarshalV8ToCLR(info[0]);
      System::String^ name = stringV82CLR(info[1]->ToString());
      System::String^ accessibility = stringV82CLR(info[2]->ToString());
      bool staticDef = ((System::Boolean ^)MarshalV8ToCLR(info[3]))->CompareTo(true);
      bool readOnly = ((System::Boolean ^)MarshalV8ToCLR(info[4]))->CompareTo(true);
      System::Type^ propType = (System::Type^)MarshalV8ToCLR(info[5]);
      System::Object ^value = MarshalV8ToCLR(info[6]);

      FieldBuilder^ fieldBuilder = tb->DefineField("_" + name, propType, FieldAttributes::Private);

      MethodAttributes attr = MethodAttributes::Public;
      if(accessibility == "private") attr = MethodAttributes::Private;
      if(staticDef) attr = attr | MethodAttributes::Static;
      attr = attr | MethodAttributes::SpecialName | MethodAttributes::HideBySig;

      PropertyBuilder^ propertyBuilder = tb->DefineProperty(name, PropertyAttributes::HasDefault, propType, nullptr);
      MethodBuilder^ getPropMthdBldr = tb->DefineMethod("get_" + name, attr, propType, System::Type::EmptyTypes);
      ILGenerator^ getIl = getPropMthdBldr->GetILGenerator();

      getIl->Emit(OpCodes::Ldarg_0);
      getIl->Emit(OpCodes::Ldfld, fieldBuilder);
      getIl->Emit(OpCodes::Ret);
       
      cli::array<System::Type ^>^ props = gcnew cli::array<System::Type ^>(1);
      props[0] = propType;
      MethodBuilder^ setPropMthdBldr = tb->DefineMethod("set_" + name,
        MethodAttributes::Public | MethodAttributes::SpecialName | MethodAttributes::HideBySig,
        nullptr, props);

      ILGenerator^ setIl = setPropMthdBldr->GetILGenerator();
      Label modifyProperty = setIl->DefineLabel();
      Label exitSet = setIl->DefineLabel();

      setIl->MarkLabel(modifyProperty);
      setIl->Emit(OpCodes::Ldarg_0);
      setIl->Emit(OpCodes::Ldarg_1);
      setIl->Emit(OpCodes::Stfld, fieldBuilder);
      setIl->Emit(OpCodes::Nop);
      setIl->MarkLabel(exitSet);
      setIl->Emit(OpCodes::Ret);

      propertyBuilder->SetGetMethod(getPropMthdBldr);
      propertyBuilder->SetSetMethod(setPropMthdBldr);

      info.GetReturnValue().Set(Nan::Undefined());
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }
  // deprecated
  static NAN_METHOD(AddFieldToClass) {
    try {
      TypeBuilder^ tb = (TypeBuilder ^)MarshalV8ToCLR(info[0]);
      System::String^ name = stringV82CLR(info[1]->ToString());
      System::String^ accessibility = stringV82CLR(info[2]->ToString());
      bool staticDef = ((System::Boolean ^)MarshalV8ToCLR(info[3]))->CompareTo(true);
      bool readOnly = ((System::Boolean ^)MarshalV8ToCLR(info[4]))->CompareTo(true);
      System::Type^ propType = (System::Type^)MarshalV8ToCLR(info[5]);
      System::Object ^value = MarshalV8ToCLR(info[6]);

      FieldAttributes attr = FieldAttributes::Public;
      if(accessibility == "private") attr = FieldAttributes::Private;
      
      if(staticDef) attr = attr | FieldAttributes::Static;
      FieldBuilder^ fieldBuilder = tb->DefineField(name, propType, attr);
      
      info.GetReturnValue().Set(Nan::Undefined());
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }
  // deprecated
  static NAN_METHOD(RegisterClass) {
    try {
      TypeBuilder^ tb = (TypeBuilder ^)MarshalV8ToCLR(info[0]);
      System::Type^ t = tb->CreateType();
      info.GetReturnValue().Set(MarshalCLRToV8(t));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }


#ifdef GC_DEBUG
  static NAN_METHOD(GetCppClassCount) {
    info.GetReturnValue().Set(Nan::New<v8::Number>(CppClassCount));
  }
  static NAN_METHOD(GetCppCollectCount) {
    info.GetReturnValue().Set(Nan::New<v8::Number>(CppCollectCount));
  }

#endif

  static NAN_METHOD(GetReferencedAssemblies) {
    try {
      cli::array<System::Reflection::AssemblyName^>^ assemblies = System::Reflection::Assembly::GetExecutingAssembly()->GetReferencedAssemblies();
      info.GetReturnValue().Set(MarshalCLRToV8(assemblies));
    } catch (System::Exception^ e) {
		  info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(GetLoadedAssemblies) {
    try {
      cli::array<System::Reflection::Assembly^>^ assemblies = System::AppDomain::CurrentDomain->GetAssemblies();
      info.GetReturnValue().Set(MarshalCLRToV8(assemblies));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(LoadAssemblyFromMemory) {
    try {
      System::String^ assemblyName = stringV82CLR(info[0]->ToString());
      System::Reflection::Assembly^ assembly = System::Reflection::Assembly::Load(assemblyName);
      delete assemblyName;
      info.GetReturnValue().Set(MarshalCLRToV8(assembly->GetTypes()));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  /** Load an Execution of DOTNET CLR **/
  static NAN_METHOD(LoadAssembly) {
    try {
      System::String^ userpath = stringV82CLR(info[0]->ToString());

      System::String^ framworkRegPath = "Software\\Microsoft\\.NetFramework";
      Microsoft::Win32::RegistryKey^ netFramework = Microsoft::Win32::Registry::LocalMachine;
      netFramework = netFramework->OpenSubKey(framworkRegPath, false);
      System::String^ installRoot = netFramework->GetValue("InstallRoot")->ToString();
      System::String^ version = System::String::Format("v{0}.{1}.{2}\\",
        System::Environment::Version->Major, 
        System::Environment::Version->Minor,
        System::Environment::Version->Build); 
      System::String^ netPath = System::IO::Path::Combine(installRoot, version);

      System::Reflection::Assembly^ assembly;
      if(System::IO::File::Exists(netPath + userpath))
        assembly = System::Reflection::Assembly::LoadFrom(netPath + userpath);
      else if (System::IO::File::Exists(userpath))
        assembly = System::Reflection::Assembly::LoadFrom(userpath);
      else
        assembly = System::Reflection::Assembly::Load(userpath);

      delete netPath;
      delete version;
      delete installRoot;
      delete netFramework;
      delete framworkRegPath;
      delete userpath;
	    info.GetReturnValue().Set(MarshalCLRToV8(assembly->GetTypes()));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(GetCLRType) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      info.GetReturnValue().Set(MarshalCLRToV8(target->GetType()));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e))); 
    }
  }

  static NAN_METHOD(GetStaticMemberTypes) {
    try {
      if(info.Length() < 1) {
        System::Object^ target = MarshalV8ToCLR(info[0]);
        System::Type^ type = (System::Type^)(target);
        System::Object^ rtn = type->GetMembers(BindingFlags::Public | BindingFlags::Static | BindingFlags::FlattenHierarchy);
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      } else {
        System::String^ type = stringV82CLR(info[1]->ToString());
        if(type->Equals("events")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetEvents(BindingFlags::Public | BindingFlags::Static | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        } else if(type->Equals("fields")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetFields(BindingFlags::Public | BindingFlags::Static | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        } else if(type->Equals("properties")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetProperties(BindingFlags::Public | BindingFlags::Static | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        } else if(type->Equals("methods")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetMethods(BindingFlags::Public | BindingFlags::Static | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        }
      }
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e))); 
    }
  }

  static NAN_METHOD(GetMemberTypes) {
    try {
      if(info.Length() < 1) {
        System::Object^ target = MarshalV8ToCLR(info[0]);
        System::Type^ type = (System::Type^)(target);
        System::Object^ rtn = type->GetMembers(BindingFlags::Public | BindingFlags::Instance | BindingFlags::FlattenHierarchy);
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      } else {
        System::String^ type = stringV82CLR(info[1]->ToString());
        if(type->Equals("events")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetEvents(BindingFlags::Public | BindingFlags::Instance | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        } else if(type->Equals("fields")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetFields(BindingFlags::Public | BindingFlags::Instance | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        } else if(type->Equals("properties")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetProperties(BindingFlags::Public | BindingFlags::Instance | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        } else if(type->Equals("methods")) {
          System::Object^ target = MarshalV8ToCLR(info[0]);
          System::Type^ type = (System::Type^)(target);
          System::Object^ rtn = type->GetMethods(BindingFlags::Public | BindingFlags::Instance | BindingFlags::FlattenHierarchy);
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        }
      }
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e))); 
    }
  }

  static NAN_METHOD(ExecNew) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);

      int argSize = info.Length() - 1;
      cli::array<System::Object^>^ cshargs = gcnew cli::array<System::Object^>(argSize);

      for(int i=0; i < argSize; i++)
        cshargs->SetValue(MarshalV8ToCLR(info[i + 1]),i);

      System::Type^ type = (System::Type^)(target);
      System::Object^ rtn = System::Activator::CreateInstance(type, cshargs);
      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecSetField) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      System::Object^ value = MarshalV8ToCLR(info[2]);
      System::String^ field = stringV82CLR(info[1]->ToString());

      System::Type^ baseType = target->GetType();
      if(baseType != System::Type::typeid && target == System::Type::typeid) {
        baseType = (System::Type ^)target;
      }

      baseType->GetField(field,
        BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy)->SetValue(target, value);
      
      delete baseType;
      delete field;

      info.GetReturnValue().Set(Nan::Undefined());
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecGetStaticField) {
    try {
      System::Type^ target = (System::Type^)MarshalV8ToCLR(info[0]);
      System::String^ field = stringV82CLR(info[1]->ToString());

      System::Reflection::FieldInfo ^fieldinfo = target->GetField(field, 
         BindingFlags::Static | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy);

      System::Object^ rtn = fieldinfo->GetValue(nullptr);
      delete fieldinfo;
      delete field;
      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecGetField) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      System::String^ field = stringV82CLR(info[1]->ToString());
      System::Type^ baseType = target->GetType();
      if(baseType != System::Type::typeid && target == System::Type::typeid)
        baseType = (System::Type ^)target;

      System::Reflection::FieldInfo ^fieldinfo = baseType->GetField(field, 
         BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy);

      System::Object^ rtn = fieldinfo->GetValue(target);
      
      delete fieldinfo;
      delete baseType;
      delete field;

      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecGetStaticMethodObject) {
    try {
      System::Type^ target = (System::Type^)MarshalV8ToCLR(info[0]);
      System::String^ method = stringV82CLR(info[1]->ToString());

      int argSize = info.Length() - 2;
      cli::array<System::Type^>^ cshargs = gcnew cli::array<System::Type^>(argSize);

      for(int i=0; i < argSize; i++) {
        System::Object^ obj = MarshalV8ToCLR(info[i + 2]);
        if(obj != nullptr) {
          System::Type^ argtype = obj->GetType();
          cshargs->SetValue(argtype,i);
        } else {
          // null was passed in, since we cannot use this to properly get the method overload
          // be reflected types we'll try just a name match.
          MethodInfo^ rtnl = target->GetMethod(method,BindingFlags::Static | BindingFlags::Public | BindingFlags::DeclaredOnly);
          if(rtnl == nullptr)
            info.GetReturnValue().Set(throwV8Exception(MarshalCLRToV8("Method could not be found: "+method)));
          else
            info.GetReturnValue().Set(MarshalCLRToV8(rtnl));
        }
      }

      MethodInfo^ rtn = target->GetMethod(method, 
        BindingFlags::Static | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy,
        nullptr,
        cshargs,
        nullptr);
      
      delete method;

      if(rtn == nullptr) {
        rtn = target->GetMethod(method);
        if(rtn == nullptr) {
          info.GetReturnValue().Set(throwV8Exception(MarshalCLRToV8("Method could not be found (non-null-args): "+method+" on "+target)));
        }
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      } else {
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      }
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecGetMethodObject) {
    try {
      System::Object^ target = (System::Object^)MarshalV8ToCLR(info[0]);
      System::String^ method = stringV82CLR(info[1]->ToString());

      int argSize = info.Length() - 2;
      cli::array<System::Type^>^ cshargs = gcnew cli::array<System::Type^>(argSize);

      for(int i=0; i < argSize; i++) {
        System::Object^ obj = MarshalV8ToCLR(info[i + 2]);
        if(obj != nullptr) {
          System::Type^ argtype = obj->GetType();
          cshargs->SetValue(argtype, i);
        } else {
          // null was passed in, since we cannot use this to properly get the method overload
          // be reflected types we'll try just a name match.
          MethodInfo^ rtnl = target->GetType()->GetMethod(method, 
            BindingFlags::Instance | BindingFlags::Public | BindingFlags::DeclaredOnly);
          if(rtnl == nullptr)
            info.GetReturnValue().Set(throwV8Exception(MarshalCLRToV8("Method could not be found: "+method)));
          else
            info.GetReturnValue().Set(MarshalCLRToV8(rtnl));
          return;
        }
      }

      MethodInfo^ rtn = target->GetType()->GetMethod(method, 
        BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy,
        nullptr,
        cshargs,
        nullptr);
      
      delete method;

      if(rtn == nullptr) {
        rtn = target->GetType()->GetMethod(method);
        if(rtn == nullptr) {
          info.GetReturnValue().Set(throwV8Exception(MarshalCLRToV8("Method could not be found (non-null-args): "+method)));
        } else {
          info.GetReturnValue().Set(MarshalCLRToV8(rtn));
        }
      } else {
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      }
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecGetStaticPropertyObject) {
    try {
      System::Type^ target = (System::Type^)MarshalV8ToCLR(info[0]);
      System::String^ property = stringV82CLR(info[1]->ToString());
      try {
        PropertyInfo^ rtn = target->GetProperty(property, 
          BindingFlags::Static | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy);
        delete property;
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      } catch (AmbiguousMatchException^ e) {
        PropertyInfo^ rtn = target->GetProperty(property,
          BindingFlags::Static | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy | BindingFlags::DeclaredOnly);
        delete property;
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      }
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecGetPropertyObject) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      System::String^ property = stringV82CLR(info[1]->ToString());
      try {
        PropertyInfo^ rtn = target->GetType()->GetProperty(property, 
          BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy);
        delete property;
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      } catch (AmbiguousMatchException^ e) {
        PropertyInfo^ rtn = target->GetType()->GetProperty(property,
          BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy | BindingFlags::DeclaredOnly);
        delete property;
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      }
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecMethodObject) {
    try {
      MethodInfo^ method = (MethodInfo^)MarshalV8ToCLR(info[0]);
      System::Object^ target = MarshalV8ToCLR(info[1]);
      int argSize = info.Length() - 2;
      cli::array<System::Object^>^ cshargs = gcnew cli::array<System::Object^>(argSize);

      for (int i = 0; i < argSize; i++) {
        System::Object^ objectPassed = MarshalV8ToCLR(info[i + 2]);
        cshargs->SetValue(objectPassed, i);
      }
      System::Object^ rtn = method->Invoke(target, cshargs);
  
      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecPropertyGet) {
    System::Object^ target = MarshalV8ToCLR(info[1]);
    PropertyInfo^ prop = (PropertyInfo^)MarshalV8ToCLR(info[0]);
    try {
      info.GetReturnValue().Set(MarshalCLRToV8(prop->GetValue(target)));
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecPropertySet) {
    try {
      PropertyInfo^ prop = (PropertyInfo^)MarshalV8ToCLR(info[0]);
      prop->SetValue(MarshalV8ToCLR(info[1]), MarshalV8ToCLR(info[2]));
      info.GetReturnValue().Set(Nan::Undefined());
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  // deprecated
  static NAN_METHOD(ExecSetProperty) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      System::Object^ value = MarshalV8ToCLR(info[2]);
      System::String^ field = stringV82CLR(info[1]->ToString());
      target->GetType()->GetProperty(field)->SetValue(target, value);
      delete field;
      info.GetReturnValue().Set(Nan::Undefined());
    } catch(System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  // deprecated
  static NAN_METHOD(ExecGetStaticProperty) {
    try {
      System::Type^ target = (System::Type^)MarshalV8ToCLR(info[0]);
      System::String^ property = stringV82CLR(info[1]->ToString());
      System::Object^ rtn = target->GetProperty(property,
        BindingFlags::Static | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy)->GetValue(nullptr);
      delete property;
      delete target;
      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  // deprecated
  static NAN_METHOD(ExecGetProperty) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      System::String^ property = stringV82CLR(info[1]->ToString());
      try {
        System::Object^ rtn = target->GetType()->GetProperty(property,
          BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy)->GetValue(target);
        delete property;
        delete target;
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      } catch (AmbiguousMatchException^ e) {
        System::Object^ rtn = target->GetType()->GetProperty(property,
          BindingFlags::Instance | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::FlattenHierarchy | BindingFlags::DeclaredOnly)->GetValue(target);
        delete property;
        delete target;
        info.GetReturnValue().Set(MarshalCLRToV8(rtn));
      }
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  // deprecated
  static NAN_METHOD(ExecStaticMethod) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      int argSize = info.Length() - 2;
      cli::array<System::Object^>^ cshargs = gcnew cli::array<System::Object^>(argSize);
 
      for (int i = 0; i < argSize; i++) {
        cshargs->SetValue(MarshalV8ToCLR(info[i + 2]), i);
      }
      System::Type^ type = (System::Type ^)target;
      System::String^ method = stringV82CLR(info[1]->ToString());
      System::Object^ rtn = type->InvokeMember(method,
        BindingFlags::Static | BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::InvokeMethod,
        nullptr,
        target,
        cshargs);

      delete cshargs;
      delete type;
      delete method;
      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  // deprecated
  static NAN_METHOD(ExecMethod) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      int argSize = info.Length() - 2;
      cli::array<System::Object^>^ cshargs = gcnew cli::array<System::Object^>(argSize);

      for(int i=0; i < argSize; i++)
        cshargs->SetValue(MarshalV8ToCLR(info[i + 2]),i);

      System::Type^ type = target->GetType();
      System::String^ method = stringV82CLR(info[1]->ToString());
      System::Object^ rtn = type->InvokeMember(method,
        BindingFlags::Public | BindingFlags::Instance | BindingFlags::NonPublic | BindingFlags::InvokeMethod,
        nullptr,
        target,
        cshargs);
      delete cshargs;
      delete type;
      delete method;
      info.GetReturnValue().Set(MarshalCLRToV8(rtn));
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static NAN_METHOD(ExecMethodObjectAsync) {
    try {
      MethodInfo^ method = (MethodInfo^)MarshalV8ToCLR(info[0]);
      System::Object^ target = MarshalV8ToCLR(info[1]);
      int argSize = info.Length() - 2;
      cli::array<System::Object^>^ cshargs = gcnew cli::array<System::Object^>(argSize);

      for(int i=0; i < argSize; i++)
        cshargs->SetValue(MarshalV8ToCLR(info[i + 2]),i);

      TintInterop::AsyncEventDelegate^ del = gcnew TintInterop::AsyncEventDelegate(target,method,cshargs);
      System::ComponentModel::BackgroundWorker^ worker = gcnew System::ComponentModel::BackgroundWorker();
      worker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(del, &(TintInterop::AsyncEventDelegate::DoWorkHandler));
      worker->RunWorkerAsync();
      info.GetReturnValue().Set(Nan::Undefined());
    } 
    catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

  static void CLR::HandleMessageLoop(System::Windows::Interop::MSG% msg, bool% handled) {
    if(msg.message == WM_APP+1) {
      uv_run_nowait();
    }
  }

  static NAN_METHOD(ExecAddEvent) {
    try {
      System::Object^ target = MarshalV8ToCLR(info[0]);
      System::String^ event = stringV82CLR(info[1]->ToString());
      System::Type^ typeDef = target->GetType();
      if(typeDef->ToString()->Equals("System.RuntimeType")) {
        typeDef = (System::Type^)target;
      }
      System::Reflection::EventInfo^ eInfo = typeDef->GetEvent(event);
      v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[2]);
      CLREventHandler ^handle = gcnew CLREventHandler();
      handle->SetCallback(callback);
      System::Reflection::MethodInfo^ eh = handle->GetType()->GetMethod("EventHandlerOnMain");
      System::Delegate^ d = System::Delegate::CreateDelegate(eInfo->EventHandlerType, handle, eh);
      eInfo->AddEventHandler(target, d);

      GCHandle gc = GCHandle::Alloc(d);
      void *ptr = GCHandle::ToIntPtr(gc).ToPointer();
      info.GetReturnValue().Set(Nan::NewBuffer((char *)ptr, sizeof(ptr) + 1024, gchandle_cleanup, NULL).ToLocalChecked());
    } catch (System::Exception^ e) {
      info.GetReturnValue().Set(throwV8Exception(MarshalCLRExceptionToV8(e)));
    }
  }

};

void browser_callback(uv_async_t* work);

namespace IEWebBrowserFix {
  public ref class ScriptInterface
  {
  public:
    ScriptInterface(v8::Local<v8::Function> cb) {
      Nan::HandleScope scope;
      callback = new Nan::Callback(cb);
      loop = uv_default_loop();
      async = new uv_async_t;
      uv_async_init(loop, async, browser_callback);
      uv_ref((uv_handle_t*)async);
      sem = gcnew Semaphore(0,1);
    }
      
    void postMessageBackOnMain(System::String^ str) {
      if(GetCurrentThreadId() == v8Thread) {
        this->postMessageBack(this->_response);
        return;
      }
      _response = str;
      handle = GCHandle::Alloc(this);
      void *data = (void *)GCHandle::ToIntPtr(handle).ToPointer();
      async->data = data;
      uv_async_send(async);
      sem->WaitOne();
    }

    void postMessageBackOnMainCallback() {
      this->postMessageBack(this->_response);
    }

    [System::Runtime::InteropServices::ComVisibleAttribute(true)]
    void postMessageBack(System::String^ str)
    {
      Nan::HandleScope scope;
      v8::Local<v8::Value> argv[1];

      argv[0] = MarshalCLRToV8(str);

      v8::TryCatch try_catch;

      if (this->callback->IsEmpty()) {
        throw gcnew System::Exception("CLR Fatal: Script bridge callback has been garbage collected.");
		    exit(1);
      } else {
        // invoke the registered callback function
        this->callback->Call(1, reinterpret_cast<v8::Local<v8::Value> *>(argv));
        if(_response != nullptr) {
          sem->Release();
        }
      }
      if (try_catch.HasCaught()) {
        throw gcnew System::Exception(exceptionV82stringCLR(try_catch.Exception()));
        exit(1);
      }
    }

    private:
      uv_loop_t *loop;
      uv_async_t *async;
      Nan::Callback *callback;
      GCHandle handle;
      System::String^ _response;
      Semaphore^ sem;
  };

  static NAN_METHOD(CreateScriptInterface) {
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[0]);
    info.GetReturnValue().Set(MarshalCLRToV8(gcnew ScriptInterface(callback)));
  }

  static void SetBrowserFeatureControlKey(System::Security::Permissions::RegistryPermission^ perms, System::String^ feature, System::String^ fileName, DWORD value) {
    System::String^ HKCU = "HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\"+feature;
    System::String^ HKCU32 = "HKEY_CURRENT_USER\\Software\\Wow6432Node\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\"+feature;
    //System::String^ HKLM = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\"+feature;
    //System::String^ HKLM32 = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\"+feature;

    try {
      perms->AddPathList(System::Security::Permissions::RegistryPermissionAccess::AllAccess, HKCU);
      perms->AddPathList(System::Security::Permissions::RegistryPermissionAccess::AllAccess, HKCU32);
      //perms->AddPathList(System::Security::Permissions::RegistryPermissionAccess::AllAccess, HKLM);
      //perms->AddPathList(System::Security::Permissions::RegistryPermissionAccess::AllAccess, HKLM32);
      Microsoft::Win32::Registry::SetValue(HKCU, fileName, value, Microsoft::Win32::RegistryValueKind::DWord);
      Microsoft::Win32::Registry::SetValue(HKCU32, fileName, value, Microsoft::Win32::RegistryValueKind::DWord);
      //Microsoft::Win32::Registry::SetValue(HKLM, fileName, value, Microsoft::Win32::RegistryValueKind::DWord);
      //Microsoft::Win32::Registry::SetValue(HKLM32, fileName, value, Microsoft::Win32::RegistryValueKind::DWord);
    } catch (System::Exception^ e) {
      // Absorb the potential error message coming through and keep moving; its
      // a security exception that will exist on specific policy groups of windows.
      // Unfortunately this means whatever feature control we'd like to turn on, 
      // we cannot.
    }
  }

  static void SetBrowserFeatureControl() {
    System::Security::Permissions::RegistryPermission^ perms = gcnew System::Security::Permissions::RegistryPermission(System::Security::Permissions::PermissionState::Unrestricted);
    System::Diagnostics::Process ^process = System::Diagnostics::Process::GetCurrentProcess();
    System::String^ fileName = System::IO::Path::GetFileName(process->MainModule->FileName);

    SetBrowserFeatureControlKey(perms, "FEATURE_96DPI_PIXEL", fileName, 1); // enable high-dpi support.
    System::Version^ version = System::Environment::OSVersion->Version;
    if(version->Major < 6) {
      SetBrowserFeatureControlKey(perms, "FEATURE_BROWSER_EMULATION", fileName, 0x0000); // turn off compatibility mode.
    } else {
      // turn off compatibility mode by explicitly requesting IE11.
      // using 0x00000 doesnt work on windows 10, and IE11 is the fallback and wont
      // be continued so its safe to hard code this version for now, long term we need
      // to begin moving to XAML apps and WebView control to take advantage of htmledge.
      SetBrowserFeatureControlKey(perms, "FEATURE_BROWSER_EMULATION", fileName, 11000); 
    }
    SetBrowserFeatureControlKey(perms, "FEATURE_AJAX_CONNECTIONEVENTS", fileName, 1);
    SetBrowserFeatureControlKey(perms, "FEATURE_ENABLE_CLIPCHILDREN_OPTIMIZATION", fileName, 1);
    SetBrowserFeatureControlKey(perms, "FEATURE_GPU_RENDERING", fileName, 1); // use GPU rendering
    SetBrowserFeatureControlKey(perms, "FEATURE_IVIEWOBJECTDRAW_DMLT9_WITH_GDI  ", fileName, 0); // force directX
    SetBrowserFeatureControlKey(perms, "FEATURE_NINPUT_LEGACYMODE", fileName, 0);
    SetBrowserFeatureControlKey(perms, "FEATURE_DISABLE_NAVIGATION_SOUNDS", fileName, 1);
    SetBrowserFeatureControlKey(perms, "FEATURE_SCRIPTURL_MITIGATION", fileName, 1);
    SetBrowserFeatureControlKey(perms, "FEATURE_SPELLCHECKING", fileName, 0);
    SetBrowserFeatureControlKey(perms, "FEATURE_STATUS_BAR_THROTTLING", fileName, 1);
    SetBrowserFeatureControlKey(perms, "FEATURE_VALIDATE_NAVIGATE_URL", fileName, 0);
    SetBrowserFeatureControlKey(perms, "FEATURE_WEBOC_DOCUMENT_ZOOM", fileName, 1); // allow zoom.
    SetBrowserFeatureControlKey(perms, "FEATURE_WEBOC_POPUPMANAGEMENT", fileName, 0); // disallow auto-popups
    SetBrowserFeatureControlKey(perms, "FEATURE_ADDON_MANAGEMENT", fileName, 0); // disallow auto-addons/plugins
    SetBrowserFeatureControlKey(perms, "FEATURE_WEBSOCKET", fileName, 1);
    SetBrowserFeatureControlKey(perms, "FEATURE_WINDOW_RESTRICTIONS", fileName, 0); // disallow popups
    SetBrowserFeatureControlKey(perms, "FEATURE_SECURITYBAND", fileName, 0); // disallow security band (still retains security)
    SetBrowserFeatureControlKey(perms, "FEATURE_LOCALMACHINE_LOCKDOWN", fileName, 1); // allow file's to integrate with IWebBrowser JS execute.
    SetBrowserFeatureControlKey(perms, "FEATURE_BLOCK_LMZ_SCRIPT", fileName, 0); // disable activeX security band warnings on local scripts.
    SetBrowserFeatureControlKey(perms, "FEATURE_BLOCK_LMZ_OBJECT", fileName, 0); // disable activeX security.
    SetBrowserFeatureControlKey(perms, "FEATURE_RESTRICT_ACTIVEXINSTALL", fileName, 0);
    SetBrowserFeatureControlKey(perms, "FEATURE_PROTOCOL_LOCKDOWN", fileName, 0);
    SetBrowserFeatureControlKey(perms, "FEATURE_ZONE_ELEVATION", fileName, 0);
    SetBrowserFeatureControlKey(perms, "FEATURE_SCRIPTURL_MITIGATION", fileName, 0);
  }
}


void browser_callback(uv_async_t* work) {
  IEWebBrowserFix::ScriptInterface^ handle = (IEWebBrowserFix::ScriptInterface^)GCHandle::FromIntPtr(System::IntPtr(work->data)).Target;
  handle->postMessageBackOnMainCallback();
}

extern "C" void CLR_Init(Handle<v8::Object> target) {
  Nan::HandleScope scope;
  v8Thread = GetCurrentThreadId();
  // Fix registry and "FEATURE" controls, these help align IE with a normal behavior
  // expected (on a per app registry setting basis).  This does not set global registry
  // values for anything outside of our application.
  IEWebBrowserFix::SetBrowserFeatureControl();

  // OLD, non-optimized execution methods.
  Nan::SetMethod(target, "execNew", CLR::ExecNew);
  Nan::SetMethod(target, "execAddEvent", CLR::ExecAddEvent);
  Nan::SetMethod(target, "execMethod", CLR::ExecMethod);
  Nan::SetMethod(target, "execStaticMethod", CLR::ExecStaticMethod);
  Nan::SetMethod(target, "execSetField", CLR::ExecSetField);
  Nan::SetMethod(target, "execGetField", CLR::ExecGetField);
  Nan::SetMethod(target, "execGetStaticField", CLR::ExecGetStaticField);
  Nan::SetMethod(target, "execSetProperty", CLR::ExecSetProperty);
  Nan::SetMethod(target, "execGetProperty", CLR::ExecGetProperty);
  Nan::SetMethod(target, "execGetStaticProperty", CLR::ExecGetStaticProperty);

  // get programmatic information
  Nan::SetMethod(target, "getReferencedAssemblies", CLR::GetReferencedAssemblies);
  Nan::SetMethod(target, "getLoadedAssemblies", CLR::GetLoadedAssemblies);
  Nan::SetMethod(target, "loadAssemblyFromMemory", CLR::LoadAssemblyFromMemory);
  Nan::SetMethod(target, "loadAssembly", CLR::LoadAssembly);
  Nan::SetMethod(target, "getMemberTypes", CLR::GetMemberTypes);
  Nan::SetMethod(target, "getStaticMemberTypes", CLR::GetStaticMemberTypes);
  Nan::SetMethod(target, "getCLRType", CLR::GetCLRType);

  // 2nd gen optimized execution
  Nan::SetMethod(target, "getStaticPropertyObject", CLR::ExecGetStaticPropertyObject);
  Nan::SetMethod(target, "getPropertyObject", CLR::ExecGetPropertyObject);
  Nan::SetMethod(target, "getMethodObject", CLR::ExecGetMethodObject);
  Nan::SetMethod(target, "getStaticMethodObject", CLR::ExecGetStaticMethodObject);
  Nan::SetMethod(target, "getProperty", CLR::ExecPropertyGet);
  Nan::SetMethod(target, "setProperty", CLR::ExecPropertySet);
  Nan::SetMethod(target, "callMethod", CLR::ExecMethodObject);
  Nan::SetMethod(target, "callMethodAsync", CLR::ExecMethodObjectAsync);

  // create classes
  Nan::SetMethod(target, "classCreate", CLR::CreateClass);
  Nan::SetMethod(target, "classAddConstructor", CLR::AddConstructor);
  Nan::SetMethod(target, "classAddMethod", CLR::AddMethodToClass);
  Nan::SetMethod(target, "classAddProperty", CLR::AddPropertyToClass);
  Nan::SetMethod(target, "classAddField", CLR::AddFieldToClass);
  Nan::SetMethod(target, "classRegister", CLR::RegisterClass);

  // callback for webviews
  Nan::SetMethod(target, "createScriptInterface", IEWebBrowserFix::CreateScriptInterface);
#ifdef GC_DEBUG
  Nan::SetMethod(target, "getCppClassCount", CLR::GetCppClassCount);
  Nan::SetMethod(target, "getCppCollectCount", CLR::GetCppCollectCount);
#endif

  // Register the thread handle to communicate back to handle application
  // specific events when in WPF mode.
  System::Windows::Interop::ComponentDispatcher::ThreadFilterMessage += 
      gcnew System::Windows::Interop::ThreadMessageEventHandler(CLR::HandleMessageLoop);
}

