sim: sim.cpp
	g++ -std=c++11 sim.cpp -o sim
	@echo "-----------DONE WITH SIM-----------"
clean:
	rm -f sim

