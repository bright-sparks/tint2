@echo off
:: Oh GYP and MSVS do not make for good bed fellows.
:: We must manually delete all the stuff MSVS placed
:: around the workspace. Unfortunately due to a bug in
:: GYP (the way it makes MSVS files) this is unavoidable.
rmdir /S /Q .\build\msvs\
rmdir /S /Q .\build\Release\
rmdir /S /Q .\build\Debug\
rmdir /S /Q .\build\out\
rmdir /S /Q .\build\ninja\
rmdir /S /Q .\Debug
rmdir /S /Q .\Release
rmdir /S /Q .\libraries\dlfcn-win32\Debug
rmdir /S /Q .\libraries\dlfcn-win32\Release
del /S /Q .\libraries\dlfcn-win32\dlfcn.sln
del /S /Q .\libraries\dlfcn-win32\dlfcn.vcxproj
del /S /Q node_dtrace_header.vcxproj
del /S /Q node_dtrace_provider.vcxproj
del /S /Q node_dtrace_ustack.vcxproj
del /S /Q node_etw.vcxproj
del /S /Q node_js2c.vcxproj
del /S /Q node_js2c.vcxproj.filters
del /S /Q node_perfctr.vcxproj
del /S /Q node_systemtap_header.vcxproj
del /S /Q ref_binding.vcxproj
del /S /Q ref_binding.vcxproj.filters
del /S /Q tint.sln
del /S /Q tint.vcxproj
del /S /Q tint.vcxproj.filters
del /S /Q tint_js2c.vcxproj
del /S /Q tint_js2c.vcxproj.filters
del /S /Q tint_runtime.vcxproj
del /S /Q tint_runtime.vcxproj.filters
del /S /Q tint_clr.vcxproj
del /S /Q tint_clr.vcxproj.filters
del /S /Q ffi_bindings.vcxproj
del /S /Q ffi_bindings.vcxproj.filters
del /S /Q libraries\node\node.sln
del /S /Q libraries\node\node.vcxproj
del /S /Q libraries\node\node.vcxproj.filters