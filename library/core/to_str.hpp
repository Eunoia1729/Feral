/*
	Copyright (c) 2020, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef LIBRARY_CORE_TO_STR_HPP
#define LIBRARY_CORE_TO_STR_HPP

#include <iomanip>
#include <sstream>

#include "VM/VM.hpp"

var_base_t * all_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	var_base_t * _data = fd.args[ 0 ];
	char res[ 1024 ];
	sprintf( res, "type: %s at %p", vm.type_name( _data ).c_str(), _data );
	return make< var_str_t >( res );
}

var_base_t * nil_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	return make< var_str_t >( "(nil)" );
}

var_base_t * bool_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	return make< var_str_t >( BOOL( fd.args[ 0 ] )->get() ? "true" : "false" );
}

var_base_t * typeid_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	return make< var_str_t >( "typeid<" + std::to_string( TYPEID( fd.args[ 0 ] )->get() ) + ">" );
}

var_base_t * int_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	return make< var_str_t >( INT( fd.args[ 0 ] )->get().get_str() );
}

var_base_t * flt_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	std::ostringstream oss;
	oss << std::setprecision( 21 ) << FLT( fd.args[ 0 ] )->get();
	return make< var_str_t >( oss.str() );
}

var_base_t * str_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	return fd.args[ 0 ];
}

var_base_t * vec_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	var_vec_t * vec = VEC( fd.args[ 0 ] );
	std::string res = "[";
	for( auto & e : vec->get() ) {
		std::string str;
		if( !e->to_str( vm, str, fd.src_id, fd.idx ) ) {
			return nullptr;
		}
		res += str + ", ";
	}
	if( vec->get().size() > 0 ) {
		res.pop_back();
		res.pop_back();
	}
	res += "]";
	return make< var_str_t >( res );
}

var_base_t * map_to_str( vm_state_t & vm, const fn_data_t & fd )
{
	var_map_t * map = MAP( fd.args[ 0 ] );
	std::string res = "{";
	for( auto & e : map->get() ) {
		std::string str;
		if( !e.second->to_str( vm, str, fd.src_id, fd.idx ) ) {
			return nullptr;
		}
		res += e.first + ": " + str + ", ";
	}
	if( map->get().size() > 0 ) {
		res.pop_back();
		res.pop_back();
	}
	res += "}";
	return make< var_str_t >( res );
}

#endif // LIBRARY_CORE_TO_STR_HPP