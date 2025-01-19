#pragma once

#ifdef GITWRAP_EXPORTS
#define GITWRAP_API __declspec(dllexport)
#else
#define GITWRAP_API __declspec(dllimport)
#endif