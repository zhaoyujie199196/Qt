////
//// Created by Yujie Zhao on 2023/2/15.
////

#include "qmetatype.h"
#include "qmetatype_p.h"

QT_BEGIN_NAMESPACE

//static 只对正在编译的源代码文件
//const 表示不能修改里面的成员
static const struct QMetaTypeModuleHelper {
    const QtPrivate::QMetaTypeInterface *interfaceForType(int type) const {
        switch(type) {
            QT_FOR_EACH_STATIC_PRIMITIVE_TYPE(QT_METATYPE_CONVERT_ID_TO_TYPE)
            QT_FOR_EACH_STATIC_CORE_CLASS(QT_METATYPE_CONVERT_ID_TO_TYPE)
        default:
            return nullptr;
        }
    }
};

QMetaTypeModuleHelper metatypeHelper;

static const QMetaTypeModuleHelper *qMetaTypeCoreHelper = &metatypeHelper;

static const QMetaTypeModuleHelper *qModuleHelperForType(int type) {
    if (type < QMetaType::LastCoreType) {
        return qMetaTypeCoreHelper;
    }
    else {
        Q_ASSERT(false);
        return nullptr;
    }
}


static const QtPrivate::QMetaTypeInterface *interfaceForType(int typeId) {
    const QtPrivate::QMetaTypeInterface *interface = nullptr;
    if (typeId >= QMetaType::User) {
        Q_ASSERT(false);
        return nullptr;
    }
    else {
        auto moduleHelper = qModuleHelperForType(typeId);
        if (moduleHelper) {
            interface = moduleHelper->interfaceForType(typeId);
        }
    }
    if (!interface && typeId != QMetaType::UnknownType) {
        Q_ASSERT(false);
    }
    return interface;
}

QMetaType::QMetaType(int typeId)
    : QMetaType(interfaceForType(typeId))
{

}

//创建数据，使用默认构造
void *QMetaType::create(const void *copy) const {
    if (d_ptr && (copy ? !!d_ptr->copyCtr : !!d_ptr->defaultCtr)) {
        //定义了默认对齐的宏，new采用的是默认对齐的方案
        //在有些系统中，alignof(T)可能会大于__STDCPP_DEFAULT_NEW_ALIGNMENT__，如果不对齐，可能会产生一些性能方面的损耗
#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
        //operator new只分配内存，不会嗲用构造函数
        //std::align_val_t是枚举类型
        void *address = d_ptr->alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__ ?
                operator new(d_ptr->size, std::align_val_t(d_ptr->alignment)) :
                operator new(d_ptr->size);
#else
        void *address = operator new(d_ptr->size);
#endif
        return construct(address, copy);
    }
    else {
        Q_ASSERT(d_ptr && d_ptr->defaultCtr);
        return nullptr;
    }
}

void QMetaType::destory(void *data) const {
    //operator new只负责分配内存，不会调用构造函数
    //operator delete只释放内存，不会调用析构函数
    if (d_ptr) {
        //先尝试调用析构函数，然后调用delete释放data的内存
        if (d_ptr->dtor) {
            d_ptr->dtor(d_ptr, data);
        }
        if (d_ptr->alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            operator delete(data, std::align_val_t(d_ptr->alignment));
        }
        else {
            operator delete(data);
        }
    }
}

void *QMetaType::construct(void *address, const void *copy) const {
    if (!address) {
        return nullptr;
    }
    if (d_ptr) {
        if (copy) {
            d_ptr->copyCtr(d_ptr, address, copy);
            return address;
        }
        else if (!copy && d_ptr->defaultCtr){
            d_ptr->defaultCtr(d_ptr, address);
            return address;
        }
    }
    Q_ASSERT(false);
    return nullptr;
}

QT_END_NAMESPACE
