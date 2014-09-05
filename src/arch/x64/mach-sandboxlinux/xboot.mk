#
# Machine makefile
#

DEFINES		+=	-D__X64_ARCH__ -D__SANDBOX__

#
# Sandbox namespace
#
NS_CTYPE	:=	-Disalnum=xboot_isalnum -Disalpha=xboot_isalpha \
				-Disalpha=xboot_isalpha -Disascii=xboot_isascii \
				-Disblank=xboot_isblank -Discntrl=xboot_iscntrl \
				-Disdigit=xboot_isdigit -Disgraph=xboot_isgraph \
				-Dislower=xboot_islower -Disprint=xboot_isprint \
				-Dispunct=xboot_ispunct -Disspace=xboot_isspace \
				-Disupper=xboot_isupper -Disxdigit=xboot_isxdigit \
				-Dtoascii=xboot_toascii -Dtolower=xboot_tolower \
				-Dtoupper=xboot_toupper

NS_ENVIRON	:=	-Dgetenv=xboot_getenv -Dputenv=xboot_putenv \
				-Dsetenv=xboot_setenv -Dunsetenv=xboot_unsetenv \
				-Dclearenv=xboot_clearenv

NS_ERRNO	:=	-Dstrerror=xboot_strerror

NS_EXIT		:=	-Dabort=xboot_abort -Dexit=xboot_exit

NS_JMP		:=	-Dsetjmp=xboot_setjmp -Dlongjmp=xboot_longjmp

NS_LOCALE	:=	-Dsetlocale=xboot_setlocale -Dlocaleconv=xboot_localeconv \

NS_MALLOC	:=	-Dmalloc=xboot_malloc -Dmemalign=xboot_memalign \
				-Drealloc=xboot_realloc -Dcalloc=xboot_calloc \
				-Dfree=xboot_free

NS_READLINE	:=	-Dreadline=xboot_readline

NS_STDIO	:=	-Dfopen=xboot_fopen \
				-Dfreopen=xboot_freopen -Dfclose=xboot_fclose \
				-Dfeof=xboot_feof -Dferror=xboot_ferror \
				-Dfflush=xboot_fflush -Dclearerr=xboot_clearerr \
				-Dfseek=xboot_fseek -Dftell=xboot_ftell \
				-Dgetc=xboot_getc -Dputc=xboot_putc \
				-Drewind=xboot_rewind -Dfgetpos=xboot_fgetpos \
				-Dfsetpos=xboot_fsetpos -Dfread=xboot_fread \
				-Dfwrite=xboot_fwrite -Dfgetc=xboot_fgetc \
				-Dfgets=xboot_fgets -Dfputc=xboot_fputc \
				-Dfputs=xboot_fputs -Dungetc=xboot_ungetc \
				-Dsetvbuf=xboot_setvbuf -Dsetbuf=xboot_setbuf \
				-Dtmpfile=xboot_tmpfile -Dfprintf=xboot_fprintf \
				-Dfscanf=xboot_fscanf -Dvsnprintf=xboot_vsnprintf \
				-Dvsscanf=xboot_vsscanf -Dsprintf=xboot_sprintf \
				-Dsnprintf=xboot_snprintf -Dsscanf=xboot_sscanf

NS_STDLIB	:=	-Drand=xboot_rand -Dsrand=xboot_srand \
				-Dabs=xboot_abs -Dlabs=xboot_labs \
				-Dllabs=xboot_llabs -Dllabs=xboot_llabs \
				-Datoi=xboot_atoi -Datol=xboot_atol \
				-Datoll=xboot_atoll -Datof=xboot_atof \
				-Dstrtol=xboot_strtol -Dstrtoll=xboot_strtoll \
				-Dstrtoul=xboot_strtoul -Dstrtoull=xboot_strtoull \
				-Dstrtod=xboot_strtod -Dstrtoimax=xboot_strtoimax \
				-Dstrtoumax=xboot_strtoumax -Dstrntoimax=xboot_strntoimax \
				-Dstrntoumax=xboot_strntoumax -Dbsearch=xboot_bsearch \
				-Ddiv=xboot_div -Dldiv=xboot_ldiv \
				-Dlldiv=xboot_lldiv -Dqsort=xboot_qsort

NS_STRING	:=	-Dstrcpy=xboot_strcpy -Dstrncpy=xboot_strncpy \
				-Dstrlcpy=xboot_strlcpy -Dstrcat=xboot_strcat \
				-Dstrncat=xboot_strncat -Dstrlcat=xboot_strlcat \
				-Dstrlen=xboot_strlen -Dstrnlen=xboot_strnlen \
				-Dstrcmp=xboot_strcmp -Dstrncmp=xboot_strncmp \
				-Dstrnicmp=xboot_strnicmp -Dstrcasecmp=xboot_strcasecmp \
				-Dstrncasecmp=xboot_strncasecmp -Dstrcoll=xboot_strcoll \
				-Dstrchr=xboot_strchr -Dstrrchr=xboot_strrchr \
				-Dstrnchr=xboot_strnchr -Dstrspn=xboot_strspn \
				-Dstrcspn=xboot_strcspn -Dstrpbrk=xboot_strpbrk \
				-Dstrstr=xboot_strstr -Dstrnstr=xboot_strnstr \
				-Dstrdup=xboot_strdup -Dstrsep=xboot_strsep \
				-Dmemset=xboot_memset -Dmemcpy=xboot_memcpy \
				-Dmemmove=xboot_memmove -Dmemchr=xboot_memchr \
				-Dmemscan=xboot_memscan -Dmemcmp=xboot_memcmp

NS_TIME		:=	-Dstrftime=xboot_strftime -Dasctime=xboot_asctime \
				-Dclock=xboot_clock -Dctime=xboot_ctime \
				-Ddifftime=xboot_difftime -Dgettimeofday=xboot_gettimeofday \
				-Dgmtime=xboot_gmtime -Dlocaltime=xboot_localtime \
				-Dtime=xboot_time

NS_MATH		:=	-D__ieee754_rem_pio2=xboot___ieee754_rem_pio2 -D__kernel_rem_pio2=xboot___kernel_rem_pio2 \
				-D__kernel_sin=xboot___kernel_sin -D__kernel_cos=xboot___kernel_cos \
				-D__kernel_tan=xboot___kernel_tan -D__ieee754_rem_pio2f=xboot___ieee754_rem_pio2f \
				-D__kernel_rem_pio2f=xboot___kernel_rem_pio2f -D__kernel_sindf=xboot___kernel_sindf \
				-D__kernel_cosdf=xboot___kernel_cosdf -D__kernel_tandf=xboot___kernel_tandf \
				-Dcopysign=xboot_copysign -Dcopysignf=xboot_copysignf \
				-Dscalbn=xboot_scalbn -Dldexp=xboot_ldexp \
				-Dscalbnf=xboot_scalbnf -Dldexpf=xboot_ldexpf \
				-Dexpm1=xboot_expm1 -Dexpm1f=xboot_expm1f \
				-Dfabs=xboot_fabs -Dfabsf=xboot_fabsf \
				-Dceil=xboot_ceil -Dceilf=xboot_ceilf \
				-Dfloor=xboot_floor -Dfloorf=xboot_floorf \
				-Dmodf=xboot_modf -Dmodff=xboot_modff \
				-Dfrexp=xboot_frexp -Dfrexpf=xboot_frexpf \
				-Dsqrt=xboot_sqrt -Dsqrtf=xboot_sqrtf \
				-Dexp=xboot_exp -Dexpf=xboot_expf \
				-Dfmod=xboot_fmod -Dfmodf=xboot_fmodf \
				-Dpow=xboot_pow -Dpowf=xboot_powf \
				-Dlog=xboot_log -Dlogf=xboot_logf \
				-Dlog10=xboot_log10 -Dlog10f=xboot_log10f \
				-Dsin=xboot_sin -Dsinf=xboot_sinf \
				-Dcos=xboot_cos -Dcosf=xboot_cosf \
				-Dtan=xboot_tan -Dtanf=xboot_tanf \
				-Dsinh=xboot_sinh -Dsinhf=xboot_sinhf \
				-Dcosh=xboot_cosh -Dcoshf=xboot_coshf \
				-Dtanh=xboot_tanh -Dtanhf=xboot_tanhf \
				-Dasin=xboot_asin -Dasinf=xboot_asinf \
				-Dacos=xboot_acos -Dacosf=xboot_acosf \
				-Datan=xboot_atan -Datanf=xboot_atanf \
				-Datan2=xboot_atan2 -Datan2f=xboot_atan2f \
				-Dhypot=xboot_hypot -Dhypotf=xboot_hypotf

NS_FILEIO	:=	-Dmount=xboot_mount -Dsync=xboot_sync \
				-Dumount=xboot_umount -Dopen=xboot_open \
				-Dread=xboot_read -Dwrite=xboot_write \
				-Dlseek=xboot_lseek -Dfstat=xboot_fstat \
				-Dioctl=xboot_ioctl -Dfsync=xboot_fsync \
				-Dclose=xboot_close -Dopendir=xboot_opendir \
				-Dreaddir=xboot_readdir -Drewinddir=xboot_rewinddir \
				-Dclosedir=xboot_closedir -Dgetcwd=xboot_getcwd \
				-Dchdir=xboot_chdir -Dmkdir=xboot_mkdir \
				-Drmdir=xboot_rmdir -Dstat=xboot_stat \
				-Daccess=xboot_access -Drename=xboot_rename \
				-Dunlink=xboot_unlink -Dmknod=xboot_mknod \
				-Dchmod=xboot_chmod -Dchown=xboot_chown \
				-Dumask=xboot_umask -Dftruncate=xboot_ftruncate \
				-Dtruncate=xboot_truncate -Dreadv=xboot_readv \
				-Dwritev=xboot_writev

NS_TEMP		:=	-Dmktime=xboot_mktime -Dctrlc=xboot_ctrlc \
				-Dexec_cmdline=xboot_exec_cmdline -Dparser=xboot_parser

DEFINES		+=	$(NS_CTYPE) $(NS_ENVIRON) $(NS_ERRNO) $(NS_EXIT) \
				$(NS_JMP) $(NS_LOCALE) $(NS_MALLOC) $(NS_READLINE) \
				$(NS_STDIO) $(NS_STDLIB) $(NS_STRING) $(NS_TIME) \
				$(NS_MATH) $(NS_FILEIO) $(NS_TEMP)

ASFLAGS		:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:= -d
MCFLAGS		:=

LIBDIRS		:= arch/$(ARCH)/$(MACH)
LIBS 		:= -lsandboxlinux -lc -lgcc -lgcc_eh -lpthread $(shell pkg-config sdl --libs)

INCDIRS		:=
SRCDIRS		:=
