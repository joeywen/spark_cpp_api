#!/bin/bash

MAKEFILE=$1

make -f $MAKEFILE clean && make -f $MAKEFILE
