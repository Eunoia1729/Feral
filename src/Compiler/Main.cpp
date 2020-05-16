/*
	Copyright (c) 2020, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <fstream>
#include <cstdio>
#include <cstring>

#include "Common/Config.hpp"
#include "Common/FS.hpp"
#include "Common/Env.hpp"
#include "Common/String.hpp"

#include "Compiler/Args.hpp"
#include "Compiler/LoadFile.hpp"

#include "VM/VM.hpp"

int main( int argc, char ** argv )
{
	std::unordered_map< std::string, std::string > args;
	std::vector< std::string > code_args;
	size_t flags = args::parse( argc, ( const char ** )argv, args, code_args );

	if( flags & OPT_V ) {
		fprintf( stdout, "Feral %d.%d.%d\nBuilt with %s\nOn %s\n", FERAL_VERSION_MAJOR,
			 FERAL_VERSION_MINOR, FERAL_VERSION_PATCH, BUILD_CXX_COMPILER, BUILD_DATE );
		return E_OK;
	}

	Errors err = E_OK;

	// feral binary absolute location
	std::string feral_base, feral_bin;
	feral_bin = fs::abs_path( env::get_proc_path(), & feral_base, true );

	std::string src_file = args[ "__main__" ];

	if( src_file == "testdir" ) {
		if( fs::exists( src_file + fmod_ext() ) ) {
			src_file += fmod_ext();
		} else {
			src_file = feral_base + "/include/feral/" + src_file + fmod_ext();
		}
	} else if( src_file == "install" || src_file == "build" ) {
		if( src_file == "install" ) code_args.insert( code_args.begin(), "install" );
		src_file = std::string( "build" ) + fmod_ext();
	}

	if( src_file.empty() ) {
		fprintf( stderr, "usage: %s [flags] <source file>\n", argv[ 0 ] );
		return E_FAIL;
	}

	if( src_file.find( fmod_ext() ) == std::string::npos ) {
		src_file += fmod_ext();
	}

	srcfile_t * main_src = fmod_load( src_file, flags, true, err, false );
	if( err != E_OK ) {
		return err;
	}

	int exec_err = 0;
	if( !( flags & OPT_D ) ) {
		vm_state_t vm( feral_bin, feral_base, code_args, flags );
		vm.set_fmod_load_fn( fmod_load );
		vm.set_fmod_read_code_fn( fmod_read_code );
		vm.push_src( main_src, 0 );
		if( !vm.load_core_mods() ) {
			vm.pop_src();
			err = E_EXEC_FAIL;
			return err;
		}
		exec_err = vm::exec( vm );
		vm.pop_src();
	} else {
		if( main_src ) delete main_src;
	}
	return exec_err;
}
