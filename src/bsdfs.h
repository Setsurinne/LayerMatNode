#pragma once
#include <variant>
#include <optional>
#include <vector>

#include <ai_shader_bsdf.h>
#include <ai_shaderglobals.h>

#include "common.h"
#include "random.h"

struct BSDFSample
{
    BSDFSample(AtVector w, AtRGB f, float pdf, int type, float eta = 1.f) :
        w(w), f(f), pdf(pdf), type(type), eta(eta) {}

    bool IsInvalid() const
    {
        return type == AI_RAY_UNDEFINED;
    }

    AtVector w;
    AtRGB f;
    float pdf;
    int type;
    float eta;
};

const BSDFSample InvalidSample(AtVector(), AtRGB(), 0, AI_RAY_UNDEFINED);

struct BSDFState
{
    BSDFState() = default;

    BSDFState(const AtShaderGlobals* sg)
    {
        SetDirections(sg);
    }

    void SetDirections(const AtShaderGlobals* sg)
    {
        nf = sg->Nf;
        ns = sg->Ns * AiV3Dot(sg->Ngf, sg->Ng);
        ng = sg->Ngf;
        wo = ToLocal(nf, -sg->Rd);
    }

    // front-facing mapped smooth normal
    AtVector nf;
    // front-facing smooth normal without normal map
    AtVector ns;
    // geometry normal, not smoothed
    AtVector ng;
    // outgoing direction of light transport in local coordinate
    AtVector wo;
    //
    RandomEngine rng;
};

struct FakeBSDF : BSDFState
{
    AtRGB F(const AtVector& wi, bool adjoint);
    float PDF(const AtVector& wi, bool adjoint);
    BSDFSample Sample(bool adjoint);
    bool IsDelta() const { return true; }
    bool HasTransmit() const { return true; }
};

struct LambertBSDF : BSDFState
{
    AtRGB F(const AtVector& wi, bool adjoint);
    float PDF(const AtVector& wi, bool adjoint);
    BSDFSample Sample(bool adjoint);
    bool IsDelta() const { return false; }
    bool HasTransmit() const { return false; }

    AtRGB albedo = AtRGB(.8f);
};

struct DielectricBSDF : BSDFState
{
    AtRGB F(const AtVector& wi, bool adjoint);
    float PDF(const AtVector& wi, bool adjoint);
    BSDFSample Sample(bool adjoint);
    bool IsDelta() const { return ApproxDelta(); }
    bool HasTransmit() const { return true; }
    bool ApproxDelta() const { return roughness < .01f; }

    float eta;
    float roughness;
};

struct LayeredBSDF;
using BSDF = std::variant<FakeBSDF, LambertBSDF, DielectricBSDF, LayeredBSDF>;

struct LayeredBSDF : BSDFState
{
    AtRGB F(const AtVector& wi, bool adjoint);
    float PDF(const AtVector& wi, bool adjoint);
    BSDFSample Sample(bool adjoint);
    bool IsDelta() const;
    bool HasTransmit() const;

    BSDF* top = nullptr;
    BSDF* bottom = nullptr;
    float thickness;
    float g;
    AtRGB albedo;
};

AtRGB F(BSDF& bsdf, const AtVector& wi, bool adjoint);
float PDF(BSDF& bsdf, const AtVector& wi, bool adjoint);
BSDFSample Sample(BSDF& bsdf, bool adjoint);
bool IsDelta(BSDF& bsdf);
bool HasTransmit(BSDF& bsdf);
AtVector& Nf(BSDF& bsdf);
AtVector& Ns(BSDF& bsdf);
AtVector& Ng(BSDF& bsdf);
AtVector& Wo(BSDF& bsdf);
RandomEngine& Rng(BSDF& bsdf);

float FresnelDielectric(float cosThetaI, float eta);

AtBSDF* AiLambertBSDF(const AtShaderGlobals* sg, const LambertBSDF& lambertBSDF);
AtBSDF* AiDielectricBSDF(const AtShaderGlobals* sg, const DielectricBSDF* bsdf);