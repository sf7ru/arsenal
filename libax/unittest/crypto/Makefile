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

TARGET_NAME				:= test
TARGET_DESC				:= crypto test
TARGET_CODENAME			:= test
TARGET_VERSION			:= 0.01
TARGET_LOCALE			:= 
TARGET_TYPE				:= stdapp
TARGET_OPTIONS			+= addon=BUILDH

TARGET_TOOLCHAIN		= 
TARGET_PATH				?=


## ---------------------------------------------------------------------------
## ------------------------------- SOURCES -----------------------------------
## ---------------------------------------------------------------------------

SOURCE_FILES			=	main.c


SOURCE_BEANS            	:= \
crypto@arsenal/libax		\
strings@arsenal/libax		\

SOURCE_LIBRARIES		:= 

SOURCE_HEADERS			:= libax@arsenal

SOURCE_BINLIBRARIES +=

## ---------------------------------------------------------------------------
## ----------------------- MAIN MAKEFILE INCLUSION ---------------------------
## ---------------------------------------------------------------------------

JKL=$(firstword $(foreach d,./ u uu uuu uuuu uuuuu,$(wildcard $(subst u,../,$(d))jackal/) ))
include $(JKL)./makefiles/Makefile
