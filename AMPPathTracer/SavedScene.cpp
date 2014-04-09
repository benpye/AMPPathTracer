#include "SavedScene.h"

#include <fstream>

SavedScene SavedScene::LoadFromFile(std::string filename)
{
	std::stringstream buf;

	std::ifstream fileRead(filename);
	buf << fileRead.rdbuf();
	fileRead.close();

	SavedScene save;
	jsoncpp::parse(save, buf.str());

	return save;
}