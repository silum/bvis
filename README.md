# bvis -- binary visualiser

`bvis` is a tool to visualise binary files.  The concept is strongly borrowed
from [binvis.io](http://binvis.io) and from
[scurve](https://github.com/cortesi/scurve), both by Aldo Cortesi.

Currently it outputs a [Netpbm](https://en.wikipedia.org/wiki/Netpbm_format)
stream to standard output.  Capture this stream and convert it to a more
convenient file format.  Hint, on `macOS`, write it to file, and use `sips` to
convert it to, for instance a [Portable Network
Graphics](https://en.wikipedia.org/wiki/Portable_Network_Graphics) image file
as follows:

    $ bvis > vis.pnm
    $ sips -s format png vis.pnm --out vis.png

`bvis` is written in C in the C99 standard, and has no external dependencies.
