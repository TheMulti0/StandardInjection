# Dll Injection

This is a sample (based on [ZeroMemory's initial project](https://github.com/Zer0Mem0ry/StandardInjection)) of injecting a custom DLL into a running process.

The way the code works is the following:

1. The dll's full path is allocated in the target process's memory
2. A thread is opened in the target process
3. The thread loads the dll by calling [LoadLibrary](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya) and suppling the allocated path to the dll file
4. The handles and threads are freed

The `DllInjector` makes use of modern C++ features to wrap the lifecycle of the loaded DLL, process and thread handles.

Injecting using pid and window name are currently supported.