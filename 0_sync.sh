#!/bin/bash

find . -type f -name *.c -exec cp -f ../../arsenal/{} {} \;
find . -type f -name *.h -exec cp -f ../../arsenal/{} {} \;
find . -type f -name castfile -exec cp -f ../../arsenal/{} {} \;
