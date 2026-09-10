/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#pragma once

#define VZX_REQ_TRUE(t) \
  if (!(t)) \
    return

#define VZX_REQ_TRUE_V(t, v) \
  if (!(t)) \
    return v

#define VZX_REQ_FALSE_V(t, v) \
  if ((t)) \
    return v

#define VZX_REQ_FALSE(t) \
  if ((t)) \
    return

#define VZX_REQ_TRUE_F(t) \
  if (!(t)) \
    return false

#define VZX_REQ_FALSE_F(t) \
  if ((t)) \
    return false

#define nreq(t) VZX_REQ_FALSE(t)
#define req(t) VZX_REQ_TRUE(t)
#define reqrv(statement, failed_return_value) VZX_REQ_TRUE_V(statement, failed_return_value)
#define reqrf(t) VZX_REQ_TRUE_V(t, false)
#define freq(t) VZX_REQ_FALSE(t)
#define req_constexpr(t) if constexpr(!(t)) \
  return

// require false, otherwise return value
#define freqrv(t, v) VZX_REQ_FALSE_V(t, v)
#define freqrf(t) VZX_REQ_FALSE_V(t, false)

#define req_error(t, err_string) \
  if (!(t)) \
  { \
    VZX_debug_error("In %hs:%d", __FUNCTION__, __LINE__); \
    VZX_printf("%hs", err_string); \
    return; \
  }

#define ret(t) return (void)(t)

#define req_continue(t) \
  if (!(t)) \
    continue

#define req_break(t) \
  if (!(t)) \
    break

// ---------
// Debug requires
#define req_debug(t) \
  if constexpr (VSX_COMMON_DEBUG_OUTPUT == 1) \
    req(t)

#define reqrv_debug(statement, return_value) \
  if constexpr (VSX_COMMON_DEBUG_OUTPUT == 1) \
    reqrv(statement, return_value)

#define continue_after(statement) \
  if (true) {\
    statement;                    \
    continue;\
  }

#define return_after(statement) \
  return [&](){statement;}()

#define return_value_after(statement, value) \
  return [&](){statement; return value;}()

#define break_after(statement) \
  if (true)                             \
  {                               \
    statement;                    \
    break;\
  }

