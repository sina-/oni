#pragma once

#include <oni-core/utils/debug-assert.h>

struct oniAssert : debug_assert::default_handler, debug_assert::set_level<1> {
};

#define ONI_DEBUG_ASSERT(condition) DEBUG_ASSERT(condition, oniAssert{});