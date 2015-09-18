#cREST
Multithreaded REST client for Windows

##Design
* Built in Visual Studio 2015 (C++), cREST performs simple HTTP requests with the most common HTTP methods, and displays the corresponding responses. 
* The core and GUI are written with the Win32 API, and networking is implemented with the Berkeley sockets API via Winsock2.
* No HTTP implementation wrappers are used; all requests and responses are handled via specially constructed sockets, allowing for extreme responsiveness.
* HTTP requests are given their own thread in which to operate, so as to not block the application in the event of the hanging on the network or server. The threads post messages to the main thread as they receive data, which the main thread interprets and then posts a corresponding message to the main dialog box.
* Informative dialog box informs user of the status of their request, and reports errors including DNS lookup failures and socket communication errors.

##Usage
* Select an HTTP method from the top left dialog (default is GET), enter your URL on the line directly below, then press ENTER or click the SEND button and watch the magic happen!
* Currently only supports default paths ("/") and implies HTTP:
>###OK:
>* www.foo.com
>* foo.com
>* bar.foo.com
>### Error (or client hang):
>* http://www.foo.com
>* https://www.foo.com
>* www.foo.com/bar
>* www.foo.com?bar
>* www.foo.com#bar

>

##Build
* Open the solution in Visual Studio 15, and build the Release configuration, specifying x86 or x64 depending on your machine.
* If you cannot open the solution, then the project can be compiled with the following specifications:
  - Should compile & link successfully in most Windows environments.
  - Specify C++11 syntax.
  - Link against 'Ws_2.lib' for Winsock2, which should be available in most recent Windows SDKs. The same goes for the corresponding DLL.
  - If building still fails, make sure that all of the Win32 libraries and DLLs are available to your linker.