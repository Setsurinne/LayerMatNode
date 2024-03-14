#include "bsdfs.h"

AI_SHADER_NODE_EXPORT_METHODS(LambertNodeMtd);

enum LambertNodeParams
{
	p_albedoNode = 1,
	p_albedo,
	p_stemNode,
	p_scale,
	p_offset,
	p_orientation,
	p_knotNode,
	p_density,
	p_lifespan
};

node_parameters
{
	AiParameterStr(NodeParamTypeName, LambertNodeName);
	AiParameterNode("albedoSample", nullptr)
	AiParameterRGB("albedo", .8f, .8f, .8f);

	AiParameterNode("stemSample", nullptr)
	AiParameterVec("Scale", 0.f, 0.f, 0.f);
	AiParameterVec("Offset", 0.f, 0.f, 0.f);
	AiParameterVec("Orientation", 0.f, 0.f, 0.f);

	AiParameterNode("knotSample", nullptr)
	AiParameterFlt("Density", 0.3f);
	AiParameterFlt("Lifespan", 0.3f);
}

node_initialize
{
	auto bsdf = new BSDF;
	*bsdf = LambertBSDF();
	AiNodeSetLocalData(node, bsdf);
}

node_update
{
	LambertBSDF lambertBSDF;
	lambertBSDF.albedo = AiNodeGetRGB(node, "albedo");
	GetNodeLocalDataRef<BSDF>(node) = lambertBSDF;
}

node_finish
{
	//delete GetNodeLocalDataPtr<BSDF>(node);
}

shader_evaluate
{
	LambertBSDF lambertBSDF;
	lambertBSDF.albedo = AiShaderEvalParamRGB(p_albedo);
	GetNodeLocalDataRef<BSDF>(node) = lambertBSDF;

	if (sg->Rt & AI_RAY_SHADOW)
		return;
	sg->out.CLOSURE() = AiLambertBSDF(sg, { lambertBSDF, BSDFState() });
}