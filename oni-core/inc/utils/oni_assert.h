#pragma once

#include "debugassert.h"

struct oni_assert : debug_assert::default_handler, debug_assert::set_level<1> {
};

#define ONI_DEBUG_ASSERT(condition) DEBUG_ASSERT(condition, oni_assert{});