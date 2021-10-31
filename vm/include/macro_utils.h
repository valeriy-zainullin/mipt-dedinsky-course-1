#pragma once

#define ARRAY_DEF(...) { __VA_ARGS__ }

#define EXPAND2(MACRO) #MACRO
#define EXPAND(MACRO) EXPAND2(MACRO)
