///////////////////////////////////////////////////////////////////////////////
#ifndef __WMQ_INTERFACE_H__
#define __WMQ_INTERFACE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
///////////////////////////////////////////////////////////////////////////////
#include "mesh/extension/ExItems.h"
#include "mesh\extension/ExKernelT.cpp" 
#include "mesh\read_write/read_write.cpp" 
#include <string>
#include <ctime>

typedef MeshN::ExKernelT<MeshN::ExItems>   MyMesh;
typedef MeshN::ReaderWriterT<MyMesh>       Reader;


MyMesh  mesh;
Reader  reader(&mesh);

vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {

		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}


		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

void MyMeshInit(string input_path, int patch_num, double patch_radius) 
{
	reader.off_reader(input_path.c_str());
	mesh.meshInit();
	vector<string> temp = split(input_path, ".");
	string modelName = temp[0];
	mesh.MeshSegment(modelName, patch_num, patch_radius);
}

#endif // 