// Copyright (C) 2024 owoDra

#pragma once

#include "Logging/LogMacros.h"

GLHADDON_API DECLARE_LOG_CATEGORY_EXTERN(LogGLHA, Log, All);

#if !UE_BUILD_SHIPPING

#define GLHALOG(FormattedText, ...) UE_LOG(LogGLHA, Log, FormattedText, __VA_ARGS__)

#define GLHAENSURE(InExpression) ensure(InExpression)
#define GLHAENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GLHAENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GLHACHECK(InExpression) check(InExpression)
#define GLHACHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GLHALOG(FormattedText, ...)

#define GLHAENSURE(InExpression) InExpression
#define GLHAENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GLHAENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GLHACHECK(InExpression) InExpression
#define GLHACHECK_MSG(InExpression, InFormat, ...) InExpression

#endif