#CFLAGS = -O3 -Wall
CFLAGS = -g -Wall
files = tpgmain.o input.o level.o sim.o podem.o test.o init_flist.o faultsim.o display.o tdfsim.o tdfatpg.o 
atpg: $(files)
	@echo "> Building atpg"
	@g++ $(CFLAGS) $(files) -o  $@
	

-include $(files:.o=.d)
%.o: %.c
	@echo "> Compiling "$<
	@gcc $(CFLAGS) $< -c -MMD

%.o: %.cpp
	@echo "> Compiling "$<
	@g++ $(CFLAGS) $< -c -MMD


clean:
	rm -f *.o *.d atpg
	
