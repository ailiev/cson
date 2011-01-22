FAERIEPLAY_DIST_ROOT ?= $(HOME)/faerieplay

DIST_ROOT ?= $(FAERIEPLAY_DIST_ROOT)

ifeq ($(DIST_ROOT),)
$(error Please set the environment variable FAERIEPLAY_DIST_ROOT)
endif
