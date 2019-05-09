#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <type_traits>
#include <functional>
#include <map>
#include <string>
#include <memory>
#include <vector>

namespace ot {
    template <typename T>
    class Reference {
    public:
        Reference(bool create = false)
        : object(create ? new T() : nullptr),
          refCount(new unsigned int),
          shouldDeleteOnDestruct(create)
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
                (*refCount)++;
            }
        }

        template <typename B,
                  typename = std::enable_if<std::is_base_of<B, T>::value>
        >
        Reference(const Reference<B>& other)
        : object(other.get()),
          refCount(other.getRefCountPtr()) {
            if(refCount) {
                (*refCount)++;
            }
        }

        Reference& operator=(const Reference& other) {
            if(refCount) {
                if(--(*refCount) == 0) {
                    if(shouldDeleteOnDestruct && object != nullptr) {
                        delete object;
                    }
					delete refCount;
                }
            }
            object = other.object;
            refCount = other.refCount;
            if(refCount) {
                (*refCount)++;
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

    inline Reference<Type> empty() {
		return Reference<Type>(true);
	}

	template <typename T>
	class vector : public std::vector<T> {
	public:
		T operator[](const size_t i) {
            if (this->size() > i) {
                return this->at(i);
			}
			else {
				return T();
			}
		}
	};

}
#define TYPE(TypeName) \
    struct TypeName : public ot::Reference<ot::Type> { \
        TypeName() : ot::Reference<ot::Type>() {} \
        TypeName(const ot::Reference<ot::Type>& other) : ot::Reference<ot::Type>(other){} \
        template <typename T> \
        TypeName(void (*write)(TypeName, T), T value) { if(*this) { write(*this, value); } } /* Write attribute / Add value to attribute list */ \
        template <typename T> \
        T operator[](T (*read)(TypeName)) const { return read(*this); } /* Read attribute value */ \
        template <typename T> \
        TypeName& operator()(void (*write)(TypeName, T), T value) { if(*this) { write(*this, value); } return *this; } /* Write attribute / Add value to attribute list */ \
		template <typename T> \
		TypeName& operator()(void (*writeAt)(TypeName, size_t, T), size_t i, T value) { if(*this) { writeAt(*this, i, value); } return *this; }  /* Write value to position in attribute list */  \
		void operator-=(void (*remove)(TypeName, TypeName*)) const { remove(*this, nullptr); }  /* Remove attribute value */  \
    };

#define ATTR1(AttrName, TypeName, AttrType) \
    inline std::map<ot::Type*, AttrType>& TypeName ##_ ##AttrName() { \
        static std::map<ot::Type*, AttrType> attrMap; \
        return attrMap; \
    } \
	/* Read */ \
    inline AttrType AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if(it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
	/* Remove */ \
    inline void AttrName(TypeName object, TypeName*) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        attrMap.erase(object.get()); \
	} \
	/* Write */ \
    inline void AttrName(TypeName object, AttrType value) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if(it != attrMap.cend()) { \
            it->second = std::move(value); \
        } \
        else { \
            attrMap.emplace(object.get(), std::move(value)); \
        } \
    }

#define ATTRN(AttrName, TypeName, AttrType) \
    inline std::map<ot::Type*, ot::vector<AttrType>>& TypeName ##_ ##AttrName() { \
        static std::map<ot::Type*, ot::vector<AttrType>> attrMap; \
        return attrMap; \
    } \
	/* Read complete list */ \
    inline ot::vector<AttrType> AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if(it != attrMap.cend()) { \
			return it->second; \
        } \
        else { \
            return ot::vector<AttrType>(); \
        } \
    } \
	/* Remove */ \
    inline void AttrName(TypeName object, TypeName*) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        attrMap.erase(object.get()); \
	} \
	/* Write item at end*/ \
    inline void AttrName(TypeName object, AttrType value) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if(it == attrMap.cend()) { \
           it = attrMap.emplace(object.get(), ot::vector<AttrType>()).first; \
        } \
		it->second.emplace_back(std::move(value)); \
    } \
	/* Write item at position*/ \
    inline void AttrName(TypeName object, size_t i, AttrType value) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if(it != attrMap.cend()) { \
			if(it->second.size() > i) { \
				it->second.at(i) = std::move(value); \
			} \
        } \
    }

#endif // OPEN_TYPES_HPP
