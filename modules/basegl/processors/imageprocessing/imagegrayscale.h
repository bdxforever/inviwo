/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Contact: Erik Sund�n
 *
 *********************************************************************************/

#ifndef IVW_IMAGEGRAYSCALE_H
#define IVW_IMAGEGRAYSCALE_H

#include <modules/basegl/baseglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <modules/basegl/processors/imagegpuprocessor.h>
#include <inviwo/core/properties/baseoptionproperty.h>

namespace inviwo {

namespace LuminanceModels {
    enum Models {
        PerceivedLum, // Y = 0.299 R + 0.587 G + 0.114 B
        RelativeLum,  // Y = 0.2126 R + 0.7152 G + 0.0722 B
        AverageLum,   // Y = 0.3333 R + 0.3333 G + 0.3333 B
        RedOnly,      // Y = R
        GreenOnly,    // Y = G
        BlueOnly,     // Y = B
    };
}

/*! \class ImageGrayscale
 *
 * \brief Compute a gray-scale image from color input. Alpha channel is not touched.
 *
 * This processor computes the gray-scale image from a color image according to either
 * perceived luminance (Y = 0.299 R + 0.587 G + 0.114 B) or relative luminance for XYZ color
 * space (Y = 0.2126 R + 0.7152 G + 0.0722 B) utilizing the ImageGPUProcessor. 
 */
class IVW_MODULE_BASEGL_API ImageGrayscale : public ImageGPUProcessor {
public:
    ImageGrayscale();
    ~ImageGrayscale();
    InviwoProcessorInfo();

protected:
    virtual void preProcess();

private:
    OptionPropertyInt luminanceModel_;
};

} // namespace

#endif // IVW_IMAGEGRAYSCALE_H