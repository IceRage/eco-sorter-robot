#include "EcoSorterProject.h"
#include <conio.h.>

using namespace std;

int main(int argc, char** argv) {
	EcoSorterProject* project = new EcoSorterProject();

	project->run();

	delete project;

	printf("Press any key to continue...\n");
	_getch();

	return 0;
}