#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <type_traits>
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

namespace ot {
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
		/*~Type() {
			for (auto remover : removers) {
				remover(this);
			}
		}*/
    };

	/**
	Reference to nullptr
	*/
	static const std::shared_ptr<Type> nil = nullptr;

	/**
	Returns a reference to a new created object
	*/
    inline std::shared_ptr<Type> empty() {
		return std::make_shared<Type>();
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
    struct TypeName : public std::shared_ptr<ot::Type> { \
        TypeName() : std::shared_ptr<ot::Type>() {} \
        TypeName(const std::shared_ptr<ot::Type>& other) : std::shared_ptr<ot::Type>(other){} \
		/* Write attribute  on construction */ \
        template <typename AttrType> \
        TypeName(void (*write)(TypeName, AttrType), AttrType value) \
        : std::shared_ptr<ot::Type>(std::make_shared<ot::Type>()) { \
			write(*this, value); \
		} \
		/* Read attribute value */ \
        template <typename AttrType> \
        AttrType operator[](AttrType (*read)(TypeName)) const { \
			return read(*this); \
		} \
		/* Write attribute / Add value to attribute list */ \
        template <typename AttrType> \
        TypeName& operator()(void (*write)(TypeName, AttrType), AttrType value) { \
			write(*this, value); \
			return *this; \
		} \
		/* Write value to position in attribute list */ \
		template <typename AttrType> \
		TypeName& operator()(void (*writeAt)(TypeName, size_t, AttrType), size_t i, AttrType value) { \
			writeAt(*this, i, value); \
			return *this; \
		} \
		/* Remove attribute value */ \
		void operator-=(void (*remove)(TypeName, ot::AttributeDeleter)) { \
			remove(*this, {}); \
		} \
		/* Remove value from attribute list */ \
		template <typename AttrType> \
		void operator-=(ot::ItemDeleter<TypeName, AttrType> itemDeleter) { \
			itemDeleter.remove(*this, itemDeleter.shouldRemove); \
		} \
    };

#define TEMPLATE_TYPES_ENABLED 0
#if TEMPLATE_TYPES_ENABLED
/* expands to the first argument */

#define FIRST_HELPER(first, ...) first
#define FIRST(...) FIRST_HELPER(__VA_ARGS__, throwaway)

/*
 * if there's only one argument, expands to nothing.  if there is more
 * than one argument, expands to a comma followed by everything but
 * the first argument.  only supports up to 9 arguments but can be
 * trivially expanded.
 */
#define REST(...) REST_HELPER(NUM(__VA_ARGS__), __VA_ARGS__)
#define REST_HELPER(qty, ...) REST_HELPER2(qty, __VA_ARGS__)
#define REST_HELPER2(qty, ...) REST_HELPER_##qty(__VA_ARGS__)
#define REST_HELPER_ONE(first)
#define REST_HELPER_TWOORMORE(first, ...) __VA_ARGS__
#define SELECT_10TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10
#define NUM(...) \
    SELECT_10TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE,\
                TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)

#define TYPENAMES6(...) \
	, typename FIRST(__VA_ARGS__)

#define TYPENAMES5(...) \
	, typename FIRST(__VA_ARGS__) REST(TYPENAMES6(__VA_ARGS__))

#define TYPENAMES4(...) \
	, typename FIRST(__VA_ARGS__) REST(TYPENAMES5(__VA_ARGS__))

#define TYPENAMES3(...) \
	, typename FIRST(__VA_ARGS__) REST(TYPENAMES4(__VA_ARGS__))

#define TYPENAMES2(...) \
	, typename FIRST(__VA_ARGS__) REST(TYPENAMES3(__VA_ARGS__))

#define TYPENAMES(...) \
	typename FIRST(__VA_ARGS__) REST(TYPENAMES2(__VA_ARGS__))


#define TEMPLATE_TYPE(TypeName, ...) \
	template <TYPENAMES(__VA_ARGS__)> \
	struct TypeName : public std::shared_ptr<ot::Type> { \
        TypeName() : std::shared_ptr<ot::Type>() {} \
        TypeName(const std::shared_ptr<ot::Type>& other) : std::shared_ptr<ot::Type>(other){} \
		/* Write attribute  on construction */ \
        template <typename AttrType> \
        TypeName(void (*write)(TypeName, AttrType), AttrType value) \
        : std::shared_ptr<ot::Type>(std::make_shared<ot::Type>()) { \
			write(*this, value); \
		} \
		/* Read attribute value */ \
        template <typename AttrType> \
        AttrType operator[](AttrType (*read)(TypeName)) const { \
			return read(*this); \
		} \
		/* Write attribute / Add value to attribute list */ \
        template <typename AttrType> \
        TypeName& operator()(void (*write)(TypeName, AttrType), AttrType value) { \
			write(*this, value); \
			return *this; \
		} \
		/* Write value to position in attribute list */ \
		template <typename AttrType> \
		TypeName& operator()(void (*writeAt)(TypeName, size_t, AttrType), size_t i, AttrType value) { \
			writeAt(*this, i, value); \
			return *this; \
		} \
		/* Remove attribute value */ \
		void operator-=(void (*remove)(TypeName, ot::AttributeDeleter)) { \
			remove(*this, {}); \
		} \
		/* Remove value from attribute list */ \
		template <typename AttrType> \
		void operator-=(ot::ItemDeleter<TypeName, AttrType> itemDeleter) { \
			itemDeleter.remove(*this, itemDeleter.shouldRemove); \
		} \
    };
#endif

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

#define TEMPLATE_ATTR1(AttrName, TypeName, AttrType, ...) \
	/* Attribute map */ \
	template <__VA_ARGS__> \
    inline std::unordered_map<ot::Type*, AttrType>& TypeNameBase ##_ ##AttrName() { \
        static std::unordered_map<ot::Type*, AttrType> attrMap; \
        return attrMap; \
    } \
	/* Read */ \
	template <__VA_ARGS__> \
    inline AttrType AttrName(TypeName object) { \
        auto& attrMap = TypeNameBase ##_ ##AttrName(); \
        auto it = attrMap.find(object.get()); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
	/* Write */ \
	template <__VA_ARGS__> \
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
	template <__VA_ARGS__> \
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

#define ATTR11Link(AttrName, TypeName, TypeNameOther, AttrNameOther) \
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
			object.get()->removers.emplace_back([](ot::Type * object_ptr) { auto& attrMap = TypeName ##_ ##AttrName(); attrMap.erase(object_ptr); }); \
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

#define ATTRN1Link(AttrName, TypeName, TypeNameOther, AttrNameOther) \
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

#define ATTR1NLink(AttrName, TypeName, TypeNameOther, AttrNameOther) \
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
			object.get()->removers.emplace_back([](ot::Type * object_ptr) { auto& attrMap = TypeName ##_ ##AttrName(); attrMap.erase(object_ptr); }); \
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

#define ATTRNNLink(AttrName, TypeName, TypeNameOther, AttrNameOther) \
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
		bool found = false; \
		auto list = AttrNameOther(other); \
		for(auto value : list) { \
			if(value == object) { \
				found = true; \
				break; \
			} \
		} \
		if(!found) { AttrNameOther(other, i, object); } \
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
					auto list = AttrNameOther(other); \
					for(auto value : list) { \
						if(value == object) { \
							AttrNameOther(other, [object](TypeNameOther value, size_t index) -> std::pair<bool, bool> { \
									return { value == object, true }; \
								} \
							); \
						} \
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

#define REL11(AttrName1, TypeName1, TypeName2, AttrName2) \
	/* Prototypes */ \
	TypeName1 AttrName2(TypeName2); \
	void AttrName2(TypeName2, TypeName1); \
	void AttrName2(TypeName2, ot::AttributeDeleter); \
    ATTR11Link(AttrName1, TypeName1, TypeName2, AttrName2) \
	ATTR11Link(AttrName2, TypeName2, TypeName1, AttrName1) 

#define REL1N(AttrName1, TypeName1, TypeName2, AttrName2) \
	/* Prototypes */ \
	TypeName1 AttrName2(TypeName2); \
	void AttrName2(TypeName2, TypeName1); \
	void AttrName2(TypeName2, ot::AttributeDeleter); \
	ATTRN1Link(AttrName1, TypeName1, TypeName2, AttrName2) \
	ATTR1NLink(AttrName2, TypeName2, TypeName1, AttrName1) 

#define RELN1(AttrName1, TypeName1, TypeName2, AttrName2) \
	REL1N(AttrName2, TypeName2, TypeName1, AttrName1)

#define RELNN(AttrName1, TypeName1, TypeName2, AttrName2) \
	/* Prototypes */ \
	TypeName1 AttrName2(TypeName2); \
	void AttrName2(TypeName2, TypeName1); \
	void AttrName2(TypeName2, size_t, TypeName1); \
	void AttrName2(TypeName2, std::function<std::pair<bool, bool> (TypeName1, size_t)>); \
	ATTRNNLink(AttrName1, TypeName1, TypeName2, AttrName2) \
	ATTRNNLink(AttrName2, TypeName2, TypeName1, AttrName1)

#define REL1(AttrName, TypeName) \
	/* Prototypes */ \
	void AttrName(TypeName, ot::AttributeDeleter); \
	ATTR11Link(AttrName, TypeName, TypeName, AttrName)

#define RELN(AttrName, TypeName) \
	ATTRNNLink(AttrName, TypeName, TypeName, AttrName)

#endif // OPEN_TYPES_HPP
