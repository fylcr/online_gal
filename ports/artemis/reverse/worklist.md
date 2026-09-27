# Reached functions, by call count

Of 5579 instrumented functions, **2105** were entered
during this run (37.7%).

This is the triage list for a readable rewrite: the top of the table is where
naming and restructuring pays, the count-1 tail is where it does not.

| call count | functions |
| --- | --- |
| >= 1e6 | 0 |
| 1e5 .. 1e6 | 25 |
| 1e4 .. 1e5 | 80 |
| 1e3 .. 1e4 | 192 |
| 100 .. 1e3 | 219 |
| 10 .. 100 | 397 |
| 2 .. 10 | 378 |
| 1 | 814 |

## Hottest 60

| # | index | calls | first-run order | name | subsystems |
| --- | --- | --- | --- | --- | --- |
| 1 | 202 | 453.0K | 420 | — | — |
| 2 | 201 | 424.7K | 419 | — | — |
| 3 | 779 | 310.0K | 772 | — | — |
| 4 | 428 | 294.7K | 1104 | — | — |
| 5 | 393 | 279.0K | 976 | — | — |
| 6 | 244 | 218.4K | 1085 | — | — |
| 7 | 1727 | 218.4K | 1086 | — | — |
| 8 | 463 | 213.0K | 464 | — | — |
| 9 | 200 | 198.1K | 577 | — | — |
| 10 | 206 | 171.7K | 824 | — | — |
| 11 | 5508 | 171.7K | 825 | — | — |
| 12 | 643 | 155.2K | 773 | — | — |
| 13 | 400 | 148.8K | 473 | — | — |
| 14 | 340 | 125.2K | 1105 | — | — |
| 15 | 203 | 122.9K | 780 | — | — |
| 16 | 898 | 120.8K | 796 | — | — |
| 17 | 326 | 118.1K | 795 | — | — |
| 18 | 805 | 117.6K | 608 | — | — |
| 19 | 777 | 117.6K | 785 | — | — |
| 20 | 4832 | 116.6K | 1168 | — | — |
| 21 | 247 | 113.4K | 758 | — | — |
| 22 | 4551 | 109.0K | 1167 | — | — |
| 23 | 778 | 108.9K | 1088 | — | — |
| 24 | 1111 | 104.6K | 771 | — | — |
| 25 | 210 | 101.9K | 779 | — | — |
| 26 | 5721 | 86.8K | 746 | — | — |
| 27 | 265 | 83.9K | 477 | — | — |
| 28 | 910 | 76.0K | 1110 | — | — |
| 29 | 708 | 74.4K | 1092 | — | — |
| 30 | 707 | 74.0K | 1109 | — | — |
| 31 | 364 | 69.6K | 1114 | — | — |
| 32 | 223 | 67.5K | 751 | — | — |
| 33 | 645 | 66.6K | 1091 | — | — |
| 34 | 399 | 63.0K | 747 | — | — |
| 35 | 476 | 62.1K | 1103 | — | — |
| 36 | 4579 | 59.6K | 904 | — | — |
| 37 | 346 | 54.6K | 1100 | — | — |
| 38 | 1116 | 50.6K | 1090 | — | — |
| 39 | 204 | 49.7K | 573 | — | — |
| 40 | 904 | 41.7K | 766 | — | call |
| 41 | 1128 | 41.3K | 791 | — | — |
| 42 | 209 | 39.6K | 768 | — | — |
| 43 | 207 | 37.8K | 448 | — | — |
| 44 | 1125 | 37.2K | 802 | — | — |
| 45 | 453 | 36.1K | 826 | — | — |
| 46 | 414 | 36.0K | 1107 | — | — |
| 47 | 4715 | 34.4K | 1159 | — | — |
| 48 | 970 | 34.3K | 463 | — | — |
| 49 | 303 | 31.1K | 804 | — | — |
| 50 | 1233 | 31.1K | 451 | — | — |
| 51 | 592 | 30.5K | 691 | — | — |
| 52 | 226 | 30.5K | 450 | — | — |
| 53 | 1550 | 30.2K | 692 | — | — |
| 54 | 2801 | 30.2K | 891 | — | — |
| 55 | 896 | 30.1K | 770 | — | — |
| 56 | 4082 | 30.1K | 953 | — | — |
| 57 | 1071 | 29.3K | 955 | — | — |
| 58 | 3784 | 29.3K | 954 | — | — |
| 59 | 1042 | 29.2K | 893 | — | backlog |
| 60 | 3785 | 29.2K | 952 | — | — |

## Naming coverage

- functions with a name in the map: **21 / 2105**
- functions with a subsystem tag: **27 / 2105**

### Named or tagged, by call count

| index | calls | name | subsystems |
| --- | --- | --- | --- |
| 904 | 41.7K | — | call |
| 1042 | 29.2K | — | backlog |
| 3551 | 948 | — | alreadyread |
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
