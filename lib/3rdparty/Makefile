# -*- makefile -*-
# 
#	makefile for socket library
#
# @author Yvan Castilloux

# create all libraries -> call all makefiles in subdirectories
SUBTARGETS=cJSON

all:
	for d in $(SUBTARGETS); do \
		make -C $$d; \
	done
	
	# TODO fix library paths in dependents to point to this relative directory
	# You shoud run this command manually when it complains about any [driver.so]
	# sudo ln -s -f ${IOTSDK}/c/lib/[driver.so] /lib/[driver.so]

clean:
	for d in $(SUBTARGETS); do \
		make -C $$d clean; \
	done
