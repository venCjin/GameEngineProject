#include "pch.h"
#include "Test.h"
#include "assimp/Importer.hpp"

void Test::print()
{
	Assimp::Importer importer;
	importer.GetImporterInfo(0);
}