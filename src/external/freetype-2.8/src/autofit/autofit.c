/***************************************************************************/
/*                                                                         */
/*  autofit.c                                                              */
/*                                                                         */
/*    Auto-fitter module (body).                                           */
/*                                                                         */
/*  Copyright 2003-2017 by                                                 */
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

#include "afangles.c.h"
#include "afblue.c.h"
#include "afcjk.c.h"
#include "afdummy.c.h"
#include "afglobal.c.h"
#include "afhints.c.h"
#include "afindic.c.h"
#include "aflatin.c.h"
#include "aflatin2.c.h"
#include "afloader.c.h"
#include "afmodule.c.h"
#include "afpic.c.h"
#include "afranges.c.h"
#include "afshaper.c.h"
#include "afwarp.c.h"


/* END */
