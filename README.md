# fakefs

Aug 16, 2022;

fakefs -m [mount point] -n [srv name]
added by https://github.com/nevermarine

If any one else wants to add other features, like commandline args or other systhetic files, feel free to contribute.

~~~~~

This is an example for a simple Read Only synthetic file system for Plan 9.

It will place an entry in /srv and mount in /n by default

In it will be 3 files

/fake/backtalk
uses getuser() to generate a responce to being read

/fake/i-ching
uses nrand() to make an I Ching hexagram

/fake/tyme
uses time(0) to generate a message about the seconds on the Unix Epoch.


This was inspired by gpsfs when looking for examples of 
Plan 9 style file systems for reading sensors.  
