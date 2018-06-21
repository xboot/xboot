/***************************************************************************/
/*                                                                         */
/*  gxvalid.c                                                              */
/*                                                                         */
/*    FreeType validator for TrueTypeGX/AAT tables (body only).            */
/*                                                                         */
/*  Copyright 2005-2017 by                                                 */
/*  suzuki toshiya, Masatake YAMATO, Red Hat K.K.,                         */
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

#include "gxvbsln.c.h"
#include "gxvcommn.c.h"
#include "gxvfeat.c.h"
#include "gxvjust.c.h"
#include "gxvkern.c.h"
#include "gxvlcar.c.h"
#include "gxvmod.c.h"
#include "gxvmort.c.h"
#include "gxvmort0.c.h"
#include "gxvmort1.c.h"
#include "gxvmort2.c.h"
#include "gxvmort4.c.h"
#include "gxvmort5.c.h"
#include "gxvmorx.c.h"
#include "gxvmorx0.c.h"
#include "gxvmorx1.c.h"
#include "gxvmorx2.c.h"
#include "gxvmorx4.c.h"
#include "gxvmorx5.c.h"
#include "gxvopbd.c.h"
#include "gxvprop.c.h"
#include "gxvtrak.c.h"


/* END */
