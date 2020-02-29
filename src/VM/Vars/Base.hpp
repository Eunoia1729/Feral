/*
	Copyright (c) 2020, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_VARS_BASE_HPP
#define VM_VARS_BASE_HPP

#include <cassert>
#include <vector>
#include <string>
#include <unordered_map>
#include <gmpxx.h>

#include "../../../third_party/mpfrxx.hpp"
#include "../SrcFile.hpp"

enum VarTypes
{
	VT_ALL,

	VT_NIL,

	VT_BOOL,
	VT_INT,
	VT_FLT,
	VT_STR,

	VT_FUNC,

	VT_MOD,

	// all custom types have typeid >= VT_STRUCT
	VT_STRUCT,

	_VT_LAST,
};

class var_base_t
{
	size_t m_type;
	size_t m_idx;
	size_t m_ref;
public:
	var_base_t( const size_t & type, const size_t & idx, const size_t & ref );
	virtual ~var_base_t();

	virtual var_base_t * copy( const size_t & idx ) = 0;
	virtual void set( var_base_t * from ) = 0;

	inline size_t type() const { return m_type; }

	inline size_t idx() const { return m_idx; }

	inline void iref() { ++m_ref; }
	inline size_t dref() { assert( m_ref > 0 ); --m_ref; return m_ref; }
	inline size_t ref() const { return m_ref; }

	static void * operator new( size_t sz );
	static void operator delete( void * ptr, size_t sz );
};

template< typename T > inline void var_iref( T * & var )
{
	if( var == nullptr ) return;
	var->iref();
}
template< typename T > inline void var_dref( T * & var )
{
	if( var == nullptr ) return;
	var->dref();
	if( var->ref() == 0 ) {
		delete var;
		var = nullptr;
	}
}

class var_nil_t : public var_base_t
{
public:
	var_nil_t( const size_t & idx );

	var_base_t * copy( const size_t & idx );
	void set( var_base_t * from );
};
#define NIL( x ) static_cast< var_nil_t * >( x )

class var_bool_t : public var_base_t
{
	bool m_val;
public:
	var_bool_t( const bool val, const size_t & idx );

	var_base_t * copy( const size_t & idx );
	bool & get();
	void set( var_base_t * from );
};
#define BOOL( x ) static_cast< var_bool_t * >( x )

class var_int_t : public var_base_t
{
	mpz_class m_val;
public:
	var_int_t( const mpz_class & val, const size_t & idx );

	var_base_t * copy( const size_t & idx );
	mpz_class & get();
	void set( var_base_t * from );
};
#define INT( x ) static_cast< var_int_t * >( x )

class var_flt_t : public var_base_t
{
	mpfr::mpreal m_val;
public:
	var_flt_t( const mpfr::mpreal & val, const size_t & idx );

	var_base_t * copy( const size_t & idx );
	mpfr::mpreal & get();
	void set( var_base_t * from );
};
#define FLT( x ) static_cast< var_flt_t * >( x )

class var_str_t : public var_base_t
{
	std::string m_val;
public:
	var_str_t( const std::string & val, const size_t & idx );

	var_base_t * copy( const size_t & idx );
	std::string & get();
	void set( var_base_t * from );
};
#define STR( x ) static_cast< var_str_t * >( x )

struct fn_assn_arg_t
{
	std::string name;
	var_base_t * val;
};

struct fn_body_span_t
{
	size_t begin;
	size_t end;
};

struct fn_data_t
{
	size_t idx;
	std::vector< var_base_t * > args;
	std::vector< fn_assn_arg_t > assn_args;
};
struct vm_state_t;
typedef var_base_t * ( * nativefnptr_t )( vm_state_t & vm, const fn_data_t & fd );

union fn_body_t
{
	nativefnptr_t native;
	fn_body_span_t feral;
};

class var_fn_t : public var_base_t
{
	size_t m_fn_id;
	size_t m_src_id;
	std::string m_kw_arg;
	std::string m_var_arg;
	std::vector< std::string > m_args;
	std::vector< fn_assn_arg_t > m_def_args;
	fn_body_t m_body;
	bool m_is_native;
public:
	var_fn_t( const size_t & src_id, const std::string & kw_arg,
		  const std::string & var_arg, const std::vector< std::string > & args,
		  const std::vector< fn_assn_arg_t > & def_args,
		  const fn_body_t & body, const bool is_native, const size_t & idx );
	var_fn_t( const size_t & src_id, const std::vector< std::string > & args,
		  const fn_body_t & body, const size_t & idx );

	~var_fn_t();

	var_base_t * copy( const size_t & idx );

	size_t fn_id();
	size_t src_id();
	std::string & kw_arg();
	std::string & var_arg();
	std::vector< std::string > & args();
	std::vector< fn_assn_arg_t > & def_args();
	fn_body_t & body();
	bool is_native();

	bool call( vm_state_t & vm, const std::vector< var_base_t * > & args,
		   const std::vector< fn_assn_arg_t > & assn_args,
		   const size_t & idx );

	void set( var_base_t * from );
};
#define FN( x ) static_cast< var_fn_t * >( x )

class srcfile_vars_t;
class var_module_t : public var_base_t
{
	srcfile_t * m_src;
	srcfile_vars_t * m_vars;
	bool m_copied;
public:
	var_module_t( srcfile_t * src, srcfile_vars_t * vars, const size_t & idx );
	~var_module_t();

	var_base_t * copy( const size_t & idx );

	srcfile_t * src();
	srcfile_vars_t * vars();
	bool copied();

	void set( var_base_t * from );
};
#define MOD( x ) static_cast< var_module_t * >( x )

/* only used as base class for structures (not instantiated by itself) */
class var_struct_t : public var_base_t
{
	size_t m_id;
	std::vector< size_t > m_inherits;
	// TODO: should this be an unordered_map???
	std::unordered_map< std::string, var_base_t * > m_attrs;
public:
	var_struct_t( const size_t & id, const size_t & idx );
	var_struct_t( const size_t & idx );
	~var_struct_t();

	var_base_t * copy( const size_t & idx );

	size_t id();
	bool inherits( const size_t & id );
	void inherit( const size_t & id );
	bool add_attr( const std::string & name, var_base_t * val, const bool iref );
	var_base_t * get_attr( const std::string & name );
	bool has_attr( const std::string & name );
	const std::vector< size_t > & inherit_chain() const;
	const std::unordered_map< std::string, var_base_t * > & attrs() const;

	void set( var_base_t * from );
};
#define STRUCT( x ) static_cast< var_struct_t * >( x )

void init_builtin_types( vm_state_t & vm );

#endif // VM_VARS_BASE_HPP