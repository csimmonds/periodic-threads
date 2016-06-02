# Periodic pthreads
This repository contains some sample code showing how to create periodic threads
in Linux. There is a full description of the code in my post on the Inner Penguin blog,
named [Over and over again: periodic tasks in Linux](http://2net.co.uk/tutorial/periodic_threads)

# Native compile
```
    $ make
```

# Cross compile
Override CC with the compiler you want to use, for example:
```
    make CC=~/x-tools/arm-cortex_a8-linux-gnueabihf/bin/arm-cortex_a8-linux-gnueabihf-gcc
```

# Cross compile with a Yocto Project SDK
When you install a Yocto Project SDK, you will find a script that sets up the environment
for the toolchain. Among other things, it sets the CC shell variable. So, to build
the samples, you would do something like this:

```
    $ . /opt/poky/2.0.1/environment-setup-armv5e-poky-linux-gnueabi
    $ make
```

