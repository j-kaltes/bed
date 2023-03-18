#!/bin/sh
SRCNUM=$1
SRCNUMBAK=$1.bak
mv $SRCNUM $SRCNUMBAK
num=`cat $SRCNUMBAK`
num=$((num+1))
echo $num> $SRCNUM



