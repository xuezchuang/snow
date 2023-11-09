
#include "GPU_init_exit.h"
//#include "BLI_sys_types.h"
//#include "GPU_batch.h"

//#include "gpu_codegen.h"
//#include <iostream>
#include <stdbool.h> 
//#include <stdint.h>


#include "utildefines.h"
#include <string>

//#include <algorithm>
//#include <iomanip>
//#include <iostream>
//#include <regex>
//#include <sstream>
//#include <stdarg.h>

#include "gpu_shader_create_info_private.hh"
#include "gpu_shader_dependency_private.h"

//static CreateInfoDictionnary *g_create_infos = nullptr;

/**
* although the order of initialization and shutdown should not matter
* (except for the extensions), I chose alphabetical and reverse alphabetical order
*/
static bool initialized = false;

void GPU_init(void)
{
	/* can't avoid calling this multiple times, see wm_window_ghostwindow_add */
	if(initialized)
	{
		return;
	}

	initialized = true;

	gpu_shader_dependency_init();
	gpu_shader_create_info_init();

	//gpu_codegen_init();

	//gpu_batch_init();
}

void GPU_exit(void)
{
	//gpu_batch_exit();

	//gpu_codegen_exit();

	//gpu_shader_dependency_exit();
	//gpu_shader_create_info_exit();

	//gpu_backend_delete_resources();
	initialized = false;
}

bool GPU_is_init(void)
{
	return initialized;
}
