#
# Machine makefile
#

#
# Sandbox namespace
#
NS_JMP		:=	-Dsetjmp=xboot_setjmp -Dlongjmp=xboot_longjmp

NS_CTYPE	:=	-Disalnum=xboot_isalnum -Disalpha=xboot_isalpha \
				-Disascii=xboot_isascii -Disblank=xboot_isblank \
				-Discntrl=xboot_iscntrl -Disdigit=xboot_isdigit \
				-Disgraph=xboot_isgraph -Dislower=xboot_islower \
				-Disprint=xboot_isprint -Dispunct=xboot_ispunct \
				-Disspace=xboot_isspace -Disupper=xboot_isupper \
				-Disxdigit=xboot_isxdigit -Dtoascii=xboot_toascii \
				-Dtolower=xboot_tolower -Dtoupper=xboot_toupper

NS_ENVIRON	:=	-Dclearenv=xboot_clearenv -Dgetenv=xboot_getenv \
				-Dputenv=xboot_putenv -Dsetenv=xboot_setenv \
				-Dunsetenv=xboot_unsetenv

NS_ERRNO	:=	-Dstrerror=xboot_strerror

NS_EXIT		:=	-Dabort=xboot_abort -Dexit=xboot_exit

NS_LOCALE	:=	-Dlocaleconv=xboot_localeconv -Dsetlocale=xboot_setlocale 

NS_MALLOC	:=	-Dmalloc=xboot_malloc -Dmemalign=xboot_memalign \
				-Drealloc=xboot_realloc -Dcalloc=xboot_calloc \
				-Dfree=xboot_free

NS_PATH		:=	-Dbasename=xboot_basename -Ddirname=xboot_dirname

NS_STDIO	:=	-Dasprintf=xboot_asprintf -Dclearerr=xboot_clearerr \
				-Dfclose=xboot_fclose -Dfeof=xboot_feof \
				-Dferror=xboot_ferror -Dfflush=xboot_fflush \
				-Dfgetc=xboot_fgetc -Dfgetpos=xboot_fgetpos \
				-Dfgets=xboot_fgets -Dfopen=xboot_fopen \
				-Dfprintf=xboot_fprintf -Dfputc=xboot_fputc \
				-Dfputs=xboot_fputs -Dfread=xboot_fread \
				-Dfreopen=xboot_freopen -Dfscanf=xboot_fscanf \
				-Dfseek=xboot_fseek -Dfsetpos=xboot_fsetpos \
				-Dftell=xboot_ftell -Dfwrite=xboot_fwrite \
				-Dgetc=xboot_getc -Dgetchar=xboot_getchar \
				-Dprintf=xboot_printf -Dputchar=xboot_putchar \
				-Dputc=xboot_putc -Dremove=xboot_remove \
				-Drename=xboot_rename -Drewind=xboot_rewind \
				-Dscanf=xboot_scanf -Dsetbuf=xboot_setbuf \
				-Dsetvbuf=xboot_setvbuf -Dsnprintf=xboot_snprintf \
				-Dsprintf=xboot_sprintf -Dsscanf=xboot_sscanf \
				-Dsystem=xboot_system -Dtmpfile=xboot_tmpfile \
				-Dtmpnam=xboot_tmpnam -Dungetc=xboot_ungetc \
				-Dvasprintf=xboot_vasprintf -Dvsnprintf=xboot_vsnprintf \
				-Dvsscanf=xboot_vsscanf

NS_STDLIB	:=	-Dabs=xboot_abs -Datof=xboot_atof \
				-Datoi=xboot_atoi -Datol=xboot_atol \
				-Datoll=xboot_atoll -Dbsearch=xboot_bsearch \
				-Ddiv=xboot_div -Dlabs=xboot_labs \
				-Dldiv=xboot_ldiv -Dllabs=xboot_llabs \
				-Dlldiv=xboot_lldiv -Dqsort=xboot_qsort \
				-Drand=xboot_rand -Dsrand=xboot_srand \
				-Dstrntoimax=xboot_strntoimax -Dstrntoumax=xboot_strntoumax \
				-Dstrtod=xboot_strtod -Dstrtoimax=xboot_strtoimax \
				-Dstrtol=xboot_strtol -Dstrtoll=xboot_strtoll \
				-Dstrtoul=xboot_strtoul -Dstrtoull=xboot_strtoull \
				-Dstrtoumax=xboot_strtoumax

NS_STRING	:=	-Dmemchr=xboot_memchr -Dmemcmp=xboot_memcmp \
				-Dmemcpy=xboot_memcpy -Dmemmove=xboot_memmove \
				-Dmemscan=xboot_memscan -Dmemset=xboot_memset \
				-Dstrcasecmp=xboot_strcasecmp -Dstrcat=xboot_strcat \
				-Dstrchr=xboot_strchr -Dstrcmp=xboot_strcmp \
				-Dstrcoll=xboot_strcoll -Dstrcpy=xboot_strcpy \
				-Dstrcspn=xboot_strcspn -Dstrdup=xboot_strdup \
				-Dstrlcat=xboot_strlcat -Dstrlcpy=xboot_strlcpy \
				-Dstrlen=xboot_strlen -Dstrncasecmp=xboot_strncasecmp \
				-Dstrncat=xboot_strncat -Dstrnchr=xboot_strnchr \
				-Dstrncmp=xboot_strncmp -Dstrncpy=xboot_strncpy \
				-Dstrnicmp=xboot_strnicmp -Dstrnlen=xboot_strnlen \
				-Dstrnstr=xboot_strnstr -Dstrpbrk=xboot_strpbrk \
				-Dstrrchr=xboot_strrchr -Dstrsep=xboot_strsep \
				-Dstrspn=xboot_strspn -Dstrstr=xboot_strstr

NS_TIME		:=	-Dasctime=xboot_asctime -Dclock=xboot_clock \
				-Dctime=xboot_ctime -Ddifftime=xboot_difftime \
				-Dgettimeofday=xboot_gettimeofday -Dgmtime=xboot_gmtime \
				-Dlocaltime=xboot_localtime -Dmktime=xboot_mktime \
				-Dstrftime=xboot_strftime -Dtime=xboot_time

NS_MATH		:=	-Dacos=xboot_acos -Dacosf=xboot_acosf \
				-Dacosh=xboot_acosh -Dacoshf=xboot_acoshf \
				-Dasin=xboot_asin -Dasinf=xboot_asinf \
				-Dasinh=xboot_asinh -Dasinhf=xboot_asinhf \
				-Datan=xboot_atan -Datanf=xboot_atanf \
				-Datan2=xboot_atan2 -Datan2f=xboot_atan2f \
				-Datanh=xboot_atanh -Datanhf=xboot_atanhf \
				-Dcbrt=xboot_cbrt -Dcbrtf=xboot_cbrtf \
				-Dceil=xboot_ceil -Dceilf=xboot_ceilf \
				-Dcos=xboot_cos -Dcosf=xboot_cosf \
				-Dcosh=xboot_cosh -Dcoshf=xboot_coshf \
				-Dexp=xboot_exp -Dexpf=xboot_expf \
				-Dexp2=xboot_exp2 -Dexp2f=xboot_exp2f \
				-Dexpm1=xboot_expm1 -Dexpm1f=xboot_expm1f \
				-Dfabs=xboot_fabs -Dfabsf=xboot_fabsf \
				-Dfdim=xboot_fdim -Dfdimf=xboot_fdimf \
				-Dfloor=xboot_floor -Dfloorf=xboot_floorf \
				-Dfmod=xboot_fmod -Dfmodf=xboot_fmodf \
				-Dfrexp=xboot_frexp -Dfrexpf=xboot_frexpf \
				-Dhypot=xboot_hypot -Dhypotf=xboot_hypotf \
				-Dldexp=xboot_ldexp -Dldexpf=xboot_ldexpf \
				-Dlog=xboot_log -Dlogf=xboot_logf \
				-Dlog10=xboot_log10 -Dlog10f=xboot_log10f \
				-Dlog1p=xboot_log1p -Dlog1pf=xboot_log1pf \
				-Dlog2=xboot_log2 -Dlog2f=xboot_log2f \
				-Dmodf=xboot_modf -Dmodff=xboot_modff \
				-Dpow=xboot_pow -Dpowf=xboot_powf \
				-Drint=xboot_rint -Drintf=xboot_rintf \
				-Dround=xboot_round -Droundf=xboot_roundf \
				-Dscalbn=xboot_scalbn -Dscalbnf=xboot_scalbnf \
				-Dscalbln=xboot_scalbln -Dscalblnf=xboot_scalblnf \
				-Dsin=xboot_sin -Dsinf=xboot_sinf \
				-Dsinh=xboot_sinh -Dsinhf=xboot_sinhf \
				-Dsqrt=xboot_sqrt -Dsqrtf=xboot_sqrtf \
				-Dtan=xboot_tan -Dtanf=xboot_tanf \
				-Dtanh=xboot_tanh -Dtanhf=xboot_tanhf \
				-Dtrunc=xboot_trunc -Dtruncf=xboot_truncf \
				-D__cos=xboot___cos -D__cosdf=xboot___cosdf \
				-D__expo2=xboot___expo2 -D__expo2f=xboot___expo2f \
				-D__fpclassify=xboot___fpclassify -D__fpclassifyf=xboot___fpclassifyf \
				-D__rem_pio2_large=xboot___rem_pio2_large -D__rem_pio2=xboot___rem_pio2 \
				-D__rem_pio2f=xboot___rem_pio2f -D__sin=xboot___sin \
				-D__sindf=xboot___sindf -D__tan=xboot___tan \
				-D__tandf=xboot___tandf

NS_SHELL	:=	-Dreadline=xboot_readline

DEFINES		+=	$(NS_JMP) $(NS_CTYPE) $(NS_ENVIRON) $(NS_ERRNO) \
				$(NS_EXIT) $(NS_LOCALE) $(NS_MALLOC) $(NS_PATH) \
				$(NS_STDIO) $(NS_STDLIB) $(NS_STRING) $(NS_TIME) \
				$(NS_MATH) $(NS_SHELL)

DEFINES		+=	-D__SANDBOX__

XFLAGS		:= `pkg-config --cflags libdrm` $(shell sdl2-config --cflags)
XLIBS		:= `pkg-config --libs libdrm` $(shell sdl2-config --libs)

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld
MCFLAGS		:= -m64 -mmmx -msse -msse2 -mssse3 -mfpmath=sse

LIBDIRS		:= arch/$(ARCH)/$(MACH)/libsandbox
LIBS 		:= -lsandbox -lc -lm -lgcc -lrt -lpthread -lasound -lprofiler $(XLIBS)
INCDIRS		:= arch/$(ARCH)/$(MACH)/libsandbox
SRCDIRS		:=

xbegin:
	@echo Building libsandbox
	@CROSS_COMPILE="$(CROSS_COMPILE)" ASFLAGS="$(XFLAGS)" CFLAGS="$(XFLAGS)" $(MAKE) -C arch/$(ARCH)/$(MACH)/libsandbox

xclean:
	@$(MAKE) -C arch/$(ARCH)/$(MACH)/libsandbox clean
