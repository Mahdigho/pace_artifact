# msp430Base
Base Generator repo for msp430 intermittent projects w/ Abstract group's maker.

This repo comes with submodules, to ensure stability of your project, don't forget to make your own branch of the submodules, just in case things break.

Structure:

    + Makefile                         : general make specifying Makefile shared across all apps + includes maker
    + Makefile.env                     : Makefile environment variables specific to user arch/build directories
    +--+ apps/
        +--+ appfoo/
            + Makefile                 : build recipe that defines the source files to build and configures dependencies
            +--+ src/                  : contains application source files and headers
            +--+ bld/                  : contains the build artifacts in one subdirectory per toolchain
               +--+ toolchainA/
               +--+ toolchainB/
               +--+ ...
    +--+ tools/
       +--+ maker/                     : maker itself is a dependency included as a submodule
    +--+ ext/                          : contains dependencies (optionally but usually, included as Git submodules)
       +--+ libbar/                    : repository with the dependency source code (usually a Git submodule)
       +--+ ...
       +--+ toolchainA/                : some dependencies may be toolchains that are used to build the application
       +--+ toolchainB/
       +--+ ...

Files/directories not included with the repo:
* `Makefile.env`
* `apps/*/bld`
* `tools/tools`


Other files you may wish to include but are ignored by git:
* `.clangd`
* `.editorconfig`

