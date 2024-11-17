output: heuristicasClique.o
	gcc heuristicasClique.o -o heuristicasClique

nRainhas.o: heuristicasClique.c
	gcc -c heuristicasClique.c

clean:
	rm *.o heuristicasClique