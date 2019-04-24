#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <type_traits>
#include <functional>
#include <map>
#include <string>

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

	struct BaseType {
		BaseType() : id(counter++) {}
		virtual std::string toString() { return ""; }
		int getId() { return id; }
	private:
		static int counter;
		const int id;
	};

	int BaseType::counter = 0;

	template <typename AttrType>
	struct AttributeAccessor {

		static AttributeAccessor<AttrType>* get(const std::string& name) {
			auto it = attributes.find(name);
			if (it != attributes.end()) {
				return it->second.get();
			}
			else {
				auto attr = std::make_shared<AttributeAccessor<AttrType>>();
				attributes.emplace(name, attr);
				return attr.get();
			}
		}

		void addType(const std::string& typeName) {
			if (values.find(typeName) == values.end()) {
				values.emplace(typeName, std::map<int, AttrType>());
			}
		}

		AttrType getValueFor(BaseType* obj) {
			auto itType = values.find(obj->toString());
			if (itType != values.end()) {
				const auto& innerMap = itType->second;
				auto itValue = innerMap.find(obj->getId());
				if (itValue != innerMap.end()) {
					return itValue->second;
				}
			}
			return AttrType();
		}

	private:
		static std::map<std::string, std::shared_ptr<AttributeAccessor<AttrType>>> attributes;
		std::map<std::string, std::map<int, AttrType>> values;
	};
}
#define TYPE(TypeName) \
    struct TypeName : public ot::Reference<ot::Type>, public ot::BaseType { \
        TypeName() : ot::Reference<ot::Type>(), ot::BaseType() {} \
        TypeName(const ot::Reference<ot::Type>& other) : ot::Reference<ot::Type>(other){} \
		virtual std::string toString() override { return #TypeName; }  \
    };

#define ATTR1(AttrName, TypeName, AttrType) \
	ot::AttributeAccessor<AttrType>::get(#AttrName)->addType(#TypeName); 

#endif // OPEN_TYPES_HPP
