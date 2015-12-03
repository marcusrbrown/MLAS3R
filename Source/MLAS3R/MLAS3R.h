// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

// Use these macros to disable and enable optimization on a function-by-function basis. They should be placed
// immediately before and after each function where optimization is to be disabled and enabled.
#if defined(__clang__)
#define MLAS3R_DISABLE_OPTIMIZATION __attribute__((optnone))
#define MLAS3R_ENABLE_OPTIMIZATION
#elif defined(_MSC_VER)
#define MLAS3R_DISABLE_OPTIMIZATION PRAGMA_DISABLE_OPTIMIZATION_ACTUAL
#define MLAS3R_ENABLE_OPTIMIZATION PRAGMA_ENABLE_OPTIMIZATION_ACTUAL
#endif