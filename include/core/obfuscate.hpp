#ifndef VAPI_CORE_OBFUSCATE_HPP
#define VAPI_CORE_OBFUSCATE_HPP

#include "core/types.hpp"

#define VAPI_OBF_LIT(s) (::vapi::ObfString<sizeof(s)>(::vapi::detail::obf_xor(s, ::vapi::detail::obf_key)))

#ifdef VAPI_OBFUSCATE
#define VAPI_OBF(s) VAPI_OBF_LIT(s)
#else
#define VAPI_OBF(s) (::vapi::ObfPassthrough(s))
#endif

#endif
