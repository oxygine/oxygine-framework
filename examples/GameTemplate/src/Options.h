#pragma once
#include <string>
#include "pugixml/pugixml.hpp"

namespace oxygine
{
	using namespace std;

	class Options
	{
	public:
		static Options instance;

		Options();
		~Options();

		void init(const string &version = "1");
		void reset();
		void load();
		void save();


		void setPath(const string &path);

		pugi::xml_node getRoot() const {return _doc.root();}

		pugi::xml_attribute addValue(const string &name);
		pugi::xml_attribute getValue(const string &name);


	private:
		string _version;
		string _path;
		pugi::xml_document _doc;
	};
}
