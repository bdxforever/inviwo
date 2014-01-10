/**********************************************************************
 * Copyright (C) 2012-2013 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Erik Sund�n
 *
 **********************************************************************/

#include <inviwo/core/util/formats.h>
#include <modules/opengl/image/layergl.h>

namespace inviwo {

LayerGL::LayerGL(uvec2 dimensions, LayerType type, const DataFormatBase* format,
                 Texture2D* tex)
    : LayerRepresentation(dimensions, type, format)
    , texture_(tex){
    initialize();
}

LayerGL::~LayerGL() {
    deinitialize();
}

void LayerGL::initialize() {
    if(!texture_){
        GLFormats::GLFormat glFormat = getGLFormats()->getGLFormat(getDataFormatId());
        if(getLayerType() == DEPTH_LAYER){
            texture_ = new Texture2D(getDimension(), GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, glFormat.type, GL_NEAREST);
        }
        else
            texture_ = new Texture2D(getDimension(), glFormat, GL_LINEAR);
    }
}

void LayerGL::deinitialize() {
    if(texture_ && texture_->decreaseRefCount() <= 0){
        delete texture_;
        texture_ = NULL;
    }
}

DataRepresentation* LayerGL::clone() const {
    Texture2D* tex = NULL;
    if(texture_)
        tex = texture_->clone();
    LayerGL* newLayerGL = new LayerGL(getDimension(), getLayerType(), getDataFormat(), tex);
    return newLayerGL;
}

void LayerGL::bindTexture(GLenum texUnit) const {
    glActiveTexture(texUnit);
    texture_->bind();
}

void LayerGL::unbindTexture() const {
    texture_->unbind();
}

bool LayerGL::copyAndResizeLayer(DataRepresentation* targetLayerGL) const{
    /*const LayerGL* source = this;
    LayerGL* target = dynamic_cast<LayerGL*>(targetLayerGL);
    if(!target){
        return false;
        LogError("Target representation missing.");
    }

    const Texture2D* sTex = source->getTexture();
    Texture2D* tTex = target->getTexture();
    tTex->uploadFromPBO(sTex);
    LGL_ERROR;*/

    return false;
}

void LayerGL::resize(uvec2 dimensions) {
    dimensions_ = dimensions;
    if(texture_){
        texture_->unbind();
        texture_->resize(dimensions_);    
        texture_->unbind();
    }
}

Texture2D* LayerGL::getTexture() {
    return texture_;
}

const Texture2D* LayerGL::getTexture() const {
    return texture_;
}

} // namespace