#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <type_traits>
#include <functional>
#include <map>

namespace ot {
    template <typename T>
    class Reference {
    public:
        Reference(bool create = false)
        : object(create ? new T() : nullptr),
          refCount(new unsigned int),
          shouldDeleteOnDestruct(true)
        {
            *refCount = 1;
        }

        ~Reference() {
            if(--(*refCount) == 0) {
                if(shouldDeleteOnDestruct && object) {
                    delete object;
                }
                if(refCount) {
                    delete refCount;
                }
            }
        }

        T* get() const {
            return object;
        }

        unsigned int* getRefCountPtr() const {
            return refCount;
        }

        Reference(T* ptr)
        : object(ptr),
          refCount(new unsigned int) {
            *refCount = 1;
        }

        Reference(const Reference& other)
        : object(other.get()),
          refCount(other.getRefCountPtr()) {
            if(refCount) {
                *refCount += 1;
            }
        }

        template <typename B,
                  typename = std::enable_if<std::is_base_of<B, T>::value>
        >
        Reference(const Reference<B>& other)
        : object(other.get()),
          refCount(other.getRefCountPtr()) {
            if(refCount) {
                *refCount++;
            }
        }

        Reference& operator=(const Reference& other) {
            if(refCount) {
                if(*--refCount == 0) {
                    if(shouldDeleteOnDestruct && object != nullptr) {
                        delete object;
                    }
                }
            }
            object = other.object;
            refCount = other.refCount;
            if(refCount) {
                *refCount++;
            }
            return *this;
        }

        bool operator==(const Reference& other) {
            return object == other.get();
        }

        bool operator!=(const Reference& other) {
            return !(object == other.get());
        }

        operator bool() const {
            return (object != nullptr);
        }

    protected:
        T* object;
        unsigned int* refCount;
        bool shouldDeleteOnDestruct = false;
    };

    struct Type {
    };

    static const Reference<Type> nil(nullptr);

    Reference<Type> empty() {
        return Reference<Type>(true);
    }

    template <typename T, typename A>
    A attribute(T obj, std::function<A (const T&)> func) {
        func(obj);
    }
}

#define TYPE(TypeName) \
    struct TypeName : public ot::Reference<ot::Type> { \
        TypeName() : ot::Reference<ot::Type>() {} \
        TypeName(const ot::Reference<ot::Type>& other) : ot::Reference<ot::Type>(other){} \
    };

#define ATTR1(AttrName, TypeName, AttrType) \
    std::map<TypeName, AttrType> TypeName ##_ ##AttrName; \
    AttrType AttrName(const TypeName& object) { \
        auto it = TypeName ##_ ##AttrName.find(object); \
        if(it != TypeName ##_ ##AttrName.end()) { \
            return it->second; \
        } \
        return AttrType(); \
    }

#endif // OPEN_TYPES_HPP
