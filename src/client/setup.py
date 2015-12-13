# encoding=utf-8

from distutils.core import setup
import py2exe

setup(windows=['Main.py'], options={'py2exe': {'dll_excludes': ['MSVCP90.dll']}})
