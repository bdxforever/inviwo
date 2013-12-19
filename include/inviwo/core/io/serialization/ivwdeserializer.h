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
 * Primary author : Sathish Kottravel
 *
 **********************************************************************/

#ifndef IVW_DESERIALIZER_H
#define IVW_DESERIALIZER_H

#pragma warning(disable: 4290)

#include <inviwo/core/io/serialization/ivwserializebase.h>
#include <inviwo/core/util/exception.h>

namespace inviwo {

class IvwSerializable;

class IVW_CORE_API IvwDeserializer : public  IvwSerializeBase {
public:
    /** 
     * \brief Deserializer constructor
     * 
     * @param IvwDeserializer & s optional reference to existing deserializer.
     * @param bool allowReference flag to manage references to avoid multiple object creation.     
     */
    IvwDeserializer(IvwDeserializer &s, bool allowReference=true);
    /** 
     * \brief Deserializer constructor
     *     
     * @param std::string fileName path to file that is to be deserialized.
     * @param bool allowReference flag to manage references to avoid multiple object creation.     
     */
    IvwDeserializer(std::string fileName, bool allowReference=true);

    virtual ~IvwDeserializer();

    virtual void readFile();

    // std containers
    /** 
     * \brief Deserialize a vector
     *
     * Deserialize the vector that has pre-allocated objects of type T or allocated by deserializer.
     * A vector is identified by key and vector items are identified by itemKey
     *
     * eg. xml tree with key=Properties and itemKey=Property
     *
     * <Properties>
     *      <Property identifier="enableMIP" displayName="MIP">
     *          <value content="0" />
     *      </Property>
     *      <Property identifier="enableShading" displayName="Shading">
     *          <value content="0" />
     *      </Property>
     * <Properties>
     * 
     * @param const std::string & key vector key.
     * @param std::vector<T> & sVector vector to be deserialized.
     * @param const std::string & itemKey vector item key     
     */
    template <typename T>
    void deserialize(const std::string &key, 
                     std::vector<T> &sVector, 
                     const std::string &itemKey);
    /** 
     * \brief  Deserialize a map
     *
     * Deserialize a map, which can have 
     * keys of type K, 
     * values of type V* (pointers) 
     * and compare function C ( optional if 
     * K primitive type, i.e., std::string, int, etc.,)
     * eg., std::map<std::string, Property*>
     *
     * eg. xml tree
     *
     * <Properties>
     *      <Property identifier="enableMIP" displayName="MIP">
     *          <value content="0" />
     *      </Property>
     *      <Property identifier="enableShading" displayName="Shading">
     *          <value content="0" />
     *      </Property>
     * <Properties>
     *
     * In the above xml tree,
     *
     * key                   = "Properties"
     * itemKey               = "Property"
     * param comparisionAttribute  = "identifier"
     * param sMap["enableMIP"]     = address of a property
     *         sMap["enableShading"] = address of a property
     *         where, "enableMIP" & "enableShading" are keys.
     *         address of a property is a value
     *
     * Note: If children has attribute "type" , then comparisionAttribute becomes meaningless.
     *       Because deserializer always allocates a new instance of type using registered factories.
     *
     *       eg., <Processor type="EntryExitPoints" identifier="EntryExitPoints" reference="ref2" />
     *
     * @param const std::string & key Map key or parent node of itemKey.
     * @param std::map<K    
     * @param V * 
     * @param C> & sMap  map to be deserialized - source / input map. 
     * @param const std::string & itemKey map item key of childeren nodes.
     * @param const std::string & comparisionAttribute  - forced comparison attribute.     
     */    
    template <typename K, typename V, typename C, typename A>
    void deserialize(const std::string &key, 
                     std::map<K,V,C,A> &sMap, 
                     const std::string &itemKey, 
                     const std::string &comparisionAttribute = IvwSerializeConstants::KEY_ATTRIBUTE);

    /** 
     * \brief Deserialize string data.
     * 
     * @param const std::string & key Parent node key e.g, "Property"
     * @param std::string & data string data to be deserialized
     * @param const bool asAttribute if attribute is true the xml node is formatted as <Key data="this is an attribute"\>, otherwise <Key> <data="this is non-attribute"> <Key\>     
     */
    void deserialize(const std::string &key, 
                     std::string &data, 
                     const bool asAttribute=false);    
   
    // primitive types
    void deserialize(const std::string &key, bool &data);
    void deserialize(const std::string &key, float &data);
    void deserialize(const std::string &key, double &data);
    void deserialize(const std::string &key, int &data);
    void deserialize(const std::string &key, unsigned int &data);
    void deserialize(const std::string &key, long &data);
    void deserialize(const std::string &key, long long &data);

    // glm vector types
    template<class T>
    void deserialize(const std::string& key, glm::detail::tvec4<T>& data);
    template<class T>
    void deserialize(const std::string& key, glm::detail::tvec3<T>& data);
    template<class T>
    void deserialize(const std::string& key, glm::detail::tvec2<T>& data);
    // glm matrix types
    template<class T>
    void deserialize(const std::string& key, glm::detail::tmat4x4<T>& data);
    template<class T>
    void deserialize(const std::string& key, glm::detail::tmat3x3<T>& data);
    template<class T>
    void deserialize(const std::string& key, glm::detail::tmat2x2<T>& data);
    
    /** 
     * \brief  Deserialize any Serializable object
     */
    void deserialize(const std::string &key, IvwSerializable &sObj);
    /** 
     * \brief  Deserialize pointer data of type T, which is of type 
     *         serializeble object or primitive data
     */
    template <class T>
    void deserialize(const std::string& key, T* & data, bool getChild=true);
    
protected:
    friend class NodeSwitch;
private:
    template <typename T>
    void deserializeSTL_Vector(const std::string &key, 
                               std::vector<T*> &sVector, 
                               const std::string &itemKey);
    template <typename T>
    void deserializeSTL_Vector(const std::string &key, 
                               std::vector<T> &sVector, 
                               const std::string &itemKey);

    /** 
     * \brief Deserialize primitive string data type which is not an attribute 
     *        that is formatted as <Key> <data="this is non-attribute"> <Key\>     
     * 
     * @param key  Parent node key e.g, "Property"
     * @param data Object to be deserialized
     */
    void deserializePrimitives(const std::string &key, std::string &data);

    /** 
     * \brief Deserialize primitive data type which string data which is an 
     *        attribute that is formatted as <Key data="this is an attribute"\>     
     *
     * @param key Parent node key e.g, "Property"
     * @param data Object to be deserialized
     */
    void deserializeAttributes(const std::string &key, std::string &data);
    
    /** 
     * \brief Deserialize primitive data type such as int, long, float, etc., 
     *        (except string data) which is not an attribute that is formatted
     *        as <Key> <stepValue=1.0> <Key\>
     *
     * @param key Parent node key e.g, "Property"
     * @param data Object to be deserialized of type int, long, float, etc., 
     *        (except string)
     */
    template <typename T>
    void deserializePrimitives(const std::string& key, T& data);
    
    /** 
     * \brief Deserialize vector data structure vec2, ive2, vec3, ivec3, etc.,
     *
     * @param const std::string & key Parent node key e.g, "Property"
     * @param T & vector Glm data structures such as vec2, ive2, vec3, ivec3, etc.,
     * @param const bool & isColor If (isColor==true) x="0" y="0" z="0" , 
     *        If (isColor==false) isColor r="0" g="0" b="0"
     */
    template <class T>
    void deserializeVector(const std::string& key, 
                           T& vector, 
                           const bool& isColor=false);

    /** 
     * \brief Deserialize pointer
     *
     * @param key Parent node key e.g, "Property"
     * @param data If data==NULL then deserializer tried to 
     *        allocated data, otherwise pre-allocated data should be passed.
     */
    template <class T>
    void deserializePointer(const std::string& key, T* & data);
};

template<class T>
void IvwDeserializer::deserialize(const std::string& key, glm::detail::tvec4<T>& data) {
    deserializeVector(key, data);
}
template<class T>
void IvwDeserializer::deserialize(const std::string& key, glm::detail::tvec3<T>& data) {
    deserializeVector(key, data);
}
template<class T>
void IvwDeserializer::deserialize(const std::string& key, glm::detail::tvec2<T>& data) {
    deserializeVector(key, data);
}

template<class T>
void IvwDeserializer::deserialize(const std::string &key, glm::detail::tmat4x4<T>& data) {
    try {
        TxElement* keyNode = rootElement_->FirstChildElement(key);
        NodeSwitch tempNodeSwitch(*this, keyNode);

        glm::detail::tvec4<T> rowVec;
            for(size_t i = 0; i < 4; i++) {
            std::stringstream key;
            key << "row" << i;
            deserializeVector(key.str(), rowVec);
            data[i][0] = rowVec[0]; 
            data[i][1] = rowVec[1];
            data[i][2] = rowVec[2];
            data[i][3] = rowVec[3];
        }
    } catch(TxException&) {}
}
template<class T>
void IvwDeserializer::deserialize(const std::string &key, glm::detail::tmat3x3<T>& data) {
    try {
        TxElement* keyNode = rootElement_->FirstChildElement(key);
        NodeSwitch tempNodeSwitch(*this, keyNode);

        glm::detail::tvec3<T> rowVec;
        for(size_t i = 0; i < 3; i++) {
            std::stringstream key;
            key << "row" << i;
            deserializeVector(key.str(), rowVec);
            data[i][0] = rowVec[0];
            data[i][1] = rowVec[1];
            data[i][2] = rowVec[2];
        }
    } catch(TxException&) {}
}
template<class T>
void IvwDeserializer::deserialize(const std::string &key, glm::detail::tmat2x2<T>& data) {
    try {
        TxElement* keyNode = rootElement_->FirstChildElement(key);
        NodeSwitch tempNodeSwitch(*this, keyNode);

        glm::detail::tvec2<T> rowVec;
        for(size_t i = 0; i < 2; i++) {
            std::stringstream key;
            key << "row" << i;
            deserializeVector(key.str(), rowVec);
            data[i][0] = rowVec[0];
            data[i][1] = rowVec[1];
        }
    } catch(TxException&) {}
}


template <typename T>
inline void IvwDeserializer::deserialize(const std::string &key,
                                         std::vector<T> &sVector,
                                         const std::string &itemKey) {
    deserializeSTL_Vector(key, sVector, itemKey);
}

template <typename T>
inline void IvwDeserializer::deserializeSTL_Vector(const std::string &key,
                                                   std::vector<T*> &sVector,
                                                   const std::string &itemKey) {
    
    TxElement* keyNode;


    try {
        keyNode = rootElement_->FirstChildElement(key);
        keyNode->FirstChildElement();
    } catch (TxException&) {
        return;
    }

    NodeSwitch tempNodeSwitch(*this, keyNode);
  
    T* item;
    std::vector<T*> tVector;

    try {    
        //TODO: Add count attribute to store vector.size() if necessary
        unsigned int i=0;
        for (TxEIt child(keyNode->FirstChildElement(itemKey), itemKey); child != child.end(); ++child) {
            rootElement_ = &(*child);
            
            if (sVector.size()==0) {
                item = 0;
                deserializePointer(itemKey, item);
                tVector.push_back(item);
            }
            else {
                deserializePointer(itemKey, sVector[i]);
                tVector.push_back(sVector[i]);
                i++;
            }        
        }
    }catch (TxException&) {
        //Proceed to next node safely if exception encountered
    }

    sVector = tVector;
}

template <typename T>
inline void IvwDeserializer::deserializeSTL_Vector(const std::string &key,
                                                   std::vector<T> &sVector,
                                                   const std::string &itemKey) {

    TxElement* keyNode;


    try {
        keyNode = rootElement_->FirstChildElement(key);
        keyNode->FirstChildElement();
    } catch (TxException&) {
        return;
    }

    NodeSwitch tempNodeSwitch(*this, keyNode);    
    
    T item;
    std::vector<T> tVector;

    try {
        //space holder
        if (sVector.size()==0) {
            for (TxEIt child(keyNode->FirstChildElement(itemKey), itemKey); child != child.end(); ++child) {
                T emptyItem;
                tVector.push_back(emptyItem);
            }
        }
        sVector = tVector;

        unsigned int i=0;
        for (TxEIt child(keyNode->FirstChildElement(itemKey), itemKey); child != child.end(); ++child) {
            rootElement_ = &(*child);
            deserialize(itemKey, sVector[i++]);
        }
    }catch (TxException&) {
        //Proceed to next node safely if exception encountered
        sVector.clear();
    }
     
}

template <typename K, typename V, typename C, typename A>
void IvwDeserializer::deserialize(const std::string &key, 
                                                std::map<K, V, C, A> &map,
                                                const std::string &itemKey, 
                                                const std::string &comparisionAttribute) {

    if(!isPrimitiveType(typeid(K)) || comparisionAttribute.empty()) {
        throw SerializationException("Error: map key has to be a primitive type");
    }

    try {
        TxElement* keyNode = rootElement_->FirstChildElement(key);
        NodeSwitch tempNodeSwitch(*this, keyNode);

        TxEIt child(itemKey);
        for(child = child.begin(rootElement_); child != child.end(); ++child) {
            NodeSwitch tempNodeSwitch(*this, &(*child));

            K key;
            child->GetAttribute(comparisionAttribute, &key);

            V value;
            typename std::map<K, V, C, A>::iterator it = map.find(key);

            if(it != map.end()) {
                value = it->second;
            } else {
                value = NULL;
            }

            deserialize(itemKey, value, false);
            map[key] = value;
        }
        
    } catch(TxException&) {
        return;
    }

}

template<class T>
void IvwDeserializer::deserializePointer(const std::string& /*key*/, T* & data) {
    
    TxElement* nextRootNode;
    TxElement* rootCopy = rootElement_;

    try {
        nextRootNode = rootElement_->FirstChildElement(); 
        //if (!keyNode) return;
    }
    catch (TxException& ) {
        nextRootNode = 0;
    }

    std::string type_attr("");
    std::string ref_attr("");
    std::string id_attr("");
    try {
        rootElement_->GetAttribute(IvwSerializeConstants::TYPE_ATTRIBUTE, &type_attr);
        if (allowRef_) {
            try {
                rootElement_->GetAttribute(IvwSerializeConstants::REF_ATTRIBUTE, &ref_attr);
            } catch (TxException& ) {}
            rootElement_->GetAttribute(IvwSerializeConstants::ID_ATTRIBUTE, &id_attr);
        }
    } catch (TxException& ) {}


    //allocation of referenced data
    if (!data) {
        if (!ref_attr.empty()) {
            //allocate only

            //search in reference list
            //if data doesnt exist just allocate
            // and add to list
            //return the data, donot deserialize it yet

           data=static_cast<T*>(refDataContainer_.find(type_attr, ref_attr));

            if (!data) {
                if (!type_attr.empty()) {
                    data = IvwSerializeBase::getRegisteredType<T>(type_attr);                        
                }
                else {
                    data = IvwSerializeBase::getNonRegisteredType<T>();                    
                }
                if (data) refDataContainer_.insert(data, rootCopy);
            }            
            
            rootElement_ = nextRootNode;

            return;

        }
        else if (!id_attr.empty()) {
            //search in the reference list
            //if data exist , data needs to deserialized but not allocated
            //else data needs to be both allocated and deserialized

            data = static_cast<T*>(refDataContainer_.find(type_attr, id_attr));
            
            if (!data) {
                if (!type_attr.empty()) {
                    data = IvwSerializeBase::getRegisteredType<T>(type_attr);     
                    if (data) { 
                        (*data).deserialize(*this);
                        rootElement_ = nextRootNode;
                    }     
                }
                else {
                    data = IvwSerializeBase::getNonRegisteredType<T>();
                    if (data) {
                        (*data).deserialize(*this);
                        rootElement_ = nextRootNode; 
                    }
                    
                }
                if (data) refDataContainer_.insert(data, rootCopy);
            }
            else {
                (*data).deserialize(*this);
                rootElement_ = nextRootNode;
            }
            
            return;
        }
   }
 

    //allocation of non-referenced data
    if (!data) {
        if (!type_attr.empty()) {
            data = IvwSerializeBase::getRegisteredType<T>(type_attr);
            if (data) {
               (*data).deserialize(*this);
                rootElement_ = nextRootNode;
            }     
        }
        else {
            data = IvwSerializeBase::getNonRegisteredType<T>();
            if (data) {
                (*data).deserialize(*this);
                rootElement_ = nextRootNode;  
            }
            
        }
        if (data) refDataContainer_.insert(data, rootCopy);
    }
    else {
         refDataContainer_.insert(data, rootCopy);
         (*data).deserialize(*this);
         rootElement_ = nextRootNode;
    }    
    
    return;

}

template<class T>
inline void IvwDeserializer::deserialize(const std::string& key, T* & data, bool getChild) {
    TxElement* keyNode;
 
    if(getChild){
        try {
            keyNode = rootElement_->FirstChildElement(key); 
        } catch (TxException& ) {
            return;
        }
    }else{
        keyNode = rootElement_;
    }

    std::string type_attr("");
    std::string ref_attr("");
    std::string id_attr("");
    try {
        keyNode->GetAttribute( IvwSerializeConstants::TYPE_ATTRIBUTE, &type_attr );
    } catch (TxException& ) {}

    if (allowRef_) {
        try {
            keyNode->GetAttribute( IvwSerializeConstants::REF_ATTRIBUTE, &ref_attr );
        } catch (TxException&) {}
        try {
            keyNode->GetAttribute( IvwSerializeConstants::ID_ATTRIBUTE, &id_attr );
        } catch (TxException&) {}
    }

    if (!data) {
        if (!ref_attr.empty()) {
            // Has reference identifier, data should already be allocated and we only have to find
            // and set the pointer to it.
            data=static_cast<T*>(refDataContainer_.find(type_attr, ref_attr));
            return;
        } else if (!type_attr.empty()) {
            data = IvwSerializeBase::getRegisteredType<T>(type_attr);
        } else {
            data = IvwSerializeBase::getNonRegisteredType<T>();
        }
    }
    if (data) {
        (*data).deserialize(*this);
        if (!id_attr.empty()) {
            refDataContainer_.insert(data, keyNode);
        }
    }
}


template<class T>
inline void IvwDeserializer::deserializePrimitives(const std::string& key, T& data) {
    TxElement* keyNode = rootElement_->FirstChildElement(key, false);
    if (keyNode && keyNode->HasAttribute(IvwSerializeConstants::CONTENT_ATTRIBUTE)){
        try{
            keyNode->GetAttribute(IvwSerializeConstants::CONTENT_ATTRIBUTE, &data);
        }catch(TxException&){
            return;
        }
    }else{
        return;
    }
}

template<class T>
inline void IvwDeserializer::deserializeVector(const std::string& key, T& vector, const bool& isColor) {

    TxElement* keyNode = rootElement_->FirstChildElement(key, false); 
    if (!keyNode) {        
        //Try to finding key in the current node before exit. If not, let the exception be thrown.
        try {
            T tempVec;
            rootElement_->GetAttribute(
                isColor ? IvwSerializeConstants::COLOR_R_ATTRIBUTE : IvwSerializeConstants::VECTOR_X_ATTRIBUTE, &tempVec[0]);
            keyNode = rootElement_;
        }
        catch (TxException& ) {
            return;
        }        
    }

    std::string attr;

    keyNode->GetAttribute(
        isColor ? IvwSerializeConstants::COLOR_R_ATTRIBUTE : IvwSerializeConstants::VECTOR_X_ATTRIBUTE, &vector[0]);

    if (vector.length() >= 2) {
        keyNode->GetAttribute(
        isColor ? IvwSerializeConstants::COLOR_G_ATTRIBUTE : IvwSerializeConstants::VECTOR_Y_ATTRIBUTE, &vector[1]);
    }

    if (vector.length() >= 3) {
        keyNode->GetAttribute(
        isColor ? IvwSerializeConstants::COLOR_B_ATTRIBUTE : IvwSerializeConstants::VECTOR_Z_ATTRIBUTE, &vector[2]);
    }

    if (vector.length() >= 4) {
       keyNode->GetAttribute(
        isColor ? IvwSerializeConstants::COLOR_A_ATTRIBUTE : IvwSerializeConstants::VECTOR_W_ATTRIBUTE, &vector[3]);
    }
}

} //namespace
#endif