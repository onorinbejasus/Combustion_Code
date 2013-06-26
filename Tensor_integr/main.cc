#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tensor files
#include <time.h>
#include <fstream>

#include "lib/std/FatalError.hh"
#include "lib/streamlines/Streamline.hh"
#include "lib/std/AllocArray.hh"
#include "lib/glyphs/VectorFieldRenderer.hh"
#include "lib/file_ops/Parameters.hh"
#include "lib/projector/Projector.hh"
//#include "lib/volume/VolumeRenderer.hh"

// open gl
#include "window.hh"
#include "lib/open_gl.hh"

#include <cuda_runtime.h>
#include <iostream>
#include <cmath>

int main(int argc, char **argv) {

printf("start main\n"); fflush(stdout);
	if (argc < 2){
		printf("Need to specify parameter file\n");
		printf("./main [parameter.csv]\n");
		exit(1);
	}

	createWindow(argc, argv);
	startApplication(argc, argv);
printf("end main\n"); fflush(stdout);
	return 0;
}


