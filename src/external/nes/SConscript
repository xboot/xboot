from building import *
import os

cwd = GetCurrentDir()
src = Glob('src/*.c') + Glob('src/nes_mapper/*.c') + Glob('rtthread/*.c')
CPPPATH = [cwd + "/inc" , cwd + "/rtthread"]

LOCAL_CFLAGS = ''

if rtconfig.PLATFORM in ['gcc', 'armclang']:
    LOCAL_CFLAGS += ' -std=c11'
elif rtconfig.PLATFORM in ['armcc']:
    LOCAL_CFLAGS += ' --c11'

group = DefineGroup('NES', src, depend = ['PKG_USING_NES_SIMULATOR'], CPPPATH = CPPPATH, LOCAL_CFLAGS = LOCAL_CFLAGS)

Return('group')
