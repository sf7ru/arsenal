## ***************************************************************************
## TITLE
##      Makefile
## PROJECT
##      Locator/RACE IGS/Dispatcher
## ***************************************************************************
##
## FILE
##      $Id:$
## HISTORY
##      $Log:$
## ***************************************************************************




## ---------------------------------------------------------------------------
## ------------------------------- OPTIONS -----------------------------------
## ---------------------------------------------------------------------------

TARGET_NAME				:= test-filewatch
TARGET_DESC				:= test-filewatch
TARGET_CODENAME			:= test-filewatch
TARGET_VERSION			:= 0.01
TARGET_LOCALE			:= 
TARGET_TYPE				:= stdapp
TARGET_OPTIONS			+= addon=BUILDH

TARGET_TOOLCHAIN		= 
TARGET_PATH				?=


## ---------------------------------------------------------------------------
## ------------------------------- SOURCES -----------------------------------
## ---------------------------------------------------------------------------

SOURCE_FILES			=	main.cpp


SOURCE_BEANS            := files/watch@arsenal/liba7x

SOURCE_LIBRARIES		:= liba7@arsenal

SOURCE_HEADERS			:= 

SOURCE_BINLIBRARIES += 

## ---------------------------------------------------------------------------
## ----------------------- MAIN MAKEFILE INCLUSION ---------------------------
## ---------------------------------------------------------------------------

JKL=$(firstword $(foreach d,./ u uu uuu uuuu uuuuu,$(wildcard $(subst u,../,$(d))jackal/) ))
include $(JKL)./makefiles/Makefile
