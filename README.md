MCGExtract
==========

MCGExtract is a QT based GUI crossplatform application that will assist with viewing, extracting, and recompressing Mechcommander Gold propietary compression formats. This will include `.PAK`, `.FST` and `.PKK` files.

The GUI will be powered by CMunsta's command line based extraction utilities. You can see a full list of them and their sources on [his website][1].


Task List
---------

- [x] initial UI menu and table setup
- [x] push all relevant utility source code
- [_] find a way to use lzcompress and lzdecomp without needing to use Visual Studio for it's handling of the `_asm` define.
- [_] determine a proper way to populate file indices and implement. Currently just listing them from indices built by myself and scrivener
- [_] wire up the FST extraction code to the UI
- [_] wire up the PAK extraction code to the UI (need to get it from CMunsta first)
- [_] wire up the PKK extraction code to the UI

Setup
-----

__1.__      Install a QT development IDE. I'm using this version of [Netbeans][2].
__2.__      Run your project! If you've followed the steps above it should be that simple.


[1]: http://therealcmunsta.webs.com/downloads.htm
[2]: https://netbeans.org/kb/docs/cnd/qt-applications.html
