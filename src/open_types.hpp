#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <type_traits>
#include <functional>
#include <map>
#include <string>
#include <memory>

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

}
#define TYPE(TypeName) \
    struct TypeName : public ot::Reference<ot::Type> { \
        TypeName() : ot::Reference<ot::Type>() {} \
        TypeName(const ot::Reference<ot::Type>& other) : ot::Reference<ot::Type>(other){} \
        template <typename T> \
        T operator[](T (*func)(TypeName)) const { return func(*this); }  \
        template <typename T> \
        void operator()(void (*func)(TypeName, const T&), const T& value) const { func(*this, value); }  \
    };

#define ATTR1(AttrName, TypeName, AttrType) \
    static std::map<TypeName, AttrType> TypeName ##_ ##AttrName; \
    AttrType AttrName(TypeName object) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object); \
        if(it != attr_map.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
    void AttrName(TypeName object, const AttrType& value) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object); \
        if(it != attr_map.cend()) { \
            it->second = value; \
        } \
        else { \
            attr_map.emplace(object, value); \
        } \
    }

#endif // OPEN_TYPES_HPP
