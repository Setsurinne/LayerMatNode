#include "bsdfs.h"

AI_BSDF_EXPORT_METHODS(MetalBSDFMtd);

bsdf_init
{
    auto fs = AiBSDFGetDataPtr<WithState<MetalBSDF>>(bsdf);

    static const AtBSDFLobeInfo lobe_info[] = { {AI_RAY_ALL, 0, AtString()} };

    AiBSDFInitLobes(bsdf, lobe_info, 1);
    AiBSDFInitNormal(bsdf, fs->state.nf, false);
}

bsdf_sample
{
    auto fs = AiBSDFGetDataPtr<WithState<MetalBSDF>>(bsdf);
    auto& state = fs->state;

    state.rng.seed(FloatBitsToInt(rnd.x) ^ state.threadId);
    BSDFSample sample = fs->bsdf.Sample(state.wo, state.rng);

    if (sample.IsInvalid())
        return AI_BSDF_LOBE_MASK_NONE;

    float cosWi = IsDeltaRay(sample.type) ? 1.f : Abs(sample.w.z);

    out_wi = AtVectorDv(ToWorld(state.nf, sample.w));
    out_lobe_index = 0;
    out_lobes[0] = AtBSDFLobeSample(sample.f * cosWi / sample.pdf, 0.0f, sample.pdf);
    return lobe_mask;
}

bsdf_eval
{
    auto fs = AiBSDFGetDataPtr<WithState<MetalBSDF>>(bsdf);
    auto& state = fs->state;
    Vec3f wiLocal = ToLocal(state.nf, wi);

    AtRGB f = fs->bsdf.F(state.wo, wiLocal);
    float cosWi = fs->bsdf.IsDelta() ? 1.f : Abs(wiLocal.z);
    float pdf = fs->bsdf.IsDelta() ? 1.f : fs->bsdf.PDF(state.wo, wiLocal);

    out_lobes[0] = AtBSDFLobeSample(f, 0.f, pdf);
    return lobe_mask;
}

AtBSDF* AiMetalBSDF(const AtShaderGlobals* sg, const WithState<MetalBSDF>& metalBSDF)
{
    AtBSDF* bsdf = AiBSDF(sg, AI_RGB_WHITE, MetalBSDFMtd, sizeof(WithState<MetalBSDF>));
    auto data = AiBSDFGetDataPtr<WithState<MetalBSDF>>(bsdf);
    *data = metalBSDF;
    return bsdf;
}