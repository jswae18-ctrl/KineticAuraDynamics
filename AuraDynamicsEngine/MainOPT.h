#pragma once

// Target Windows 7 or later with optimized Win32 lean definitions
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

#ifndef FULL_WINTARD
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
// #define NOSYSMETRICS      // <-- COMMENTED OUT: Restores GetSystemMetrics
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
// #define NOWINOFFSETS      // <-- COMMENTED OUT: Restores SetWindowLongPtr & GWLP offsets
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#endif

#define NOMINMAX
#define STRICT

#include <Windows.h>
