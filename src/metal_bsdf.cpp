#include "bsdfs.h"

AI_BSDF_EXPORT_METHODS(MetalBSDFMtd);

bsdf_init
{
    auto fs = AiBSDFGetDataPtr<MetalBSDF>(bsdf);

    static const AtBSDFLobeInfo lobe_info[] = { {AI_RAY_ALL, 0, AtString()} };

    AiBSDFInitLobes(bsdf, lobe_info, 1);
    AiBSDFInitNormal(bsdf, fs->nf, false);
}

bsdf_sample
{
    auto fs = AiBSDFGetDataPtr<MetalBSDF>(bsdf);
    fs->rng.seed(*reinterpret_cast<const uint32_t*>(&rnd.x));
    BSDFSample sample = fs->Sample(false);

    if (sample.IsInvalid())
        return AI_BSDF_LOBE_MASK_NONE;

    float cosWi = IsDeltaRay(sample.type) ? 1.f : Abs(sample.w.z);

    out_wi = AtVectorDv(ToWorld(fs->nf, sample.w));
    out_lobe_index = 0;
    out_lobes[0] = AtBSDFLobeSample(sample.f * cosWi / sample.pdf, 0.0f, sample.pdf);
    return lobe_mask;
}

bsdf_eval
{
    auto fs = AiBSDFGetDataPtr<MetalBSDF>(bsdf);
    Vec3f wiLocal = ToLocal(fs->nf, wi);
    out_lobes[0] = AtBSDFLobeSample(fs->F(wiLocal, false), 0.f, fs->PDF(wiLocal, false));
    return lobe_mask;
}

AtBSDF* AiMetalBSDF(const AtShaderGlobals* sg, const MetalBSDF& metalBSDF)
{
    AtBSDF* bsdf = AiBSDF(sg, AI_RGB_WHITE, MetalBSDFMtd, sizeof(MetalBSDF));
    auto data = AiBSDFGetDataPtr<MetalBSDF>(bsdf);
    *data = metalBSDF;
    return bsdf;
}