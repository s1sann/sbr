#include "IOManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

bool OBJReader::read(const std::string& _filename, PolyMesh* mesh) {
	fstream ifile(_filename.c_str(), std::ios_base::in);
	string::size_type dot_pos = _filename.rfind("/");

	bool result = loadMeshFromOBJ(ifile, mesh);

	ifile.close();
	return result;
}

bool OBJReader::loadMeshFromOBJ(std::istream& _in, PolyMesh* mesh) {
	string line;
	stringstream ss, data, tmp;
	double x, y, z;
	string keyword;

	while (_in && !_in.eof()) {
		std::getline(_in, line);
		if (line.size() == 0 || line[0] == '#' || isspace(line[0])) continue;

		ss.str(line);
		ss.clear();
		ss >> keyword;
		if (keyword == "v") {
			ss >> x >> y >> z;
			mesh->addVertex(x, y, z);
		}
		else if (keyword == "f") {
			string faceline;
			std::getline(ss, faceline);

			data.str(faceline);
			data.clear();

			string vertex;
			std::vector<size_t> faceVertices;
			for (int i = 0; i < 3; i++) {
				data >> vertex;

				int v;
				auto found = vertex.find("/");
				if (found != string::npos) {
					tmp.str(vertex.substr(0, found));
				}
				else {
					tmp.str(vertex);
				}
				tmp.clear();
				tmp >> v;

				faceVertices.push_back(v - 1);
			}
			mesh->addFace(faceVertices);
		}
	}

	return true;
}

bool OBJWriter::write(const std::string& _filename, PolyMesh* mesh, std::streamsize _precision) const {
	std::fstream out(_filename.c_str(), std::ios_base::out);
	out.precision(_precision);
	bool result = write(out, mesh, _precision);

	out.close();
	return result;
}

bool OBJWriter::write(std::ostream& _out, PolyMesh* mesh, std::streamsize _precision) const {
	_out << "# " << mesh->nVertices() << "vertices, ";
	_out << mesh->nFaces() << " faces" << '\n';

	MVertex* vh;
	MPoint3 v;
	vector<MVertex*> vhandles;
	for (size_t i = 0, nV = mesh->nVertices(); i < nV; i++) {
		vh = mesh->vertex(i);
		v = vh->position();

		_out << "v " << v[0] << " " << v[1] << " " << v[2] << '\n';
	}

	for (size_t i = 0, nF = mesh->nFaces(); i < nF; i++) {
		_out << "f";

		MFace* face = mesh->face(i);
		vhandles = mesh->faceVertices(face);

		for (size_t j = 0; j < vhandles.size(); j++) {
			auto idx = vhandles[j]->index() + 1;
			_out << " " << idx;
		}

		_out << '\n';
	}

	return true;
}