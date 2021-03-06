/*
 * Copyright © 2019 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of λcommon.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDACOMMON_SERIALIZABLE_H
#define LAMBDACOMMON_SERIALIZABLE_H

#include "lambdacommon.h"
#include <vector>

namespace lambdacommon::serializable
{
    extern std::vector<std::string> LAMBDACOMMON_API tokenize(const std::string& _string, const std::string& delim);
}

#endif //LAMBDACOMMON_SERIALIZABLE_H
