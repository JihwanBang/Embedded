all : key, key_nb key_nb_pc 

key: 
	arm-linux-gnueabihf-gcc -o test_single_key test_single_key.c

key_nb: 
	arm-linux-gnueabihf-gcc -o test_single_key_nb test_single_key_nb.c
key_nb_pc:
	gcc -o test_single_key_nb_pc test_single_key_nb.c
	
alg:
	gcc -o algo_metronome_tui algo_metronome_tui.c -pthread
alg_cc:
	arm-linux-gnueabihf-gcc -o algo_metronome_tui_beagle algo_metronome_tui.c -pthread
key_pc:
	gcc -o test_single_key_pc test_single_key.c
clean: 
	rm test_single_key test_single_key_nb 
