//
// Copyright © 2019 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "ProfilingGuid.hpp"

#include <string>

namespace arm
{

namespace pipe
{

class IProfilingGuidGenerator
{
public:
    /// Return the next random Guid in the sequence
    virtual ProfilingDynamicGuid NextGuid() = 0;

    /// Create a ProfilingStaticGuid based on a hash of the string
    virtual ProfilingStaticGuid GenerateStaticId(const std::string& str) = 0;

    virtual ~IProfilingGuidGenerator() {}
};

} // namespace pipe

} // namespace arm
