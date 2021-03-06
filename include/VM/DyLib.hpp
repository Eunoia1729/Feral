/*
	Copyright (c) 2020, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_DYN_LIB_HPP
#define VM_DYN_LIB_HPP

#include <string>
#include <unordered_map>

/* Wrapper class for dlfcn.h library */
class dyn_lib_t
{
	std::unordered_map< std::string, void * > m_handles;
public:
	dyn_lib_t();
	~dyn_lib_t();
	void * load( const std::string & file );
	void unload( const std::string & file );
	void * get( const std::string & file, const std::string & sym );
	inline bool fexists( const std::string & file ) { return m_handles.find( file ) != m_handles.end(); }
};

#endif // VM_DYN_LIB_HPP
