#!/bin/sh
(make realclean windebugconfig; make dep; make ) > "$1" 2>&1 3>&1
