/**
 * @file vapi_export.hpp
 * @brief Export/import macro for building or using VAPI OSP as a shared library (DLL on Windows).
 *
 * When building the library as DLL, define VAPI_OSP_BUILDING so that VAPI_API becomes dllexport.
 * When linking to the DLL, do not define VAPI_OSP_BUILDING so that VAPI_API becomes dllimport.
 * For static build or non-Windows, VAPI_API is empty.
 */

#ifndef VAPI_EXPORT_HPP
#define VAPI_EXPORT_HPP

#if defined(_WIN32) || defined(__CYGWIN__)
#  if defined(VAPI_OSP_BUILDING)
#    define VAPI_API __declspec(dllexport)
#  else
#    define VAPI_API __declspec(dllimport)
#  endif
#elif defined(__GNUC__) || defined(__clang__)
#  if defined(VAPI_OSP_BUILDING)
#    define VAPI_API __attribute__((visibility("default")))
#  else
#    define VAPI_API
#  endif
#else
#  define VAPI_API
#endif

#endif // VAPI_EXPORT_HPP
