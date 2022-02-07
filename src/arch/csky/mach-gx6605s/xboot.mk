#
# Machine makefile
#

DEFINES     +=

ASFLAGS     := -g -ggdb -Wall -D __ASSEMBLY__
CFLAGS      := -g -ggdb -Wall
LDFLAGS     := -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS     := -mlittle-endian -mcpu=ck610 -Wa,-mcpu=ck610 -fno-tree-vectorize -msoft-float -mdiv

LIBDIRS     :=
LIBS        :=
INCDIRS     :=
SRCDIRS     := arch/csky/mach-gx6605s/driver/video

UNIQUEID    := ""
ENCRYPT_KEY := "679408dc82ae80d411d5d9720b65a43fc4f1534fa563fb28c6cd8928e46aaae9"
PUBLIC_KEY  := "03cfd18e4a4b40d6529448aa2df8bbb677128258b8fbfc5b9e492fbbba4e84832f"
PRIVATE_KEY := "dc57b8a9e0e2b7f8b4c929bd8db2844e53f01f171bbcdf6e628908dbf2b2e6a9"
MESSAGE	    := "https://github.com/xboot/xboot"

export sys-ccu-flags        := -fPIC
export sys-copyself-flags   := -fPIC
export sys-dram-flags       := -fPIC
export sys-spinor-flags     := -fPIC
export sys-uart-flags       := -fPIC
