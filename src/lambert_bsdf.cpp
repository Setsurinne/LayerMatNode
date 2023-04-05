﻿#include "bsdfs.h"
#include "ai_shader_bsdf.h"

AI_BSDF_EXPORT_METHODS(LambertBSDFMtd);

bsdf_init
{
    auto fs = AiBSDFGetDataPtr<LambertBSDF>(bsdf);

    static const AtBSDFLobeInfo lobe_info[1] = {
            {AI_RAY_DIFFUSE_REFLECT, 0, AtString()}};

    AiBSDFInitLobes(bsdf, lobe_info, 1);
    AiBSDFInitNormal(bsdf, fs->nf, true);
}

RandomEngine rng;

bsdf_sample
{
    auto fs = AiBSDFGetDataPtr<LambertBSDF>(bsdf);
    fs->rng.seed(*reinterpret_cast<const uint32_t*>(&rnd.x));

    BSDFSample sample = fs->Sample(false);

    if (!sample.IsInvalid())
        return AI_BSDF_LOBE_MASK_NONE;

    if (sample.w.z < 0)
        return AI_BSDF_LOBE_MASK_NONE;

    out_wi = AtVectorDv(ToWorld(fs->nf, sample.w));
    out_lobe_index = 0;
    out_lobes[0] = AtBSDFLobeSample(sample.f, 0.0f, sample.pdf);
    return lobe_mask;
}

bsdf_eval
{
    auto fs = AiBSDFGetDataPtr<LambertBSDF>(bsdf);
    
    AtVector wiLocal = ToLocal(fs->nf, wi);

    if (wiLocal.z < 0)
        return AI_BSDF_LOBE_MASK_NONE;

    out_lobes[0] = AtBSDFLobeSample(fs->albedo * AI_ONEOVERPI, 0.f, fs->PDF(wiLocal, false));
    return lobe_mask;
}

AtBSDF* AiLambertBSDF(const AtShaderGlobals* sg, const LambertBSDF& lambertBSDF)
{
    AtBSDF* bsdf = AiBSDF(sg, AI_RGB_WHITE, LambertBSDFMtd, sizeof(LambertBSDF));
    auto data = AiBSDFGetDataPtr<LambertBSDF>(bsdf);
    *data = lambertBSDF;
    return bsdf;
}