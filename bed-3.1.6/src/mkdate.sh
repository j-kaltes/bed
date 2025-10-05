#!/bin/bash

function mkdate() {
	echo -n 'char srcdate[]="' >date.c
	echo -n `date` >> date.c
	echo  '";' >> date.c
	}

mkdate

