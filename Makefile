# Android Makefile v0.1 (c) 2011 - Daniel Hiepler <daniel@niftylight.de>
# Additions v0.2 (c) 2012 made by Scott Sibley <sisibley@gmail.com>


APPNAME = LibVisual
ACTIVITY = LibVisual
TARGET = android-8

all: bin/$(ACTIVITY)-release-unsigned.apk
bin/$(ACTIVITY)-release-unsigned.apk:
	ndk-build APP_OPTIM=release
	ant clean
	ant release

install: bin/$(APPNAME).apk
	adb install -r bin/$(APPNAME).apk

install_emu: bin/$(APPNAME).apk
	adb -e install -r bin/$(APPNAME).apk

install_dev: bin/$(APPNAME).apk
	adb -d install -r bin/$(APPNAME).apk


debug: bin/$(APPNAME)-debug.apk
bin/$(APPNAME)-debug.apk:
	ndk-build NDK_DEBUG=1 APP_OPTIM=debug V=1
	ant clean
	ant debug

install_debug: bin/$(APPNAME)-debug.apk
	adb install -r bin/$(APPNAME)-debug.apk

install_debug_emu: bin/$(APPNAME)-debug.apk
	adb -e install -r bin/$(APPNAME)-debug.apk

install_debug_dev: bin/$(APPNAME)-debug.apk
	adb -d install -r bin/$(APPNAME)-debug.apk
 
clean:
	ndk-build clean
	ant clean
	rm -rf bin obj gen

update:
	android update project --path . --target $(TARGET)

keygen: my.keystore
my.keystore:
	keytool -genkey -v -keystore my.keystore -alias $(APPNAME)_key -keyalg RSA -keysize 4096 -validity 100000

sign: bin/$(APPNAME)
bin/$(APPNAME).apk: my.keystore bin/$(ACTIVITY)-release-unsigned.apk
	jarsigner -keystore my.keystore bin/$(ACTIVITY)-release-unsigned.apk $(APPNAME)_key
	zipalign -v 4 bin/$(ACTIVITY)-release-unsigned.apk bin/$(APPNAME).apk

log:
	adb pull /data/data/org.libvisual.android/lib/ lyrical/data/data/org.libvisual.android/
	/opt/arm-2011.09/bin/arm-none-linux-gnueabi-objdump -S obj/local/armeabi/libvisual.so > libvisual.asm
	adb shell logcat -d > test.log
	./tools/stack.py --symbols-dir=./lyrical/ ./test.log 
	#./tools/parse_stack.py ./libvisual.asm ./test.log
	adb shell dumpsys meminfo -h > meminfo.txt

gdb:
	ndk-gdb --start --force --verbose

#Perform leak detection
mem1:
	adb shell setprop dalvik.vm.checkjni true
	adb shell setprop libc.debug.malloc 1
	adb shell setprop dalvik.vm.jniopts forcecopy
	adb shell stop
	adb shell start

#Fill allocated memory to detect overruns
mem5:
	adb shell setprop dalvik.vm.checkjni true
	adb shell setprop libc.debug.malloc 5
	adb shell setprop dalvik.vm.jniopts forcecopy
	adb shell stop
	adb shell start

#Fill memory and add sentinels to detect overruns
mem10:
	adb shell setprop dalvik.vm.checkjni true
	adb shell setprop libc.debug.malloc 10
	adb shell setprop dalvik.vm.jniopts forcecopy
	adb shell stop
	adb shell start

#Use special instrumented malloc/free routines for the emulator. (Emulator only!)
mem20:
	adb shell setprop dalvik.vm.checkjni true
	adb shell setprop libc.debug.malloc 20
	adb shell setprop dalvik.vm.jniopts forcecopy
	adb shell stop
	adb shell start

start:
	adb shell am start -n org.libvisual.android/.LibVisual

start_dev:
	adb -d shell am start -n org.libvisual.android/.LibVisual

start_emu:
	adb -e shell am start -n org.libvisual.android/.LibVisual

valgrind:
	adb shell setprop wrap.org.libvisual.android "logwrapper valgrind"

redirect:
	adb shell stop
	adb shell setprop log.redirect-stdio true
	adb shell start

profile:
	./tools/gprof.sh


