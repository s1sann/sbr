#include "PolyMesh.h"

#ifndef SBR_IOMANAGER_H
#define SBR_IOMANAGER_H

class OBJReader {
public:
	OBJReader() {};

	~OBJReader() {};

	bool read(const std::string& _filename, PolyMesh* mesh);

private:
	bool loadMeshFromOBJ(std::istream& _in, PolyMesh* mesh);

	std::string path_;
};

class OBJWriter {
public:
	OBJWriter() {};

	~OBJWriter() {};

	bool write(const std::string&, PolyMesh* mesh, std::streamsize _precision = 6) const;

private:
	mutable std::string path_;
	mutable std::string objName_;

	bool write(std::ostream&, PolyMesh* mesh, std::streamsize _precision = 6) const;
};

#endif // !SBR_IOMANAGER_H
