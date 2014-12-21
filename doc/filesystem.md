#File System

##Typical usage example 
Read file, modify it and save back:
	
	#include "file.h" 

	void test()
	{
		oxygine::file::buffer data;
	
		//read entire file into buffer with stdio flags "rb"
		oxygine::file::read("user", data);
	
		//modify buffer
		data[0] += 1
		data[1] += 2
		data.push_back(3);
	
		//write file back from buffer with stdio flags "wb"
		oxygine::file::write("user", data);
	}
	

**oxygine::file** is intermediate buffer for storing data when working with files. It uses std::vector with unsigned chars internally.

 
##Low level
Namespace **oxygine::file** has also low level functions to working with files.
	

	oxygine::file::handle h = oxygine::file::open("user", "wb");
	oxygine::file::write(h, "Hello World", strlen("Hello World"));	
	oxygine::file::close(h);

> **Oxygine supports only binary file operations, you allways should use 'b' flag when opening file**

##Working with missing files

Imagine you want to read file and work with loaded data. But you not sure that file is exits:

	file::read("user", data);
	//user function
	load_user(data);

If file "user" is missing you would see error:

	>> error: can't open file: user
	>> error: Assert! 941 d:\oxygine-framework\oxygine\src\core\oxygine.cpp
	>> Assertion failed!

To avoid error you should pass additional argument:

	file::read("user", data, ep_ignore_error);
	if (!data.empty())
		load_user(data);

This code will show warning if file is missing:

	file::read("user", data, ep_show_warning);
	if (!data.empty())
		load_user(data);

	>> warning: can't open file: user
	

In examples above third argument is **error_policy** enum. It has 3 values:

* ep_show_error. It is default and most strict value. Shows assert and prints error to log.
* ep_show_warning. Prints warning to log.
* ep_ignore_error. Does nothing.



Or you could check it with **oxygine::file::exists** function. It is slower than solutions above:

	if (oxygine::file::exists("user"))
	{
		file::read("user", data);
		//user function
		load_user(data);
	}



##Mounting zip archieves to filesytem

First step is mount ZipFileSystem. More than one archive could be mounted at once:

	#include "core/ZipFileSystem.h"

	oxygine::file::ZipFileSystem zp;

	void mount()
	{		
		zp.add("pack1.zip")
		zp.add("pack2.zip")
		zp.add("pack3.zip")
	
		oxygine::file::mount(&zp);		
	}

Read any data from file as usual:

	void read_packs()
	{
		oxygine::file::buffer data;
		oxygine::file::read("filename", data);
	}


> ZipFileSystem is read only.
