# Introduction
A command line tool wrapping around ioperm(2) iopl(2) outb(2), etc.

# Where it is needed
Designed for Linux-as-bootloader-payload schemes like [Heads](https://github.com/osresearch/heads/), in order to perform low-level IO operations, e.g. triggering SMIs.
