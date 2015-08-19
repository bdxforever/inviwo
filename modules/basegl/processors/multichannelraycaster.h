/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2015 Inviwo Foundation
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
 *********************************************************************************/

#ifndef IVW_MULTICHANNELRAYCASTER_H
#define IVW_MULTICHANNELRAYCASTER_H

#include <modules/basegl/baseglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/properties/simplelightingproperty.h>
#include <inviwo/core/properties/simpleraycastingproperty.h>
#include <inviwo/core/properties/cameraproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/volumeindicatorproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/volumeport.h>
#include <modules/opengl/shader/shader.h>

namespace inviwo {

/** \docpage{org.inviwo.MultichannelRaycaster, Multichannel Raycaster}
 * ![](org.inviwo.MultichannelRaycaster.png?classIdentifier=org.inviwo.MultichannelRaycaster)
 *
 * ...
 *
 * ### Inports
 *   * __volume__ ...
 *   * __exit-points__ ...
 *   * __entry-points__ ...
 *
 * ### Outports
 *   * __outport__ ...
 *
 * ### Properties
 *   * __Raycasting__ ...
 *   * __transferFunctions___ ...
 *   * __Lighting__ ...
 *   * __Camera__ ...
 *
 */
class IVW_MODULE_BASEGL_API MultichannelRaycaster : public Processor {
public:
    InviwoProcessorInfo();

    MultichannelRaycaster();
    virtual ~MultichannelRaycaster();

    virtual void initializeResources() override;
    virtual void process() override;

    // override to do member renaming.
    virtual void deserialize(IvwDeserializer& d) override;
private:
    Shader shader_;

    VolumeInport volumePort_;
    ImageInport entryPort_;
    ImageInport exitPort_;
    ImageOutport outport_;

    CompositeProperty transferFunctions_;

    SimpleRaycastingProperty raycasting_;
    CameraProperty camera_;
    SimpleLightingProperty lighting_;
    VolumeIndicatorProperty positionIndicator_;
};

}  // namespace

#endif  // IVW_MULTICHANNELRAYCASTER_H
