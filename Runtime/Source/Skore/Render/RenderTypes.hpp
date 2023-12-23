// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/Core/String.hpp"
#include "Skore/Math/MathTypes.hpp"
#include "Skore/Core/Span.hpp"

namespace Skore
{
	SK_HANDLER(RenderSwapchain);
	SK_HANDLER(RenderPass);
	SK_HANDLER(RenderCommands);
	SK_HANDLER(RenderPipelineState);


	enum RenderDeviceType_
	{
		RenderDeviceType_None   = 0,
		RenderDeviceType_Vulkan = 1,
		RenderDeviceType_OpenGL = 2,
		RenderDeviceType_D3D12  = 2,
		RenderDeviceType_Metal  = 3
	};

	enum CompareOp_
	{
		CompareOp_Never          = 0,
		CompareOp_Less           = 1,
		CompareOp_Equal          = 2,
		CompareOp_LessOrEqual    = 3,
		CompareOp_Greater        = 4,
		CompareOp_NotEqual       = 5,
		CompareOp_GreaterOrEqual = 6,
		CompareOp_Always         = 7
	};

	enum CullMode_
	{
		CullMode_None  = 0,
		CullMode_Front = 1,
		CullMode_Back  = 2
	};

	enum PolygonMode_
	{
		PolygonMode_Fill  = 0,
		PolygonMode_Line  = 1,
		PolygonMode_Point = 2,
	};

	enum PrimitiveTopology_
	{
		PrimitiveTopology_PointList                  = 0,
		PrimitiveTopology_LineList                   = 1,
		PrimitiveTopology_LineStrip                  = 2,
		PrimitiveTopology_TriangleList               = 3,
		PrimitiveTopology_TriangleStrip              = 4,
		PrimitiveTopology_TriangleFan                = 5,
		PrimitiveTopology_LineListWithAdjacency      = 6,
		PrimitiveTopology_LineStripWithAdjacency     = 7,
		PrimitiveTopology_TriangleListWithAdjacency  = 8,
		PrimitiveTopology_TriangleStripWithAdjacency = 9,
		PrimitiveTopology_PatchList                  = 10,
	};

	enum ShaderBinTarget_
	{
		ShaderBinTarget_None  = 0,
		ShaderBinTarget_SPIRV = 1,
		ShaderBinTarget_DXIL  = 2
	};

	typedef u32 RenderDeviceType;
	typedef u32 CullMode;
	typedef u32 PolygonMode;
	typedef u32 PrimitiveTopology;
	typedef u32 CompareOp;
	typedef u32 ShaderBinTarget;

	struct BeginRenderPassInfo
	{
		RenderPass      RenderPass{};
		RenderSwapchain Swapchain{};
		Span<Vec4>      ClearValues{};
		Vec2            DepthStencil{1.0, 0.0};
	};

	struct GraphicsPipelineCreation
	{
		RenderPass        RenderPass{};
		RenderSwapchain   Swapchain{};
		String            Shader{};
		bool              DepthWrite        = false;
		bool              StencilTest       = false;
		bool              BlendEnabled      = false;
		f32               MinDepthBounds    = 1.0;
		f32               MaxDepthBounds    = 0.0;
		CullMode          CullMode          = CullMode_None;
		CompareOp         CompareOperator   = CompareOp_Always;
		PolygonMode       PolygonMode       = PolygonMode_Fill;
		PrimitiveTopology PrimitiveTopology = PrimitiveTopology_TriangleList;
	};

	struct ViewportInfo
	{
		f32 X{};
		f32 Y{};
		f32 Width{};
		f32 Height{};
		f32 MinDepth{};
		f32 MaxDepth{};
	};
}