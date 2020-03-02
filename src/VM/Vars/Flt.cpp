/*
	Copyright (c) 2020, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Base.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// SOME EXTRAS ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map< std::string, var_base_t * > * var_flt_base()
{
	static std::unordered_map< std::string, var_base_t * > v;
	return & v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// VAR_FLT //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var_flt_t::var_flt_t( const mpfr::mpreal & val, const size_t & idx )
	: var_base_t( VT_FLT, idx, 1 ), m_val( val ) { fuse( VT_FLT, var_flt_base() ); }

var_base_t * var_flt_t::copy( const size_t & idx ) { return new var_flt_t( m_val, idx ); }
mpfr::mpreal & var_flt_t::get() { return m_val; }
void var_flt_t::set( var_base_t * from )
{
	m_val = FLT( from )->get();
}