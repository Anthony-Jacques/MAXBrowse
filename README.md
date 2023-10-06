# MaxBrowse

## History

This is a server browser that I wrote initially to support Quake 4, to go along with Q4Max,
back in 2006. It proved to be popular enough that I then extended it beyond Q4 into a number
of related games, such as ETQW, Warsow etc.

As Q4 got replaced with QuakeLive and then Quake Champions, the need for an external server
browser has reduced, and so I stopped development on it.

## Compiling

I've confirmed that I can build this code using the Visual Studio 2019 Community Edition,
and have made some small minor modifications, but hopefully not broken anything along the way.
The main change made is stripping out a couple of GUI components that I had used which are
open source, but not explicitly GPL licence compatible so I felt were worth removing.

Probably newer compiler toolchains will work too - I just haven't tried. 

## Distribution

The previous MaxBrowse builds are binary only releases, however I have chosen to re-licence
the code as GPL v3. Therefore, you can build and re-distribute based upon this code base
as long as you stick to the GPL license terms.

## IP location detection

MAXBrowse used to use the GeoCountry Lite database to map IP address to country. This is no
longer maintained, and the licence terms of the replacement GeoLite2 Country database put
in place additional requirements regarding updates + deletes as a result of the GDPR
"right to be forgotten" which are hard for me to comply with at this time. Therefore

## Usage

Hopefully it should be easy enough to use - that was the goal! However there is a manual.

## Adding Games

See the SupportedGames.xml file - this can be extended / updated if you can identify existing
games that work but use eg a different master server address. For other games that use a
similar protocol it's probably not too hard to extend one of the existing GameServer or
MasterServer classes to cope with the different format of data.

