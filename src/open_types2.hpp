#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace ot {
	static const bool IS_DEBUG = false;

	/// Dummy class to differentiate between function declarations
	struct DeleteDefault {
	};

	template <typename T>
	struct DeleteByValue {
		T value;
		explicit DeleteByValue(T v) : value(v) {}
	};

	struct DeleteByIndex {
		size_t index;
		explicit DeleteByIndex(size_t i) : index(i) {}
	};

	struct Type {

	public:
		using Remover = std::function<void(Type*)>;

	public:
		~Type() {
			for (const Remover& remover : removers) {
				remover(this);
			}
		}

	public:
		std::vector<Remover> removers;
	};

	class PtrManager {
	public:
		static PtrManager& getInstance() {
			static PtrManager instance;
			return instance;
		}
		PtrManager(PtrManager const&) = delete;
		void operator=(PtrManager const&) = delete;
		void inc(Type* t) {
			ptrs[t]++;
		}
		void dec(Type* t) {
			if (--ptrs[t] == 0) {
				delete t;
				ptrs.erase(t);
			}
		}

	private:
		PtrManager() {}

	private:
		std::unordered_map<Type*, size_t> ptrs;
	};


	template <typename T>
	struct Reference {
		Reference() 
			: myPtr(nullptr) {

		}

		Reference(T* ptr) 
			: myPtr(ptr) {
			PtrManager::getInstance().inc(myPtr);
		}
		~Reference() {
			PtrManager::getInstance().dec(myPtr);
		}
		Reference(const Reference& other) {
			if (myPtr) {
				PtrManager::getInstance().dec(myPtr);
			}
			myPtr = other.get();
			PtrManager::getInstance().inc(myPtr);
		}

		Reference& operator=(const Reference& other) {
			if (myPtr) {
				PtrManager::getInstance().dec(myPtr);
			}
			myPtr = other.get();
			PtrManager::getInstance().inc(myPtr);
			return *this;
		}

		bool operator==(const Reference& other) const {
			return myPtr == other.get();
		}

		bool operator!=(const Reference& other) const {
			return !(*this == other);
		}

		bool operator==(const T* other) const {
			return myPtr == other;
		}

		bool operator!=(const T* other) const {
			return !(*this == other);
		}

		operator bool() const {
			return myPtr != nullptr;
		}

		T* get() const {
			return myPtr;
		}

		static Reference create() {
			return Reference(new T());
		}

	private:
		T* myPtr = nullptr;
	};

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

	enum class ContainsResult {
		None,
		First,
		Second
	};

	template <typename A, typename B>
	struct LinkedAttributes {
		A* a;
		B* b;
		ContainsResult contains(Type* t) {
			if		((A*)t == a)	return ContainsResult::First;
			else if ((B*)t == b)	return ContainsResult::Second;
			else					return ContainsResult::None;
		}
	};

	/// Each pointer must be present at most once
	template <typename A, typename B>
	struct LinkedAttributesSet {
		std::vector<LinkedAttributes<A, B>> links;

		Type* other(Type* t) {
			if (!t) { return nullptr; }
			for (auto& link : links) {
				ContainsResult result = link.contains(t);
				if (result != ContainsResult::None) {
					if (result == ContainsResult::First) {
						return link.b;
					}
					else if (result == ContainsResult::Second) {
						return link.a;
					}
				}
			}
			return nullptr;
		}

		void set(Type* t, Type* n) {
			if (!t || !n) { return; }
			for (auto& link : links) {
				ContainsResult result = link.contains(t);
				if (result != ContainsResult::None) {
					if (result == ContainsResult::First) {
						link.b = (B*)n;
						n->removers.emplace_back([&](Type* ptr) {
							remove(ptr);
						});
						return;
					}
					else if (result == ContainsResult::Second) {
						link.a = (A*)n;
						n->removers.emplace_back([&](Type* ptr) {
							remove(ptr);
						});
						return;
					}
				}
			}
			links.push_back(LinkedAttributes<A, B>{ (A*)t, (B*)n });
			t->removers.emplace_back([&](Type* ptr) {
				remove(ptr);
			});
			n->removers.emplace_back([&](Type* ptr) {
				remove(ptr);
			});
		}

		void remove(Type* t) {
			if (!t) { return; }
			for (auto it = links.begin(); it != links.end(); it++) {
				ContainsResult result = it->contains(t);
				if (result != ContainsResult::None) {
					links.erase(it);
					break;
				}
			}
		}
	};

	/// Each pointer of B must be present at most once, pointer of A can be multiple times present
	template <typename A, typename B>
	struct LinkedAttributesListSet {
		std::vector<LinkedAttributes<A, B>> links;

		Type* otherSingle(Type* t) {
			if (!t) { return nullptr; }
			for (auto& link : links) {
				ContainsResult result = link.contains(t);
				if (result != ContainsResult::None) {
					if (result == ContainsResult::Second) {
						return link.a;
					}
				}
			}
			return nullptr;
		}
		template <typename Ref>
		vector<Ref> otherList(Type* t) {
			if (!t) { return vector<Ref>(); }
			vector<Ref> list;
			for (auto& link : links) {
				ContainsResult result = link.contains(t);
				if (result != ContainsResult::None) {
					if (result == ContainsResult::First) {
						list.push_back(Ref(link.b));
					}
				}
			}
			return list;
		}

		void setSingle(Type* value, Type* key) {
			if (!key || !value) { return; }
			for (auto& link : links) {
				ContainsResult result = link.contains(value);
				if (result != ContainsResult::None) {
					if (result == ContainsResult::Second) {
						link.a = (A*)key;
						value->removers.emplace_back([&](Type* ptr) {
							remove(ptr);
							});
						return;
					}
				}
			}
			// Add new entry
			links.push_back(LinkedAttributes<A, B>{ (A*)key, (B*)value });
			key->removers.emplace_back([&](Type* ptr) {
				remove(ptr);
				});
			value->removers.emplace_back([&](Type* ptr) {
				remove(ptr);
				});
		}

		void addToList(Type* key, Type* value) {
			if (!key || !value) { return; }
			for (auto& link : links) {
				ContainsResult result = link.contains(value);
				if (result != ContainsResult::None) {
					if (result == ContainsResult::Second) {
						link.a = (A*)key;
						value->removers.emplace_back([&](Type* ptr) {
							remove(ptr);
						});
						return;
					}
				}
			}
			// Add new entry
			links.push_back(LinkedAttributes<A, B>{ (A*)key, (B*)value });
			key->removers.emplace_back([&](Type* ptr) {
				remove(ptr);
				});
			value->removers.emplace_back([&](Type* ptr) {
				remove(ptr);
				});
		}

		void remove(Type* t) {
			if (!t) { return; }
			for (auto it = links.begin(); it != links.end();) {
				ContainsResult result = it->contains(t);
				if (result != ContainsResult::None) {
					it = links.erase(it);
				}
				else {
					it++;
				}
			}
		}
	};

}

#define OT_TYPE(TypeName) \
	namespace ot_types { \
		struct TypeName : public ot::Type { \
			~TypeName(){ if(ot::IS_DEBUG) printf("DEBUG: Destructor of ot_types::%s called\n", #TypeName);} \
			/* Read attribute value */ \
			template <typename AttrType> \
			AttrType operator[](AttrType (*read)(TypeName*)) { \
				return read(this); \
			} \
			/* Write attribute / Add value to attribute list */ \
			template <typename AttrType> \
            TypeName& operator()(void (*write)(TypeName*, AttrType), AttrType value) { \
				write(this, value); \
                return *this; \
			} \
			/* Write value to position in attribute list */ \
			template <typename AttrType> \
            TypeName& operator()(void (*writeAt)(TypeName*, size_t, AttrType), size_t i, AttrType value) { \
				writeAt(this, i, value); \
                return *this; \
			} \
			/* Remove attribute value / entire list */ \
			void operator-=(void (*remove)(TypeName*, ot::DeleteDefault)) { \
				remove(this, {}); \
			} \
			/*  Remove all elements with value from attribute list */ \
			template <typename AttrType> \
			void deleteByValue(void (*removeFromList)(TypeName*, ot::DeleteByValue<AttrType>), ot::DeleteByValue<AttrType> deleter) { \
				removeFromList(this, deleter); \
			} \
			/*  Remove value at index from attribute list */ \
			template <typename AttrType> \
			void deleteByIndex(void (*removeFromList)(TypeName*, ot::DeleteByIndex), ot::DeleteByIndex deleter) { \
				removeFromList(this, deleter); \
			} \
		}; \
	} \
	struct TypeName : public ot::Reference<ot_types::TypeName> { \
		static TypeName create() { return ot::Reference<ot_types::TypeName>::create(); } \
		static TypeName nil() { return ot::Reference<ot_types::TypeName>(nullptr); } \
		TypeName() : ot::Reference<ot_types::TypeName>() {} \
		TypeName(ot_types::TypeName* other) : ot::Reference<ot_types::TypeName>(other){} \
		TypeName(const ot::Reference<ot_types::TypeName>& other) : ot::Reference<ot_types::TypeName>(other){} \
		~TypeName(){ if(ot::IS_DEBUG) printf("DEBUG: Destructor of %s called\n", #TypeName);} \
		/* Write attribute on construction */ \
		template <typename AttrType> \
		TypeName(void (*write)(TypeName&, AttrType), AttrType value) \
		: ot::Reference<ot_types::TypeName>(ot::Reference<ot_types::TypeName>::create()) { \
			write(*this, value); \
		} \
		/* Read attribute value */ \
        template <typename AttrType> \
        AttrType operator[](AttrType (*read)(const TypeName&)) const { \
			return read(*this); \
		} \
		/* Write attribute / Add value to attribute list */ \
        template <typename AttrType> \
        TypeName& operator()(void (*write)(TypeName&, AttrType), AttrType value) { \
			write(*this, value); \
			return *this; \
		} \
		/* Write value to position in attribute list */ \
		template <typename AttrType> \
		TypeName& operator()(void (*writeAt)(TypeName&, size_t, AttrType), size_t i, AttrType value) { \
			writeAt(*this, i, value); \
			return *this; \
		} \
		/* Remove attribute value / entire list */ \
		void operator-=(void (*remove)(TypeName&, ot::DeleteDefault)) { \
			remove(*this, {}); \
		} \
		/*  Remove all elements with value from attribute list */ \
		template <typename AttrType> \
		void deleteByValue(void (*removeFromList)(TypeName&, ot::DeleteByValue<AttrType>), ot::DeleteByValue<AttrType> deleter) { \
			removeFromList(*this, deleter); \
		} \
		/*  Remove value at index from attribute list */ \
		template <typename AttrType> \
		void deleteByIndex(void (*removeFromList)(TypeName&, ot::DeleteByIndex), ot::DeleteByIndex deleter) { \
			removeFromList(*this, deleter); \
		} \
	}; \

#define OT_ATTR1(AttrName, TypeName, AttrType) \
	/* Attribute map */ \
    inline std::unordered_map<ot_types::TypeName*, AttrType>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot_types::TypeName*, AttrType> attrMap; \
        return attrMap; \
    } \
	/* Read */ \
    inline AttrType AttrName(ot_types::TypeName* ptr) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
    inline AttrType AttrName(const TypeName& object) { \
       return AttrName(object.get()); \
	} \
	/* Write */ \
	inline void AttrName(ot_types::TypeName* ptr, AttrType value) { \
		if (!ptr) { return; } /* Don't write on nullptr objects */ \
		auto& attrMap = TypeName ##_ ##AttrName(); \
		auto it = attrMap.find(ptr); \
		if (it != attrMap.cend()) { \
			it->second = std::move(value); \
		} \
		else { \
			attrMap.emplace(ptr, std::move(value)); \
			/* Add remover to delete attributes on object descruction */ \
			ptr->removers.emplace_back([](ot::Type* ptr) { \
				if(ot::IS_DEBUG) printf("DEBUG: Remover for %s::%s called\n", #TypeName, #AttrName); \
				auto& attrMap = TypeName ##_ ##AttrName(); \
				attrMap.erase((ot_types::TypeName*)ptr); \
			}); \
		} \
    } \
	inline void AttrName(TypeName& object, AttrType valueWithLongNameToAvoidNameCollisions) { \
		AttrName(object.get(), valueWithLongNameToAvoidNameCollisions); \
	} \
	/* Remove */ \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        attrMap.erase(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault deleter) { \
		AttrName(object.get(), deleter); \
	} 

#define OT_ATTRN(AttrName, TypeName, AttrType) \
	/* Attribute map */ \
    inline std::unordered_map<ot_types::TypeName*, ot::vector<AttrType>>& TypeName ##_ ##AttrName() { \
        static std::unordered_map<ot_types::TypeName*, ot::vector<AttrType>> attrMap; \
        return attrMap; \
    } \
	/* Read complete list */ \
    inline ot::vector<AttrType> AttrName(ot_types::TypeName* ptr) { \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			return it->second; \
        } \
        else { \
            return ot::vector<AttrType>(); \
        } \
    } \
	inline ot::vector<AttrType> AttrName(const TypeName& object) { \
		return AttrName(object.get()); \
	} \
	/* Add item at end */ \
    inline void AttrName(ot_types::TypeName* ptr, AttrType value) { \
		if (!ptr) { return; } /* Don't write on nil objects */ \
		auto& attrMap = TypeName ##_ ##AttrName(); \
		auto it = attrMap.find(ptr); \
		if (it == attrMap.cend()) { \
			it = attrMap.emplace(ptr, ot::vector<AttrType>()).first; \
			/* Add remover to delete attributes on object descruction */ \
			ptr->removers.emplace_back([](ot::Type* ptr){ \
				if(ot::IS_DEBUG) printf("DEBUG: List-Remover for %s::%s called\n", #TypeName, #AttrName); \
				auto& attrMap = TypeName ##_ ##AttrName(); \
				attrMap.erase((ot_types::TypeName*)ptr); \
			}); \
		} \
		it->second.emplace_back(std::move(value)); \
	} \
	inline void AttrName(TypeName& object, AttrType valueWithLongNameToAvoidNameCollisions) { \
		AttrName(object.get(), valueWithLongNameToAvoidNameCollisions); \
	} \
	/* Add item at position */ \
	inline void AttrName(ot_types::TypeName* ptr, size_t i, AttrType value) { \
		if (!ptr) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			if(it->second.size() > i) { \
				it->second.insert(begin(it->second) + i, std::move(value)); \
			} \
			else { \
				AttrName(ptr, value); /* Add item at end */ \
			} \
		} \
		else { \
			AttrName(ptr, value);  /* Create list and add item at end */ \
		} \
	} \
	inline void AttrName(TypeName& object, size_t i, AttrType valueWithLongNameToAvoidNameCollisions) { \
		AttrName(object.get(), i, valueWithLongNameToAvoidNameCollisions); \
	} \
	/* Remove entire list */ \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
		attrMap.erase(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault deleter) { \
		AttrName(object.get(), deleter); \
	} \
	/* Remove items by value */ \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteByValue<AttrType> deleter) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			auto& values = it->second; \
			for (auto v_it = begin(values); v_it != end(values);) { \
				if (*v_it == deleter.value) { \
					v_it = values.erase(v_it); \
				} \
				else { v_it++; } \
			} \
		} \
	} \
	inline void AttrName(TypeName& object, ot::DeleteByValue<AttrType> deleter) { \
		AttrName(object.get(), deleter); \
	} \
	/* Remove item by index */ \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteByIndex deleter) { \
		auto& attrMap = TypeName ##_ ##AttrName(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			auto& values = it->second; \
			if (values.size() > deleter.index) { \
				values.erase(begin(values) + deleter.index); \
			} \
		} \
	} \
	inline void AttrName(TypeName& object, ot::DeleteByIndex deleter) { \
		AttrName(object.get(), deleter); \
	}

// *** Templated Types and Attributes ***

#define COMMA ,

#define OT_TEMPLATE_TYPE(TypeName, TemplateArgsName, TemplateArgs) \
	namespace ot_types { \
		template <TemplateArgs> \
		struct TypeName : public ot::Type { \
			/* Read attribute value */ \
			template <typename AttrType> \
			AttrType operator[](AttrType (*read)(TypeName*)) { \
				return read(this); \
			} \
			/* Write attribute / Add value to attribute list */ \
			template <typename AttrType> \
			TypeName* operator()(void (*write)(TypeName*, AttrType), AttrType value) { \
				write(this, value); \
				return this; \
			} \
			/* Write value to position in attribute list */ \
			template <typename AttrType> \
			TypeName* operator()(void (*writeAt)(TypeName*, size_t, AttrType), size_t i, AttrType value) { \
				writeAt(this, i, value); \
				return this; \
			} \
			/* Remove attribute value / entire list */ \
			void operator-=(void (*remove)(TypeName*, ot::DeleteDefault)) { \
				remove(this, {}); \
			} \
			/*  Remove all elements with value from attribute list */ \
			template <typename AttrType> \
			void deleteByValue(void (*removeFromList)(TypeName*, ot::DeleteByValue<AttrType>), ot::DeleteByValue<AttrType> deleter) { \
				removeFromList(this, deleter); \
			} \
			/*  Remove value at index from attribute list */ \
			template <typename AttrType> \
			void deleteByIndex(void (*removeFromList)(TypeName*, ot::DeleteByIndex), ot::DeleteByIndex deleter) { \
				removeFromList(this, deleter); \
			} \
		}; \
	} \
	template <TemplateArgs> \
	struct TypeName : public ot::Reference<ot_types::TypeName<TemplateArgsName>> { \
		static TypeName create() { return ot::Reference<ot_types::TypeName<TemplateArgsName>>::create(); } \
		TypeName() : ot::Reference<ot_types::TypeName<TemplateArgsName>>() {} \
        TypeName(const ot::Reference<ot_types::TypeName<TemplateArgsName>>& other) : ot::Reference<ot_types::TypeName<TemplateArgsName>>(other){} \
		/* Write attribute on construction */ \
		template <typename AttrType> \
		TypeName(void (*write)(TypeName&, AttrType), AttrType value) \
		: ot::Reference<ot_types::TypeName<TemplateArgsName>>(ot::Reference<ot_types::TypeName<TemplateArgsName>>::create()) { \
			write(*this, value); \
		} \
		/* Read attribute value */ \
        template <typename AttrType> \
        AttrType operator[](AttrType (*read)(const TypeName&)) const { \
			return read(*this); \
		} \
		/* Write attribute / Add value to attribute list */ \
        template <typename AttrType> \
        TypeName& operator()(void (*write)(TypeName&, AttrType), AttrType value) { \
			write(*this, value); \
			return *this; \
		} \
		/* Write value to position in attribute list */ \
		template <typename AttrType> \
		TypeName& operator()(void (*writeAt)(TypeName&, size_t, AttrType), size_t i, AttrType value) { \
			writeAt(*this, i, value); \
			return *this; \
		} \
		/* Remove attribute value / entire list */ \
		void operator-=(void (*remove)(TypeName&, ot::DeleteDefault)) { \
			remove(*this, {}); \
		} \
		/*  Remove all elements with value from attribute list */ \
		template <typename AttrType> \
		void deleteByValue(void (*removeFromList)(TypeName&, ot::DeleteByValue<AttrType>), ot::DeleteByValue<AttrType> deleter) { \
			removeFromList(*this, deleter); \
		} \
		/*  Remove value at index from attribute list */ \
		template <typename AttrType> \
		void deleteByIndex(void (*removeFromList)(TypeName&, ot::DeleteByIndex), ot::DeleteByIndex deleter) { \
			removeFromList(*this, deleter); \
		} \
	};

#define OT_TEMPLATE_ATTR1(AttrName, TypeNameBase, TemplateArgsNames, AttrType, TemplateArgs) \
	/* Attribute map */ \
	template <TemplateArgs> \
    inline std::unordered_map<ot_types::TypeNameBase<TemplateArgsNames>*, AttrType>& TypeNameBase ##_ ##AttrName() { \
        static std::unordered_map<ot_types::TypeNameBase<TemplateArgsNames>*, AttrType> attrMap; \
        return attrMap; \
    } \
	/* Read */ \
	template <TemplateArgs> \
    inline AttrType AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr) { \
        auto& attrMap = TypeNameBase ##_ ##AttrName<TemplateArgsNames>(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return AttrType(); \
        } \
    } \
	template <TemplateArgs> \
    inline AttrType AttrName(const TypeNameBase<TemplateArgsNames>& object) { \
        return AttrName(object.get()); \
	} \
	/* Write */ \
	template <TemplateArgs> \
	inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, AttrType value) {\
		if (!ptr) { return; } /* Don't write on nil objects */ \
		auto& attrMap = TypeNameBase ##_ ##AttrName<TemplateArgsNames>(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
            it->second = std::move(value); \
        } \
        else { \
            attrMap.emplace(ptr, std::move(value)); \
			/* Add remover to delete attributes on object descruction */ \
			ptr->removers.emplace_back([](ot::Type* ptr){ \
				if(ot::IS_DEBUG) printf("DEBUG: Remover for %s<%s>::%s called\n", #TypeNameBase, #TemplateArgsNames, #AttrName); \
				auto& attrMap = TypeNameBase ##_ ##AttrName<TemplateArgsNames>(); \
				attrMap.erase((ot_types::TypeNameBase<TemplateArgsNames>*)ptr); \
			}); \
        } \
    } \
	template <TemplateArgs> \
    inline void AttrName(TypeNameBase<TemplateArgsNames>& object, AttrType valueWithLongNameToAvoidNameCollisions) { \
		AttrName(object.get(), valueWithLongNameToAvoidNameCollisions); \
	} \
	/* Remove */ \
	template <TemplateArgs> \
	inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, ot::DeleteDefault) { \
		auto& attrMap = TypeNameBase ##_ ##AttrName<TemplateArgsNames>(); \
        attrMap.erase(ptr); \
    } \
	template <TemplateArgs> \
	inline void AttrName(TypeNameBase<TemplateArgsNames>& object, ot::DeleteDefault) { \
		AttrName(object.get(), {}); \
	}

#define OT_TEMPLATE_ATTRN(AttrName, TypeNameBase, TemplateArgsNames, AttrType, TemplateArgs) \
	/* Attribute map */ \
	template <TemplateArgs> \
    inline std::unordered_map<ot_types::TypeNameBase<TemplateArgsNames>*, ot::vector<AttrType>>& TypeNameBase ##_ ##AttrName() { \
        static std::unordered_map<ot_types::TypeNameBase<TemplateArgsNames>*, ot::vector<AttrType>> attrMap; \
        return attrMap; \
    } \
	/* Read */ \
	template <TemplateArgs> \
    inline ot::vector<AttrType> AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr) { \
        auto& attrMap = TypeNameBase ##_ ##AttrName<TemplateArgsNames>(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
            return it->second; \
        } \
        else { \
            return ot::vector<AttrType>(); \
        } \
    } \
	template <TemplateArgs> \
    inline ot::vector<AttrType> AttrName(const TypeNameBase<TemplateArgsNames>& object) { \
        return AttrName(object.get()); \
	} \
	/* Add item at end */ \
	template <TemplateArgs> \
    inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, AttrType value) { \
		if (!ptr) { return; } /* Don't write on nil objects */ \
		auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
		auto it = attrMap.find(ptr); \
		if (it == attrMap.cend()) { \
			it = attrMap.emplace(ptr, ot::vector<AttrType>()).first; \
			/* Add remover to delete attributes on object descruction */ \
			ptr->removers.emplace_back([](ot::Type* ptr){ \
				if(ot::IS_DEBUG) printf("DEBUG: List-Remover for %s<%s>::%s called\n", #TypeNameBase, #TemplateArgsNames, #AttrName); \
				auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
				attrMap.erase((ot_types::TypeNameBase<TemplateArgsNames>*)ptr); \
			}); \
		} \
		it->second.emplace_back(std::move(value)); \
	} \
	template <TemplateArgs> \
	inline void AttrName(TypeNameBase<TemplateArgsNames> object, AttrType valueWithLongNameToAvoidNameCollisions) { \
		AttrName(object.get(), valueWithLongNameToAvoidNameCollisions); \
	} \
	/* Add item at position */ \
	template <TemplateArgs> \
	inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, size_t i, AttrType value) { \
		if (!ptr) { return; } /* Don't write on nil objects */ \
        auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			if(it->second.size() > i) { \
				it->second.insert(begin(it->second) + i, std::move(value)); \
			} \
			else { \
				AttrName(ptr, value); /* Add item at end */ \
			} \
		} \
		else { \
			AttrName(ptr, value);  /* Create list and add item at end */ \
		} \
	} \
	template <TemplateArgs> \
	inline void AttrName(TypeNameBase<TemplateArgsNames>& object, size_t i, AttrType valueWithLongNameToAvoidNameCollisions) { \
		AttrName(object.get(), i, valueWithLongNameToAvoidNameCollisions); \
	} \
	/* Remove entire list */ \
	template <TemplateArgs> \
	inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, ot::DeleteDefault) { \
		auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
		attrMap.erase(ptr); \
	} \
	template <TemplateArgs> \
	inline void AttrName(TypeNameBase<TemplateArgsNames>& object, ot::DeleteDefault deleter) { \
		AttrName(object.get(), deleter); \
	} \
	/* Remove items by value */ \
	template <TemplateArgs> \
	inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, ot::DeleteByValue<AttrType> deleter) { \
		auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			auto& values = it->second; \
			for (auto v_it = begin(values); v_it != end(values);) { \
				if (*v_it == deleter.value) { \
					v_it = values.erase(v_it); \
				} \
				else { v_it++; } \
			} \
		} \
	} \
	template <TemplateArgs> \
	inline void AttrName(TypeNameBase<TemplateArgsNames>& object, ot::DeleteByValue<AttrType> deleter) { \
		AttrName(object.get(), deleter); \
	} \
	/* Remove item by index */ \
	template <TemplateArgs> \
	inline void AttrName(ot_types::TypeNameBase<TemplateArgsNames>* ptr, ot::DeleteByIndex deleter) { \
		auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
        auto it = attrMap.find(ptr); \
        if (it != attrMap.cend()) { \
			auto& values = it->second; \
			if (values.size() > deleter.index) { \
				values.erase(begin(values) + deleter.index); \
			} \
		} \
	} \
	template <TemplateArgs> \
	inline void AttrName(TypeNameBase<TemplateArgsNames>& object, ot::DeleteByIndex deleter) { \
		AttrName(object.get(), deleter); \
	}

#define OT_REL11_READ(AttrName, TypeName, TypeNameOther, ListName) \
	inline TypeNameOther AttrName(ot_types::TypeName* ptr) { \
		auto& set = ListName(); \
		return (ot_types::TypeNameOther*)set.other(ptr); \
	} \
	inline TypeNameOther AttrName(const TypeName& object) { \
		return AttrName(object.get()); \
	} 

#define OT_REL11_WRITE(AttrName, TypeName, TypeNameOther, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot_types::TypeNameOther* value) { \
		auto& set = ListName(); \
		set.set((ot::Type*)ptr, (ot::Type*)value); \
	} \
	inline void AttrName(TypeName& object, TypeNameOther value) { \
		AttrName(object.get(), value.get()); \
	} \

#define OT_REL11_REMOVE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& set = ListName(); \
		set.remove(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), ot::DeleteDefault{}); \
	} \

#define OT_REL11_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, ListName) \
	inline ot::LinkedAttributesSet<ot_types::TypeName1, ot_types::TypeName2>& ListName() { \
		static ot::LinkedAttributesSet<ot_types::TypeName1, ot_types::TypeName2> set; \
		return set; \
	} \
	/* Read */ \
	OT_REL11_READ(AttrName1, TypeName1, TypeName2, ListName) \
	OT_REL11_READ(AttrName2, TypeName2, TypeName1, ListName) \
	/* Write */ \
	OT_REL11_WRITE(AttrName1, TypeName1, TypeName2, ListName) \
	OT_REL11_WRITE(AttrName2, TypeName2, TypeName1, ListName) \
	/* Remove */ \
	OT_REL11_REMOVE(AttrName1, TypeName1, ListName) \
	OT_REL11_REMOVE(AttrName2, TypeName2, ListName) \

#define OT_REL11(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_REL11_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, AttrName1 ##_ ##TypeName1 ##_ ##TypeName2 ##_ ##AttrName2)

#define OT_REL1N_REMOVE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& set = ListName(); \
		set.remove(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), {}); \
	} \

#define OT_REL1N_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, ListName) \
	inline ot::LinkedAttributesListSet<ot_types::TypeName1, ot_types::TypeName2>& ListName() { \
		static ot::LinkedAttributesListSet<ot_types::TypeName1, ot_types::TypeName2> list; \
		return list; \
	} \
	/* Read */ \
	inline ot::vector<TypeName2> AttrName1(ot_types::TypeName1* ptr) { \
		auto& list = ListName(); \
		return list.otherList<TypeName2>(ptr); \
	} \
	inline ot::vector<TypeName2> AttrName1(const TypeName1& object) { \
		return AttrName1(object.get()); \
	} \
	inline TypeName1 AttrName2(ot_types::TypeName2* ptr) { \
		auto& list = ListName(); \
		return (ot_types::TypeName1*)list.otherSingle(ptr); \
	} \
	inline TypeName1 AttrName2(const TypeName2& object) { \
		return AttrName2(object.get()); \
	} \
	/* Write */ \
	inline void AttrName1(ot_types::TypeName1* ptr, ot_types::TypeName2* value) { \
		auto& list = ListName(); \
		list.addToList(ptr, value); \
	} \
	inline void AttrName1(TypeName1& object, TypeName2 value) { \
		AttrName1(object.get(), value.get()); \
	} \
	inline void AttrName2(ot_types::TypeName2* ptr, ot_types::TypeName1* value) { \
		auto& list = ListName(); \
		list.setSingle(ptr, value); \
	} \
	inline void AttrName2(TypeName2& object, TypeName1 value) { \
		AttrName2(object.get(), value.get()); \
	} \
	/* Remove */ \
	OT_REL1N_REMOVE(AttrName1, TypeName1, ListName) \
	OT_REL1N_REMOVE(AttrName2, TypeName2, ListName) \

#define OT_REL1N(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_REL1N_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, AttrName1 ##_ ##TypeName1 ##_ ##TypeName2 ##_ ##AttrName2)

#define OT_RELN1(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_REL1N_IMPL(AttrName2, TypeName2, TypeName1, AttrName1, AttrName2 ##_ ##TypeName2 ##_ ##TypeName1 ##_ ##AttrName1)


#endif // OPEN_TYPES_HPP
