# Reached functions, by call count

Of 5579 instrumented functions, **2105** were entered
during this run (37.7%).

This is the triage list for a readable rewrite: the top of the table is where
naming and restructuring pays, the count-1 tail is where it does not.

| call count | functions |
| --- | --- |
| >= 1e6 | 0 |
| 1e5 .. 1e6 | 24 |
| 1e4 .. 1e5 | 80 |
| 1e3 .. 1e4 | 190 |
| 100 .. 1e3 | 222 |
| 10 .. 100 | 400 |
| 2 .. 10 | 381 |
| 1 | 808 |

## Hottest 60

| # | index | calls | first-run order | name | subsystems |
| --- | --- | --- | --- | --- | --- |
| 1 | 202 | 452.9K | 420 | — | — |
| 2 | 201 | 424.7K | 419 | — | — |
| 3 | 779 | 302.7K | 772 | — | — |
| 4 | 428 | 294.7K | 1105 | — | — |
| 5 | 393 | 279.0K | 977 | — | — |
| 6 | 244 | 218.4K | 1086 | — | — |
| 7 | 1727 | 218.4K | 1087 | — | — |
| 8 | 463 | 213.0K | 464 | — | — |
| 9 | 200 | 197.9K | 577 | — | — |
| 10 | 206 | 166.8K | 824 | — | — |
| 11 | 5508 | 166.8K | 825 | — | — |
| 12 | 643 | 154.9K | 773 | — | — |
| 13 | 400 | 148.8K | 473 | — | — |
| 14 | 340 | 125.2K | 1106 | — | — |
| 15 | 203 | 118.8K | 780 | — | — |
| 16 | 805 | 117.6K | 608 | — | — |
| 17 | 898 | 117.0K | 796 | — | — |
| 18 | 326 | 114.2K | 795 | — | — |
| 19 | 247 | 113.3K | 758 | — | — |
| 20 | 777 | 110.7K | 785 | — | — |
| 21 | 778 | 108.9K | 1089 | — | — |
| 22 | 4832 | 108.1K | 1169 | — | — |
| 23 | 1111 | 104.3K | 771 | — | — |
| 24 | 4551 | 101.1K | 1168 | — | — |
| 25 | 210 | 98.1K | 779 | — | — |
| 26 | 5721 | 86.3K | 746 | — | — |
| 27 | 265 | 83.9K | 477 | — | — |
| 28 | 910 | 76.0K | 1111 | — | — |
| 29 | 708 | 74.4K | 1093 | — | — |
| 30 | 707 | 74.0K | 1110 | — | — |
| 31 | 364 | 69.6K | 1115 | — | — |
| 32 | 223 | 67.0K | 751 | — | — |
| 33 | 645 | 66.6K | 1092 | — | — |
| 34 | 399 | 62.5K | 747 | — | — |
| 35 | 476 | 62.1K | 1104 | — | — |
| 36 | 4579 | 59.6K | 905 | — | — |
| 37 | 346 | 54.6K | 1101 | — | — |
| 38 | 1116 | 50.6K | 1091 | — | — |
| 39 | 204 | 49.7K | 573 | — | — |
| 40 | 904 | 39.4K | 766 | — | call |
| 41 | 1128 | 39.1K | 791 | — | — |
| 42 | 209 | 38.1K | 768 | — | — |
| 43 | 207 | 37.7K | 448 | — | — |
| 44 | 1125 | 37.2K | 802 | — | — |
| 45 | 414 | 36.0K | 1108 | — | — |
| 46 | 453 | 34.7K | 826 | — | — |
| 47 | 970 | 34.3K | 463 | — | — |
| 48 | 4715 | 31.9K | 1160 | — | — |
| 49 | 303 | 31.1K | 804 | — | — |
| 50 | 1233 | 31.1K | 451 | — | — |
| 51 | 592 | 30.5K | 691 | — | — |
| 52 | 226 | 30.5K | 450 | — | — |
| 53 | 1550 | 30.2K | 692 | — | — |
| 54 | 2801 | 30.1K | 892 | — | — |
| 55 | 4082 | 30.0K | 954 | — | — |
| 56 | 896 | 29.9K | 770 | — | — |
| 57 | 1071 | 29.3K | 956 | — | — |
| 58 | 3784 | 29.3K | 955 | — | — |
| 59 | 1042 | 29.2K | 894 | — | backlog |
| 60 | 3785 | 29.2K | 953 | — | — |

## Naming coverage

- functions with a name in the map: **21 / 2105**
- functions with a subsystem tag: **27 / 2105**

### Named or tagged, by call count

| index | calls | name | subsystems |
| --- | --- | --- | --- |
| 904 | 39.4K | — | call |
| 1042 | 29.2K | — | backlog |
| 3551 | 902 | — | alreadyread |
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
