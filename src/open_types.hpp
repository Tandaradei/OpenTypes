#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <type_traits>
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace ot {
    template <typename T>
    class Reference {
    public:
        Reference(bool create = false)
        : object(create ? new T() : nullptr),
          refCount(new unsigned int(create ? 1 : 0)),
          shouldDeleteOnDestruct(new bool(create))
        {}

        ~Reference() {
			(*refCount)--;
            if(*refCount == 0) {
                if(*shouldDeleteOnDestruct && object) {
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

		bool* getShouldDeleteOnDestruct() const {
			return shouldDeleteOnDestruct;
		}

        Reference(T* ptr)
        : object(ptr),
          refCount(new unsigned int(1)),
		  shouldDeleteOnDestruct(new bool(false)) 
		{}

        Reference(const Reference& other)
        : object(other.get()),
          refCount(other.getRefCountPtr()),
		  shouldDeleteOnDestruct(other.getShouldDeleteOnDestruct()) {
            if(refCount) {
                (*refCount)++;
            }
        }

        template <typename B,
                  typename = std::enable_if<std::is_base_of<B, T>::value>
        >
        Reference(const Reference<B>& other)
        : object(other.get()),
          refCount(other.getRefCountPtr()),
		  shouldDeleteOnDestruct(other.getShouldDeleteOnDestruct()) {
            if(refCount) {
                (*refCount)++;
            }
        }

        Reference& operator=(const Reference& other) {
            if(refCount) {
                if(--(*refCount) == 0) {
                    if(*shouldDeleteOnDestruct && object != nullptr) {
                        delete object;
                    }
					delete refCount;
                }
            }
            object = other.object;
            refCount = other.refCount;
			shouldDeleteOnDestruct = other.shouldDeleteOnDestruct;
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
        bool* shouldDeleteOnDestruct;
    };

	/**
	Dummy class to differentiate between function declarations
	*/
	struct AttributeDeleter {
	};

	/**
	Base type for all custom types
	*/
    struct Type {
		std::vector<std::function<void(Type*)>> removers;
		~Type() {
			for (auto remover : removers) {
				remover(this);
			}
		}
    };

	/**
	Reference to nullptr
	*/
	static const Reference<Type> nil(nullptr);

	/**
	Returns a reference to a new created object
	*/
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

	template <typename O, typename T>
	struct ItemDeleter {
		void (*remove)(O, std::function<std::pair<bool, bool>(T, size_t)>);
		std::function<std::pair<bool, bool>(T, size_t)> shouldRemove;
	};

}
#define TYPE(TypeName) \
    struct TypeName : public ot::Reference<ot::Type> { \
        TypeName() : ot::Reference<ot::Type>() {} \
        TypeName(const ot::Reference<ot::Type>& other) : ot::Reference<ot::Type>(other){} \
		/* Write attribute  on construction */ \
        template <typename T> \
        TypeName(void (*write)(TypeName, T), T value) \
        : ot::Reference<ot::Type>(true) { \
			write(*this, value); \
		} \
		/* Read attribute value */ \
        template <typename T> \
        T operator[](T (*read)(TypeName)) const { \
			return read(*this); \
		} \
		/* Write attribute / Add value to attribute list */ \
        template <typename T> \
        TypeName& operator()(void (*write)(TypeName, T), T value) { \
			write(*this, value); \
			return *this; \
		} \
		/* Write value to position in attribute list */ \
		template <typename T> \
		TypeName& operator()(void (*writeAt)(TypeName, size_t, T), size_t i, T value) { \
			writeAt(*this, i, value); \
			return *this; \
		} \
		/* Remove attribute value */ \
		void operator-=(void (*remove)(TypeName, ot::AttributeDeleter)) { \
			remove(*this, {}); \
		} \
		/* Remove value from attribute list */ \
		template <typename T> \
		void operator-=(ot::ItemDeleter<TypeName, T> itemDeleter) { \
			itemDeleter.remove(*this, itemDeleter.shouldRemove); \
		} \
    };

#define REMOVE_VALUE(AttrName, TypeName, AttrType, Value) \
	ot::ItemDeleter<TypeName, AttrType> { AttrName, [](AttrType value, size_t index) { return std::pair<bool, bool>{ value == Value, false }; } }

#define REMOVE_VALUE_ALL(AttrName, TypeName, AttrType, Value) \
	ot::ItemDeleter<TypeName, AttrType> { AttrName, [](AttrType value, size_t index) { return std::pair<bool, bool>{ value == Value, true }; } }

#define REMOVE_ALL(AttrName, TypeName, AttrType) \
	ot::ItemDeleter<TypeName, AttrType> { AttrName, [](AttrType value, size_t index) { return std::pair<bool, bool>{ true, true }; } }

#define REMOVE_INDEX(AttrName, TypeName, AttrType, Index) \
	ot::ItemDeleter<TypeName, AttrType> { AttrName, [](AttrType value, size_t index) { return std::pair<bool, bool>{ index == Index, false }; } }

#define ATTR1(AttrName, TypeName, AttrType) \
	/* Attribute map */ \
    inline std::unordered_map<ot::Type*, AttrType>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot::Type*, AttrType> attrMap; \
        return attrMap; \
    } \
	/* Read */ \
    inline AttrType AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
	/* Write */ \
    inline void AttrName(TypeName object, AttrType value) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
            it->second = std::move(value); \
        } \
        else { \
            attrMap.emplace(object.get(), std::move(value)); \
			/* Add remover to delete attributes on object descruction */ \
			object.get()->removers.emplace_back([](ot::Type* object_ptr){ \
				auto& attrMap = TypeName ##_ ##AttrName(); \
				attrMap.erase(object_ptr); \
			}); \
        } \
    } \
	/* Remove */ \
	inline void AttrName(TypeName object, ot::AttributeDeleter) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        attrMap.erase(object.get()); \
	} 

#define ATTRN(AttrName, TypeName, AttrType) \
	/* Attribute map */ \
    inline std::unordered_map<ot::Type*, ot::vector<AttrType>>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot::Type*, ot::vector<AttrType>> attrMap; \
        return attrMap; \
    } \
	/* Read complete list */ \
    inline ot::vector<AttrType> AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			return it->second; \
        } \
        else { \
            return ot::vector<AttrType>(); \
        } \
    } \
	/* Add item at end */ \
    inline void AttrName(TypeName object, AttrType value) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it == attrMap.cend()) { \
			it = attrMap.emplace(object.get(), ot::vector<AttrType>()).first; \
			/* Add remover to delete attributes on object descruction */ \
			object.get()->removers.emplace_back([](ot::Type* object_ptr){ \
				auto& attrMap = TypeName ##_ ##AttrName(); \
				attrMap.erase(object_ptr); \
			}); \
        } \
		it->second.emplace_back(std::move(value)); \
    } \
	/* Add item at position */ \
    inline void AttrName(TypeName object, size_t i, AttrType value) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			if(it->second.size() > i) { \
				it->second.insert(begin(it->second) + i, std::move(value)); \
			} \
			else { \
				AttrName(object, value); \
			} \
        } \
    } \
	/* Remove */ \
	inline void AttrName(TypeName object, ot::AttributeDeleter) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        attrMap.erase(object.get()); \
	} \
	/* Remove item */ \
	inline void AttrName(TypeName object, std::function<std::pair<bool, bool> (AttrType value, size_t index)> remover) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			auto& values = it->second; \
			size_t index = 0; \
			for (auto v_it = begin(values); v_it != end(values);) { \
				auto result = remover(*v_it, index); \
				if (result.first) { \
					v_it = values.erase(v_it); \
					if (!result.second) { \
						break; \
					} \
				} \
				else { v_it++; } \
				index++; \
			} \
		} \
	} 

#define ATTR1Link(AttrName, TypeName, TypeNameOther, AttrNameOther) \
	/* Attribute map */ \
	inline std::unordered_map<ot::Type*, TypeNameOther>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot::Type*, TypeNameOther> attrMap; \
        return attrMap; \
	} \
	/* Read */ \
    inline TypeNameOther AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return ot::nil; \
        } \
    } \
	/* Write */ \
    inline void AttrName(TypeName object, TypeNameOther other) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			AttrNameOther(it->second, ot::AttributeDeleter {}); \
            it->second = other; \
        } \
        else { \
            attrMap.emplace(object.get(), other); \
			/* Add remover to delete attributes on object descruction */ \
			/* object.get()->removers.emplace_back([](ot::Type * object_ptr) { auto& attrMap = TypeName ##_ ##AttrName(); attrMap.erase(object_ptr); }); */ \
        } \
		if(AttrNameOther(other) != object) { \
			AttrNameOther(other, object); \
		} \
    } \
	/* Remove */ \
	inline void AttrName(TypeName object, ot::AttributeDeleter) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
		auto other = AttrName(object); \
		attrMap.erase(object.get()); \
		if(other != ot::nil) { \
			AttrNameOther(other, ot::AttributeDeleter {}); \
		} \
	} 

#define ATTRNLink(AttrName, TypeName, TypeNameOther, AttrNameOther) \
	/* Attribute map */ \
	inline std::unordered_map<ot::Type*, ot::vector<TypeNameOther>>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot::Type*, ot::vector<TypeNameOther>> attrMap; \
        return attrMap; \
	} \
	/* Read */ \
    inline ot::vector<TypeNameOther> AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return ot::vector<TypeNameOther>(); \
        } \
    } \
	/* Add item at end */ \
    inline void AttrName(TypeName object, TypeNameOther other) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it == attrMap.cend()) { \
			it = attrMap.emplace(object.get(), ot::vector<TypeNameOther>()).first; \
			/* Add remover to delete attributes on object descruction */ \
			object.get()->removers.emplace_back([](ot::Type* object_ptr){ \
				auto& attrMap = TypeName ##_ ##AttrName(); \
				attrMap.erase(object_ptr); \
			}); \
        } \
		it->second.emplace_back(other); \
		if(AttrNameOther(other) != object) { \
			AttrNameOther(other, object); \
		} \
    } \
	/* Add item at position */ \
    inline void AttrName(TypeName object, size_t i, TypeNameOther other) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			if(it->second.size() > i) { \
				it->second.insert(begin(it->second) + i, other); \
			} \
			else { \
				AttrName(object, other); \
			} \
		} \
		if(AttrNameOther(other) != object) { \
			AttrNameOther(other, object); \
		} \
    } \
	/* Remove item */ \
	inline void AttrName(TypeName object, std::function<std::pair<bool, bool> (TypeNameOther value, size_t index)> remover) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			auto& values = it->second; \
			size_t index = 0; \
			for (auto v_it = begin(values); v_it != end(values);) { \
				auto result = remover(*v_it, index); \
				if (result.first) { \
					TypeNameOther other = *v_it; \
					v_it = values.erase(v_it); \
					if(other != ot::nil) { \
						AttrNameOther(other, ot::AttributeDeleter {}); \
					} \
					if (!result.second) { \
						break; \
					} \
				} \
				else { v_it++; } \
				index++; \
			} \
		} \
	} \
	/* Remove */ \
	inline void AttrName(TypeName object, ot::AttributeDeleter) { \
		object -= REMOVE_ALL(AttrName, TypeName, TypeNameOther); \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        attrMap.erase(object.get()); \
	} 

#define ATTR1ofNLink(AttrName, TypeName, TypeNameOther, AttrNameOther) \
	/* Attribute map */ \
	inline std::unordered_map<ot::Type*, TypeNameOther>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot::Type*, TypeNameOther> attrMap; \
        return attrMap; \
	} \
	/* Read */ \
    inline TypeNameOther AttrName(TypeName object) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return ot::nil; \
        } \
    } \
	/* Write */ \
    inline void AttrName(TypeName object, TypeNameOther other) { \
		if (!object) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
			AttrNameOther(it->second, ot::AttributeDeleter {}); \
            it->second = other; \
        } \
        else { \
            attrMap.emplace(object.get(), other); \
			/* Add remover to delete attributes on object descruction */ \
			/* object.get()->removers.emplace_back([](ot::Type * object_ptr) { auto& attrMap = TypeName ##_ ##AttrName(); attrMap.erase(object_ptr); }); */ \
        } \
		bool found = false; \
		auto list = AttrNameOther(other); \
		for(auto value : list) { \
			if(value == object) { \
				found = true; \
				break; \
			} \
		} \
		if(!found) { AttrNameOther(other, object); } \
    } \
	/* Remove */ \
	inline void AttrName(TypeName object, ot::AttributeDeleter) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
		auto other = AttrName(object); \
		attrMap.erase(object.get()); \
		if(other != ot::nil) { \
			AttrNameOther(other, ot::AttributeDeleter {}); \
		} \
	} 

#define REL11(AttrName1, TypeName1, TypeName2, AttrName2) \
	/* Prototypes */ \
	TypeName1 AttrName2(TypeName2); \
	void AttrName2(TypeName2, TypeName1); \
	void AttrName2(TypeName2, ot::AttributeDeleter); \
    ATTR1Link(AttrName1, TypeName1, TypeName2, AttrName2) \
	ATTR1Link(AttrName2, TypeName2, TypeName1, AttrName1) 

#define REL1N(AttrName1, TypeName1, TypeName2, AttrName2) \
	/* Prototypes */ \
	TypeName1 AttrName2(TypeName2); \
	void AttrName2(TypeName2, TypeName1); \
	void AttrName2(TypeName2, ot::AttributeDeleter); \
	ATTRNLink(AttrName1, TypeName1, TypeName2, AttrName2) \
	ATTR1ofNLink(AttrName2, TypeName2, TypeName1, AttrName1) 

#define RELN1(AttrName1, TypeName1, TypeName2, AttrName2) \
	REL1N(AttrName2, TypeName2, TypeName1, AttrName1)

#endif // OPEN_TYPES_HPP
