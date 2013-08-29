#include "core/oxygine.h"
#include "Options.h"
#include <vector>
#include "core/files_io.h"

namespace oxygine
{
	Options Options::instance;


	struct xml_string_writer: pugi::xml_writer
	{
		std::string result;

		virtual void write(const void* data, size_t size)
		{
			result += std::string(static_cast<const char*>(data), size);
		}
	};

	Options::Options():_path("options.xml")
	{

	}

	Options::~Options()
	{

	}

	void Options::setPath(const string &path)
	{
		_path = path;
	}

	void Options::init(const string &version)
	{
		_version = version;
		load();
		if (_doc.child("options").attribute("version").value() != _version)
			reset();
	}

	void Options::reset()
	{
		_doc.reset();
		pugi::xml_node root = _doc.append_child("options");
		root.append_attribute("version").set_value(_version.c_str());
	}


	void Options::load()
	{
		_doc.reset();
		file::buffer fb;
		file::read(_path.c_str(), fb, ep_ignore_error);
		if (fb.getSize())
			_doc.load_buffer(fb.getData(), fb.getSize());
	}
	
	pugi::xml_attribute Options::addValue(const string &name)
	{
		pugi::xml_node root = _doc.child("options");
		if (!root)
			root = _doc.append_child("options");
		pugi::xml_node child = root.child(name.c_str());
		if (!child)
			child = root.append_child(name.c_str());

		pugi::xml_attribute attr = child.attribute("value");
		if (!attr)
			attr = child.append_attribute("value");
		else
			attr = pugi::xml_attribute();

		return attr;
	}

	pugi::xml_attribute Options::getValue(const string &name)
	{
		pugi::xml_node root = _doc.child("options");
		OX_ASSERT(root);
		pugi::xml_node child = root.child(name.c_str());
		OX_ASSERT(child);
		pugi::xml_attribute attr = child.attribute("value");
		OX_ASSERT(attr);

		return attr;
	}

	void Options::save()
	{
		xml_string_writer sw;
		_doc.save(sw);

		int size = sw.result.size();
		const char *buf = sw.result.c_str();

		file::handle h = file::open(_path.c_str(), "wb");
		file::write(h, buf, size);
		file::close(h);
	}
}