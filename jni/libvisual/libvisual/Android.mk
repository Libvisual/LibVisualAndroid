
# generate version
$(shell ($(call my-dir)/update-version.sh))


include $(call all-subdir-makefiles)
