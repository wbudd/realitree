// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

#pragma once

#include "realitree.h"

rt_ret load_from_file(
    uint32_t * client_offset
);

rt_ret store_as_file(
    uint32_t client_offset
);
