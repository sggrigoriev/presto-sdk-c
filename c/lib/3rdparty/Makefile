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
	# You shoud run this command manually when it complains about -lppcsocket
	# sudo ln -s -f ${PPCHUBDIR}/lib/libppcsocket/libppsocket.so /lib/libppcsocket.so

clean:
	for d in $(SUBTARGETS); do \
		make -C $$d clean; \
	done
