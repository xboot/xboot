/***************************************************************************/
/*                                                                         */
/*  cff.c                                                                  */
/*                                                                         */
/*    FreeType OpenType driver component (body only).                      */
/*                                                                         */
/*  Copyright 1996-2017 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#define FT_MAKE_OPTION_SINGLE_OBJECT
#include <ft2build.h>

#include "cffcmap.c.h"
#include "cffdrivr.c.h"
#include "cffgload.c.h"
#include "cffparse.c.h"
#include "cffpic.c.h"
#include "cffload.c.h"
#include "cffobjs.c.h"

#include "cf2arrst.c.h"
#include "cf2blues.c.h"
#include "cf2error.c.h"
#include "cf2font.c.h"
#include "cf2ft.c.h"
#include "cf2hints.c.h"
#include "cf2intrp.c.h"
#include "cf2read.c.h"
#include "cf2stack.c.h"


/* END */
