# generate version
TMP = $(shell $(call my-dir)/update-version.sh)

include $(call all-subdir-makefiles)
