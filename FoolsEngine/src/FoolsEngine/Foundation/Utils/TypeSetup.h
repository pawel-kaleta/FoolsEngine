#pragma once

namespace fe
{
#define FE_EXPAND_TYPE(...) __VA_ARGS__

#define FE_TYPE_SETUP(...) FE_EXPAND_TYPE(__VA_ARGS__)* P() { return this; } \
FE_EXPAND_TYPE(__VA_ARGS__)& O() { return *this; }\
template <typename tType>\
	tType& Cast() { return *(tType*)this; }
}