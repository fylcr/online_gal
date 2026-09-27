# Reached functions, by call count

Of 5579 instrumented functions, **2103** were entered
during this run (37.7%).

This is the triage list for a readable rewrite: the top of the table is where
naming and restructuring pays, the count-1 tail is where it does not.

| call count | functions |
| --- | --- |
| >= 1e6 | 0 |
| 1e5 .. 1e6 | 18 |
| 1e4 .. 1e5 | 79 |
| 1e3 .. 1e4 | 182 |
| 100 .. 1e3 | 237 |
| 10 .. 100 | 397 |
| 2 .. 10 | 380 |
| 1 | 810 |

## Hottest 60

| # | index | calls | first-run order | name | subsystems |
| --- | --- | --- | --- | --- | --- |
| 1 | 202 | 452.4K | 420 | — | — |
| 2 | 201 | 424.4K | 419 | — | — |
| 3 | 428 | 294.7K | 1104 | — | — |
| 4 | 393 | 279.0K | 976 | — | — |
| 5 | 779 | 263.5K | 772 | — | — |
| 6 | 244 | 218.4K | 1085 | — | — |
| 7 | 1727 | 218.4K | 1086 | — | — |
| 8 | 463 | 212.8K | 464 | — | — |
| 9 | 200 | 197.2K | 577 | — | — |
| 10 | 643 | 154.2K | 773 | — | — |
| 11 | 400 | 148.6K | 473 | — | — |
| 12 | 206 | 138.4K | 824 | — | — |
| 13 | 5508 | 138.4K | 825 | — | — |
| 14 | 340 | 125.2K | 1105 | — | — |
| 15 | 805 | 117.6K | 608 | — | — |
| 16 | 247 | 112.6K | 758 | — | — |
| 17 | 778 | 108.9K | 1088 | — | — |
| 18 | 1111 | 102.9K | 771 | — | — |
| 19 | 898 | 94.4K | 796 | — | — |
| 20 | 203 | 93.6K | 780 | — | — |
| 21 | 326 | 91.7K | 795 | — | — |
| 22 | 265 | 83.6K | 477 | — | — |
| 23 | 5721 | 83.5K | 746 | — | — |
| 24 | 910 | 76.0K | 1110 | — | — |
| 25 | 708 | 74.4K | 1092 | — | — |
| 26 | 210 | 74.2K | 779 | — | — |
| 27 | 707 | 74.0K | 1109 | — | — |
| 28 | 777 | 73.3K | 785 | — | — |
| 29 | 364 | 69.6K | 1114 | — | — |
| 30 | 645 | 66.6K | 1091 | — | — |
| 31 | 4832 | 66.5K | 1168 | — | — |
| 32 | 4551 | 65.1K | 1167 | — | — |
| 33 | 223 | 64.2K | 751 | — | — |
| 34 | 476 | 62.1K | 1103 | — | — |
| 35 | 399 | 60.2K | 747 | — | — |
| 36 | 4579 | 59.3K | 904 | — | — |
| 37 | 346 | 54.6K | 1100 | — | — |
| 38 | 1116 | 50.6K | 1090 | — | — |
| 39 | 204 | 49.4K | 573 | — | — |
| 40 | 207 | 37.5K | 448 | — | — |
| 41 | 1125 | 37.2K | 802 | — | — |
| 42 | 414 | 36.0K | 1107 | — | — |
| 43 | 970 | 34.3K | 463 | — | — |
| 44 | 303 | 31.1K | 804 | — | — |
| 45 | 1233 | 31.1K | 451 | — | — |
| 46 | 592 | 30.5K | 691 | — | — |
| 47 | 226 | 30.5K | 450 | — | — |
| 48 | 1550 | 30.2K | 692 | — | — |
| 49 | 2801 | 29.9K | 891 | — | — |
| 50 | 4082 | 29.8K | 953 | — | — |
| 51 | 209 | 29.7K | 768 | — | — |
| 52 | 1071 | 29.3K | 955 | — | — |
| 53 | 3784 | 29.3K | 954 | — | — |
| 54 | 1042 | 29.2K | 893 | — | backlog |
| 55 | 3785 | 29.2K | 952 | — | — |
| 56 | 902 | 28.6K | 1087 | — | — |
| 57 | 896 | 28.5K | 770 | — | — |
| 58 | 1119 | 28.1K | 1098 | — | — |
| 59 | 221 | 27.8K | 1645 | — | — |
| 60 | 220 | 27.6K | 593 | — | — |

## Naming coverage

- functions with a name in the map: **21 / 2103**
- functions with a subsystem tag: **27 / 2103**

### Named or tagged, by call count

| index | calls | name | subsystems |
| --- | --- | --- | --- |
| 1042 | 29.2K | — | backlog |
| 904 | 24.5K | — | call |
| 3551 | 674 | — | alreadyread |
| 3650 | 365 | — | delonautomodein, delonautomodeout, delonbacklogin, delonbacklogout, delonclick, deloncommandskipin, deloncommandskipout, deloncontrolskipin, deloncontrolskipout, delondirchg, delondrag, delondragin, delondragout, delonhidein, delonhideout, delonpush, delonrollout, delonrollover, delonsoundfinish, delonvideofinish, delonwindowbutton |
| 3661 | 112 | — | lyevent, reset |
| 1081 | 86 | — | reset |
| 3651 | 57 | — | setonautomodein, setonautomodeout, setonbacklogin, setonbacklogout, setonclick, setoncommandskipin, setoncommandskipout, setoncontrolskipin, setoncontrolskipout, setondirchg, setondrag, setondragin, setondragout, setonhidein, setonhideout, setonpush, setonrollout, setonrollover, setonsoundfinish, setonvideofinish, setonwindowbutton |
| 4553 | 51 | — | reset |
| 3633 | 32 | — | base64_encode, character_reference_to_utf8, convert_encoding, date, delete, explode, file_crc, file_exist, file_exists, file_update_time, find, get_backlog_size, get_backlog_tags, get_layer_info, get_message_layer_height, get_message_layer_line_width, get_message_layer_width, get_message_tags, get_sound_info, hmac_sha1_base64, implode, length, os, random, screen_height, screen_width, substr, to_sjis, to_utf8, unixtime, url_decode, url_encode, var_exist |
| 3222 | 28 | — | else, elseif, if, loop, lua |
| 859 | 25 | — | lua |
| 3688 | 22 | — | hide |
| 3088 | 17 | — | video |
| 3695 | 12 | — | backlog |
| 3698 | 11 | — | video |
| 1406 | 4 | — | reset |
| 3666 | 2 | — | automode, backlog, hide, rclick, skip |
| 1465 | 1 | — | reset |
| 3112 | 1 | — | debug, file, random, tag, var |
| 3395 | 1 | — | reset |
| 3558 | 1 | — | reset |
| 3626 | 1 | — | reset |
| 3744 | 1 | — | alreadyread, anime, avoid, backlog, calllua, callnative, caption, chgmsg, debug, debugprint, debugreload, delonautomodein, delonautomodeout, delonbacklogin, delonbacklogout, delonclick, deloncommandskipin, deloncommandskipout, deloncontrolskipin, deloncontrolskipout, delondirchg, delondrag, delondragin, delondragout, delonhidein, delonhideout, delonpush, delonrollout, delonrollover, delonsoundfinish, delonvideofinish, delonwindowbutton, dialog, elseif, fontdefault, glyph, httpget, ime, indent, keyconfig, linkdisable, linkenable, lua, lyc, lydel, lydrag, lyedit, lyevent, lyprop, lytween, lytweendel, mouse, openbrowser, print, rclick, reset, return, savess, scein, sceout, sefade, sefadeout, sepan, seplay, sestop, setonautomodein, setonautomodeout, setonbacklogin, setonbacklogout, setonclick, setoncommandskipin, setoncommandskipout, setoncontrolskipin, setoncontrolskipout, setondirchg, setondrag, setondragin, setondragout, setonhidein, setonhideout, setonpush, setonrollout, setonrollover, setonsoundfinish, setonvideofinish, setonwindowbutton, sfade, sfadein, slider, splay, sstop, statusbar, sxfade, tag, takess, trans, uidel, var, vibrate, video, voice, wordparts, writebacklog |
| 5551 | 1 | — | os |
| 5596 | 1 | — | else, elseif, if, return |
| 5623 | 1 | — | debug, os |
| 5670 | 1 | — | debug |
