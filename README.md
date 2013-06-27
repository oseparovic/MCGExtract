MCGExtract
==========

MCGExtract is a QT based GUI crossplatform(eventually) application that will assist with viewing, extracting, and recompressing Mechcommander Gold propietary compression formats. This will include .PAK, .FST and .PKK files.

The GUI will be powered by CMunsta's command line based extraction utilities. You can see a full list of them and their sources on [his website][1].
[1]: http://therealcmunsta.webs.com/downloads.htm        "CMunsta"

Task List
---------

- [x] initial UI menu and table setup
- [x] push all relevant utility source code
- [ ] find a way to use lzcompress and lzdecomp without needing to use Visual Studio for it's handling of the `_asm` define.
- [ ] determine a proper way to populate file indices and implement. Currently just listing them from indices built by myself and scrivener
- [ ] wire up the FST extraction code to the UI
- [ ] wire up the PAK extraction code to the UI (need to get it from CMunsta first)
- [ ] wire up the PKK extraction code to the UI
