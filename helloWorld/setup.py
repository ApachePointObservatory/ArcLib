from setuptools import setup, Extension

extension_mod = Extension( 
        "_helloWorld", 
        ["helloWorld.i", "helloWorld.c"],
        language="c"
        )

setup( name = "helloWorld", ext_modules=[extension_mod])

