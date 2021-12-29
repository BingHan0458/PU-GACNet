#include <iostream>
#include "MeshInterface.h"

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("You must enter 5 arguments.\n");
		exit(-1);
	}
	else
	{
		string input_path = argv[1];
		int patch_num = stoi(argv[2]);
		double patch_radius = stod(argv[3]);

		cout << "input mesh file path is: " << input_path << endl;
		cout << "Patch number is " << patch_num << endl;
		cout << "Patch radius is " << patch_radius << endl;

		MyMeshInit(input_path, patch_num, patch_radius);
	}	
	return 0;
}