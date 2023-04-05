#pragma once
#include <variant>
#include <optional>
#include <vector>

#include <ai_shader_bsdf.h>
#include <ai_shaderglobals.h>

#include "common.h"
#include "random.h"

enum class BSDFType
{
    Fake,
    Lambert,
    Dielectric,
    Layered,
};

struct BSDFSample
{
    BSDFSample(AtVector w, AtRGB f, float pdf, int type, float eta = 1.f) :
        w(w), f(f), pdf(pdf), type(type), eta(eta) {}

    AtVector w;
    AtRGB f;
    float pdf;
    int type;
    float eta;
};

struct BSDF
{
    // wi in local
    virtual AtRGB F(const AtVector& wi, bool adjoint, RandomEngine* rng) = 0;
    // wi in local
    virtual float PDF(const AtVector& wi, bool adjoint, RandomEngine* rng) = 0;
    // wi and sampled direction in local
    virtual std::optional<BSDFSample> Sample(bool adjoint, RandomEngine* rng) = 0;
    virtual BSDFType Type() const = 0;
    virtual bool IsDelta() const = 0;
    virtual bool HasTransmit() const = 0;

    void SetDirections(const AtShaderGlobals* sg)
    {
        nf = sg->Nf;
        ns = sg->Ns * AiV3Dot(sg->Ngf, sg->Ng);
        ng = sg->Ngf;
        wo = ToLocal(nf, -sg->Rd);
    }

    static bool IsDeltaRay(int type)
    {
        return (type & AI_RAY_ALL_SPECULAR) != 0;
    }

    static bool IsTransmitRay(int type)
    {
        return (type & AI_RAY_ALL_TRANSMIT) != 0;
    }

    // front-facing mapped smooth normal
    AtVector nf;
    // front-facing smooth normal without normal map
    AtVector ns;
    // geometry normal, not smoothed
    AtVector ng;
    // outgoing direction of light transport in local coordinate
    AtVector wo;

    RandomEngine* mayaRng = nullptr;
};

struct FakeBSDF : public BSDF
{
    AtRGB F(const AtVector& wi, bool adjoint, RandomEngine* rng) override { return AtRGB(0.f); }

    float PDF(const AtVector& wi, bool adjoint, RandomEngine* rng) override { return 0.f; }

    std::optional<BSDFSample> Sample(bool adjoint, RandomEngine* rng) override { return BSDFSample(-wo, AtRGB(1.f), 1.f, AI_RAY_SPECULAR_TRANSMIT); }

    BSDFType Type() const override { return BSDFType::Fake; }

    bool IsDelta() const override { return true; }

    bool HasTransmit() const override { return true; }
};

struct LambertBSDF : public BSDF
{
    LambertBSDF() = default;

    LambertBSDF(const AtRGB& albedo) : albedo(albedo) {}

    AtRGB F(const AtVector& wi, bool adjoint, RandomEngine* rng) override
    {
        return albedo * AI_ONEOVERPI;
    }

    float PDF(const AtVector& wi, bool adjoint, RandomEngine* rng) override
    {
        return std::abs(wi.z) * AI_ONEOVERPI;
    }

    std::optional<BSDFSample> Sample(bool adjoint, RandomEngine* rng) override
    {
        //return std::nullopt;
        AtVector2 r = ToConcentricDisk(Sample2D(rng));
        float z = std::sqrt(1.f - AiV2Dot(r, r));
        AtVector w(r.x, r.y, z);
        return BSDFSample(w, albedo * AI_ONEOVERPI, z, AI_RAY_DIFFUSE_REFLECT);
    }

    bool IsDelta() const override { return false; }

    bool HasTransmit() const override { return true; }

    BSDFType Type() const override { return BSDFType::Lambert; }

    AtRGB albedo = AtRGB(.8f);
};

struct DielectricBSDF : public BSDF
{
    float eta;
    float roughness;
};

struct LayeredBSDF : public BSDF
{
    BSDF* top;
    BSDF* bottom;
    float thickness;
    float g;
    AtRGB albedo;
};

float FresnelDielectric(float cosThetaI, float eta);

AtBSDF* AiLambertBSDF(const AtShaderGlobals* sg, const LambertBSDF* bsdf, RandomEngine* mayaRng);
AtBSDF* AiDielectricBSDF(const AtShaderGlobals* sg, const DielectricBSDF* bsdf, RandomEngine* mayaRng);
//AtBSDF* LayeredBSDFCreate(const AtShaderGlobals* sg, const AtRGB& weight, const AtVector& N, std::vector<AtBSDF*>bsdfs);