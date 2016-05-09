name = main
tmp_name = tmp

all: m168def.inc $(name).hex 
	echo "done"
upload: $(name).hex
	avrdude -v -c stk500v1 -p ATMEGA328P -P /dev/ttyACM0 -b 19200 -U flash:w:$<
clean:
	rm $(name).hex m168def.inc

#add some header
$(tmp_name).s:
ifeq ($(strip $(wildcard $(name).s)),)
	$(error filename should be $(name).s )
endif
	echo ".nolist"  > $@
	echo ".include \"m168def.inc\"" >> $@
	echo ".list" >> $@
	cat $(name).s >> $@
	
#compile 
$(name).hex: $(tmp_name).s
	avra $<
	mv $(tmp_name).s.hex $(name).hex
	rm $(tmp_name).* 

#get atmega168
m168def.inc: 
	sed '/#ifndef\|#define\|#pragma\|#endif/d' /usr/share/avra/m168def.inc > $@

# the command of the header
# see http://forum.arduino.cc/index.php?topic=315.0
# .nolist ; We don't want to actually include defs in our listing file.
# .include "m168def.inc";m168def.inc defines all the pins on the Mega168 so we 
# 		; use them by their names rather than addresses (not fun).
# .list	; We DO want to include the following code in our listing ;D
