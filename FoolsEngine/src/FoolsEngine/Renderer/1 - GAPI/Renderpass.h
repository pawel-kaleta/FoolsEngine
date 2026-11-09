#pragma once

#include "Context.h"
#include "Data.h"
#include "Resource.h"
#include "Pipeline.h"
#include "Descriptors.h"


namespace fe::Render::GAPI
{
	void	BeginRenderpassCmd(GID framebuffer);

	void	ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, const Array<F32, 4>& value);
	void	ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, const Array<S32, 4>& value);
	void	ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, const Array<U32, 4>& value);
	void	ClearDepthCmd(float value);
	void	ClearStencilCmd(U08 mask, U08 value);

	void	SetPipelineCmd(GID pipeline);
	void	SetDepthStencilStateCmd(const Descriptors::DepthStencil::Spec& depthStencil);
	void	SetBlendStateCmd(const Descriptors::Blend::Spec& blend);

	void	SetRootSourceCmd(GID source);
	void	SetIndexSourceCmd(GID source);
	void	SetDrawParamsSourceCmd(GID source);

	void	SetUniformSourceCmd(UInt bindingIndex, GID source, U32 offset, U32 size);
	void	SetBufferSourceCmd(UInt bindingIndex, GID source);

	void	DrawCmd(UInt primitiveCount, UInt indicesOffset, U32 rootOffset);
	void	DrawIndirectCmd(UInt paramsOffset, U32 rootOffset);
	void	MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount, U32 rootOffset);
	void	MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount, U32 rootOffset);

	void	EndRenderpass();
}