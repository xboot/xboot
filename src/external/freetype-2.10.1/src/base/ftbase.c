/****************************************************************************
 *
 * ftbase.c
 *
 *   Single object library component (body only).
 *
 * Copyright (C) 1996-2019 by
 * David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 *
 */


#include <ft2build.h>
#define  FT_MAKE_OPTION_SINGLE_OBJECT

#include "ftadvanc.c.h"
#include "ftcalc.c.h"
#include "ftcolor.c.h"
#include "ftdbgmem.c.h"
#include "fterrors.c.h"
#include "ftfntfmt.c.h"
#include "ftgloadr.c.h"
#include "fthash.c.h"
#include "ftlcdfil.c.h"
#include "ftmac.c.h"
#include "ftobjs.c.h"
#include "ftoutln.c.h"
#include "ftpsprop.c.h"
#include "ftrfork.c.h"
#include "ftsnames.c.h"
#include "ftstream.c.h"
#include "fttrigon.c.h"
#include "ftutil.c.h"


/* END */
