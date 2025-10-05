#!/bin/bash

function mkinfo() {
	echo -n "`hostname`, " > machine
	pwd >> machine
	}

mkinfo

