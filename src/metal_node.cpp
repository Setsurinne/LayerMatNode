#include "bsdfs.h"

AI_SHADER_NODE_EXPORT_METHODS(MetalNodeMtd);

enum MetalNodeParams
{
	p_albedo = 1,
	p_ior,
	p_k,
	p_roughness,
};

node_parameters
{
	AiParameterStr(NodeParamTypeName, MetalNodeName);
	AiParameterRGB("albedo", .8f, .8f, .8f);
	AiParameterFlt("ior", 1.5f);
	AiParameterFlt("k", .1f);
	AiParameterFlt("roughness", .2f);
}

node_initialize
{
	BSDF* bsdf = new BSDF;
	AiNodeSetLocalData(node, bsdf);
}

node_update
{
}

node_finish
{
	//delete GetNodeLocalData<BSDF>(node);
}

shader_evaluate
{
	MetalBSDF metalBSDF;
	metalBSDF.SetDirectionsAndRng(sg, false);
	metalBSDF.albedo = AiShaderEvalParamRGB(p_albedo);
	metalBSDF.ior = AiShaderEvalParamFlt(p_ior);
	metalBSDF.k = AiShaderEvalParamFlt(p_k);
	metalBSDF.alpha = AiSqr(AiShaderEvalParamFlt(p_roughness));

	auto bsdf = GetNodeLocalData<BSDF>(node);
	*bsdf = metalBSDF;

	if (sg->Rt & AI_RAY_SHADOW)
		return;
	sg->out.CLOSURE() = AiMetalBSDF(sg, metalBSDF);
}