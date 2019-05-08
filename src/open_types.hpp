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

	Reference<Type> empty() {
		return Reference<Type>(true);
	}

	template <typename T>
	class vector : public std::vector<T> {
	public:
		T operator[](const size_t i) {
			if (size() > i) {
				return at(i);
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
        T operator[](T (*read)(TypeName)) const { return read(*this); } /* Read attribute value */ \
        template <typename T> \
        TypeName& operator()(void (*write)(TypeName, T), T value) { if(*this) { write(*this, value); } return *this; } /* Write attribute / Add value to attribute list */ \
		template <typename T> \
		TypeName& operator()(void (*writeAt)(TypeName, size_t, T), size_t i, T value) { if(*this) { writeAt(*this, i, value); } return *this; }  /* Write value to position in attribute list */  \
		void operator-=(void (*remove)(TypeName, TypeName*)) const { remove(*this, nullptr); }  /* Remove attribute value */  \
    };

#define ATTR1(AttrName, TypeName, AttrType) \
    static std::map<ot::Type*, AttrType> TypeName ##_ ##AttrName; \
	/* Read */ \
    AttrType AttrName(TypeName object) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object.get()); \
        if(it != attr_map.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
	/* Remove */ \
	void AttrName(TypeName object, TypeName* deleter) { \
		TypeName ##_ ##AttrName.erase(object.get()); \
	} \
	/* Write */ \
    void AttrName(TypeName object, AttrType value) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object.get()); \
        if(it != attr_map.cend()) { \
            it->second = std::move(value); \
        } \
        else { \
            attr_map.emplace(object.get(), std::move(value)); \
        } \
    }

#define ATTRN(AttrName, TypeName, AttrType) \
	static std::map<ot::Type*, ot::vector<AttrType>> TypeName ##_ ##AttrName; \
	/* Read complete list */ \
    ot::vector<AttrType> AttrName(TypeName object) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object.get()); \
        if(it != attr_map.cend()) { \
			return it->second; \
        } \
        else { \
            return ot::vector<AttrType>(); \
        } \
    } \
	/* Remove */ \
	void AttrName(TypeName object, TypeName* deleter) { \
		TypeName ##_ ##AttrName.erase(object.get()); \
	} \
	/* Write item at end*/ \
    void AttrName(TypeName object, AttrType value) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object.get()); \
        if(it == attr_map.cend()) { \
           it = attr_map.emplace(object.get(), ot::vector<AttrType>()).first; \
        } \
		it->second.emplace_back(std::move(value)); \
    } \
	/* Write item at position*/ \
    void AttrName(TypeName object, size_t i, AttrType value) { \
        auto& attr_map = TypeName ##_ ##AttrName; \
        auto it = attr_map.find(object.get()); \
        if(it != attr_map.cend()) { \
			if(it->second.size() > i) { \
				it->second.at(i) = std::move(value); \
			} \
        } \
    }

#endif // OPEN_TYPES_HPP
