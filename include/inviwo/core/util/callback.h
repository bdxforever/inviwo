#ifndef IVW_CALLBACK_H
#define IVW_CALLBACK_H

#include <inviwo/core/common/inviwo.h>

namespace inviwo {

class BaseCallBack {
public:
    virtual void invoke() const=0;
};


template <typename T>
class MemberFunctionCallBack : public BaseCallBack {
public:
    typedef void (T::*fPointerType1)();

    MemberFunctionCallBack(T* obj, fPointerType1 functionPtr) 
        : functionPtr1_(functionPtr)
        , obj_(obj){}

    virtual void invoke() const{
        if (functionPtr1_) (*obj_.*functionPtr1_)();
    }

private:
    fPointerType1 functionPtr1_;
    T* obj_;
};


// Example usage
// CallBackList cbList;
// cbList.addMemberFunction(&myClassObject, &MYClassObject::myFunction);

//TODO: Use map that uses string as keys : CallBackMap
class CallBackList {
public:
    CallBackList() {}
    void invokeAll() const{ 
        for (std::vector<BaseCallBack*>::const_iterator it=callBackList_.begin(); it!=callBackList_.end(); ++it)
            (*it)->invoke();        
    }

    template <typename T>
    void addMemberFunction(T* o, void (T::*m)()) {
        callBackList_.clear();
        callBackList_.push_back(new MemberFunctionCallBack<T>(o, m));
    }

private:
    std::vector<BaseCallBack*> callBackList_;
};

class SingleCallBack {
public:
    SingleCallBack() : callBack_(0) {}

    void invoke() const{
        if (callBack_)
            callBack_->invoke();
    }

    template <typename T>
    void addMemberFunction(T* o, void (T::*m)()){
        callBack_ = new MemberFunctionCallBack<T>(o,m);
    }

private:
    BaseCallBack* callBack_;
};


} // namespace

#endif // IVW_CALLBACK_H
