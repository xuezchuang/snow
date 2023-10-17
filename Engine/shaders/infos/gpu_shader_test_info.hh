/* SPDX-FileCopyrightText: 2022 Blender Foundation
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup gpu
 */

#include "gpu_interface_info.hh"
#include "gpu_shader_create_info.hh"

GPU_SHADER_CREATE_INFO(gpu_shader_Test)
	.vertex_source("gpu_shader_2D_widget_base_vert.glsl")
	.fragment_source("gpu_shader_2D_widget_base_frag.glsl")



