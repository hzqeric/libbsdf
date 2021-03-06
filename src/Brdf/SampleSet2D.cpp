// =================================================================== //
// Copyright (C) 2014-2019 Kimura Ryo                                  //
//                                                                     //
// This Source Code Form is subject to the terms of the Mozilla Public //
// License, v. 2.0. If a copy of the MPL was not distributed with this //
// file, You can obtain one at http://mozilla.org/MPL/2.0/.            //
// =================================================================== //

#include <libbsdf/Brdf/SampleSet2D.h>

#include <libbsdf/Brdf/Sampler.h>

using namespace lb;

SampleSet2D::SampleSet2D(int        numTheta,
                         int        numPhi,
                         ColorModel colorModel,
                         int        numWavelengths,
                         bool       equalIntervalAngles)
                         : equalIntervalTheta_(false),
                           equalIntervalPhi_(false),
                           sourceType_(UNKNOWN_SOURCE)
{
    lbTrace << "[SampleSet2D::SampleSet2D]";

    assert(numTheta > 0 && numPhi > 0);

    colorModel_ = colorModel;

    size_t numSamples = numTheta * numPhi;
    spectra_.resize(numSamples);

    if (equalIntervalAngles) {
        thetaAngles_ = Arrayf::LinSpaced(numTheta, 0.0, SphericalCoordinateSystem::MAX_ANGLE0);
        phiAngles_   = Arrayf::LinSpaced(numPhi,   0.0, SphericalCoordinateSystem::MAX_ANGLE1);
        updateAngleAttributes();
    }
    else {
        thetaAngles_.resize(numTheta);
        phiAngles_.resize(numPhi);
    }

    if (colorModel == MONOCHROMATIC_MODEL) {
        numWavelengths = 1;
    }
    else if (colorModel != SPECTRAL_MODEL) {
        numWavelengths = 3;
    }

    assert(numWavelengths > 0);

    for (size_t i = 0; i < numSamples; ++i) {
        Spectrum spectrum;
        spectrum.resize(numWavelengths);
        spectra_.at(i) = spectrum;
    }

    wavelengths_.resize(numWavelengths);

    if (colorModel == MONOCHROMATIC_MODEL ||
        colorModel != SPECTRAL_MODEL) {
        wavelengths_ = Arrayf::Zero(numWavelengths);
    }
}

SampleSet2D::~SampleSet2D()
{
    lbTrace << "[SampleSet2D::~SampleSet2D]";
}

Spectrum SampleSet2D::getSpectrum(const Vec3& inDir) const
{
    Spectrum sp;
    Sampler::getSpectrum<LinearInterpolator>(*this, inDir, &sp);
    return sp;
}

void SampleSet2D::updateAngleAttributes()
{
    equalIntervalTheta_ = isEqualInterval(thetaAngles_);
    equalIntervalPhi_   = isEqualInterval(phiAngles_);

    lbInfo
        << "[SampleSet2D::updateAngleAttributes] equalIntervalTheta_: "
        << equalIntervalTheta_;
    lbInfo
        << "[SampleSet2D::updateAngleAttributes] equalIntervalPhi_: "
        << equalIntervalPhi_;
}

void SampleSet2D::resizeAngles(int numTheta, int numPhi)
{
    assert(numTheta > 0 && numPhi > 0);

    size_t numSamples = numTheta * numPhi;
    spectra_.resize(numSamples);

    thetaAngles_.resize(numTheta);
    phiAngles_.resize(numPhi);
}

void SampleSet2D::resizeWavelengths(int numWavelengths)
{
    assert(numWavelengths > 0);

    size_t numSamples = thetaAngles_.size() * phiAngles_.size();

    for (size_t i = 0; i < numSamples; ++i) {
        Spectrum sp;
        sp.resize(numWavelengths);
        spectra_.at(i) = sp;
    }

    wavelengths_.resize(numWavelengths);
}

void SampleSet2D::clampAngles()
{
    thetaAngles_ = thetaAngles_.cwiseMax(0.0);
    phiAngles_   = phiAngles_.cwiseMax(0.0);

    thetaAngles_ = thetaAngles_.cwiseMin(SphericalCoordinateSystem::MAX_ANGLE0);
    phiAngles_   = phiAngles_.cwiseMin(SphericalCoordinateSystem::MAX_ANGLE1);
}
