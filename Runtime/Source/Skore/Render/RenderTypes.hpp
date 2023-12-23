// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/Core/String.hpp"
#include "Skore/Math/MathTypes.hpp"
#include "Skore/Core/Span.hpp"

#define RENDER_HANDLER(StructName) struct StructName { \
CPtr handler;                                      \
 operator bool() const {return handler != nullptr; }          \
 bool operator==(const StructName& b) const { return this->handler == b.handler; } \
 bool operator!=(const StructName& b) const { return this->handler != b.handler; } \
}

namespace Skore
{
	RENDER_HANDLER(RenderSwapchain);
	RENDER_HANDLER(RenderPass);
	RENDER_HANDLER(RenderCommands);
	RENDER_HANDLER(RenderPipelineState);


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
		RenderPass        renderPass{};
		RenderSwapchain   swapchain{};
		String            shader{};
		bool              depthWrite        = false;
		bool              stencilTest       = false;
		bool              blendEnabled      = false;
		f32               minDepthBounds    = 1.0;
		f32               maxDepthBounds    = 0.0;
		CullMode          cullMode          = CullMode_None;
		CompareOp         compareOperator   = CompareOp_Always;
		PolygonMode       polygonMode       = PolygonMode_Fill;
		PrimitiveTopology primitiveTopology = PrimitiveTopology_TriangleList;
	};

	struct ViewportInfo
	{
		f32 x{};
		f32 y{};
		f32 width{};
		f32 height{};
		f32 minDepth{};
		f32 maxDepth{};
	};
}