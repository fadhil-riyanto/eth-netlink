// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */

#ifndef INTERNAL_H
#define INTERNAL_H 1

#ifdef HAVE_VISIBILITY_HIDDEN
#	define EXPORT_SYMBOL __attribute__((visibility("default")))
#else
#	define EXPORT_SYMBOL
#endif

#ifndef INTERNAL_FUNC
#       define INTERNAL_SYMBOL __attribute__ ((visibility ("hidden")))
#endif

#endif
