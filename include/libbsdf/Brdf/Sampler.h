// =================================================================== //
// Copyright (C) 2014-2016 Kimura Ryo                                  //
//                                                                     //
// This Source Code Form is subject to the terms of the Mozilla Public //
// License, v. 2.0. If a copy of the MPL was not distributed with this //
// file, You can obtain one at http://mozilla.org/MPL/2.0/.            //
// =================================================================== //

#ifndef LIBBSDF_SAMPLER_H
#define LIBBSDF_SAMPLER_H

#include <cassert>

#include <libbsdf/Brdf/Brdf.h>
#include <libbsdf/Brdf/SampleSet.h>
#include <libbsdf/Brdf/SampleSet2D.h>
#include <libbsdf/Common/Global.h>
#include <libbsdf/Common/SphericalCoordinateSystem.h>

namespace lb {

/*!
 * \class   Sampler
 * \brief   The Sampler class provides sampling functions using interpolaton and extraplation.
 */
class Sampler
{
public:
    /*! Gets the interpolated spectrum of sample points at incoming and outgoing directions. */
    template <typename CoordSysT, typename InterpolatorT>
    static void getSpectrum(const SampleSet&    samples,
                            const Vec3&         inDir,
                            const Vec3&         outDir,
                            Spectrum*           spectrum);

    /*!
     * Gets the interpolated value of sample points at incoming and outgoing directions
     * and the index of wavelength.
     */
    template <typename CoordSysT, typename InterpolatorT>
    static float getValue(const SampleSet&  samples,
                          const Vec3&       inDir,
                          const Vec3&       outDir,
                          int               wavelengthIndex);

    /*! Gets the interpolated spectrum of sample points at incoming and outgoing directions. */
    template <typename InterpolatorT>
    static void getSpectrum(const Brdf& brdf,
                            const Vec3& inDir,
                            const Vec3& outDir,
                            Spectrum*   spectrum);

    /*!
     * Gets the interpolated value of sample points at incoming and outgoing directions
     * and the index of wavelength.
     */
    template <typename InterpolatorT>
    static float getValue(const Brdf&   brdf,
                          const Vec3&   inDir,
                          const Vec3&   outDir,
                          int           wavelengthIndex);

    /*! Gets the interpolated spectrum of sample points at an incoming direction. */
    template <typename InterpolatorT>
    static void getSpectrum(const SampleSet2D&  ss2,
                            const Vec3&         inDir,
                            Spectrum*           spectrum);
};

template <typename CoordSysT, typename InterpolatorT>
inline void Sampler::getSpectrum(const SampleSet&   samples,
                                 const Vec3&        inDir,
                                 const Vec3&        outDir,
                                 Spectrum*          spectrum)
{
    assert(inDir.z() >= 0.0);

    float angle0, angle1, angle2, angle3;
    if (samples.isIsotropic()) {
        CoordSysT::fromXyz(inDir, outDir, &angle0, &angle2, &angle3);
        InterpolatorT::getSpectrum(samples, angle0, angle2, angle3, spectrum);
    }
    else {
        CoordSysT::fromXyz(inDir, outDir, &angle0, &angle1, &angle2, &angle3);
        InterpolatorT::getSpectrum(samples, angle0, angle1, angle2, angle3, spectrum);
    }
}

template <typename CoordSysT, typename InterpolatorT>
inline float Sampler::getValue(const SampleSet& samples,
                               const Vec3&      inDir,
                               const Vec3&      outDir,
                               int              wavelengthIndex)
{
    assert(inDir.z() >= 0.0);

    float angle0, angle1, angle2, angle3;
    if (samples.isIsotropic()) {
        CoordSysT::fromXyz(inDir, outDir, &angle0, &angle2, &angle3);
        return InterpolatorT::getValue(samples, angle0, angle2, angle3, wavelengthIndex);
    }
    else {
        CoordSysT::fromXyz(inDir, outDir, &angle0, &angle1, &angle2, &angle3);
        return InterpolatorT::getValue(samples, angle0, angle1, angle2, angle3, wavelengthIndex);
    }
}

template <typename InterpolatorT>
inline void Sampler::getSpectrum(const Brdf&    brdf,
                                 const Vec3&    inDir,
                                 const Vec3&    outDir,
                                 Spectrum*      spectrum)
{
    assert(inDir.z() >= 0.0);

    const SampleSet* ss = brdf.getSampleSet();

    float angle0, angle1, angle2, angle3;
    if (ss->isIsotropic()) {
        brdf.fromXyz(inDir, outDir, &angle0, &angle2, &angle3);
        InterpolatorT::getSpectrum(*ss, angle0, angle2, angle3, spectrum);
    }
    else {
        brdf.fromXyz(inDir, outDir, &angle0, &angle1, &angle2, &angle3);
        InterpolatorT::getSpectrum(*ss, angle0, angle1, angle2, angle3, spectrum);
    }
}

template <typename InterpolatorT>
inline float Sampler::getValue(const Brdf&  brdf,
                               const Vec3&  inDir,
                               const Vec3&  outDir,
                               int          wavelengthIndex)
{
    assert(inDir.z() >= 0.0);

    const SampleSet* ss = brdf.getSampleSet();

    float angle0, angle1, angle2, angle3;
    if (ss->isIsotropic()) {
        brdf.fromXyz(inDir, outDir, &angle0, &angle2, &angle3);
        return InterpolatorT::getValue(*ss, angle0, angle2, angle3, wavelengthIndex);
    }
    else {
        brdf.fromXyz(inDir, outDir, &angle0, &angle1, &angle2, &angle3);
        return InterpolatorT::getValue(*ss, angle0, angle1, angle2, angle3, wavelengthIndex);
    }
}

template <typename InterpolatorT>
inline void Sampler::getSpectrum(const SampleSet2D& ss2,
                                 const Vec3&        inDir,
                                 Spectrum*          spectrum)
{
    float inTheta, inPhi;
    if (ss2.isIsotropic()) {
        inTheta = std::acos(inDir[2]);
        InterpolatorT::getSpectrum(ss2, inTheta, spectrum);
    }
    else {
        SphericalCoordinateSystem::fromXyz(inDir, &inTheta, &inPhi);
        InterpolatorT::getSpectrum(ss2, inTheta, inPhi, spectrum);
    }
}

} // namespace lb

#endif // LIBBSDF_SAMPLER_H
