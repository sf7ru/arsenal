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

TARGET_NAME				:= essays
TARGET_DESC				:= essays
TARGET_CODENAME			:= essays
TARGET_VERSION			:= 0.01
TARGET_LOCALE			:= 
TARGET_TYPE				:= stdapp
TARGET_OPTIONS			+= addon=BUILDH DEBUGGER=satellite

TARGET_TOOLCHAIN		= 
TARGET_PATH				?=


## ---------------------------------------------------------------------------
## ------------------------------- SOURCES -----------------------------------
## ---------------------------------------------------------------------------

SOURCE_FILES			=	main.cpp


SOURCE_BEANS            := debug@arsenal/liba7 threads@arsenal/liba7

SOURCE_LIBRARIES		:= 

SOURCE_HEADERS			:= liba7@arsenal

SOURCE_BINLIBRARIES +=

## ---------------------------------------------------------------------------
## ----------------------- MAIN MAKEFILE INCLUSION ---------------------------
## ---------------------------------------------------------------------------

JKL=$(firstword $(foreach d,./ u uu uuu uuuu uuuuu,$(wildcard $(subst u,../,$(d))jackal/) ))
include $(JKL)./makefiles/Makefile
