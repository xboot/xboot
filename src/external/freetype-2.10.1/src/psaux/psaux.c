/****************************************************************************
 *
 * psaux.c
 *
 *   FreeType auxiliary PostScript driver component (body only).
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


#define FT_MAKE_OPTION_SINGLE_OBJECT
#include <ft2build.h>

#include "afmparse.c.h"
#include "psauxmod.c.h"
#include "psconv.c.h"
#include "psobjs.c.h"
#include "t1cmap.c.h"
#include "t1decode.c.h"
#include "cffdecode.c.h"

#include "psarrst.c.h"
#include "psblues.c.h"
#include "pserror.c.h"
#include "psfont.c.h"
#include "psft.c.h"
#include "pshints.c.h"
#include "psintrp.c.h"
#include "psread.c.h"
#include "psstack.c.h"


/* END */
