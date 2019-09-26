#ifndef OPEN_TYPES_HPP
#define OPEN_TYPES_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>


namespace ot {
    static const bool SHOULD_PRINT_DEBUG = false;
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
			if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Called ~Type for %p\n", this);
			for (auto& remover : removers) {
				remover.second(this);
			}
		}

	public:
		std::unordered_map<std::string, Remover> removers;
	};

	class PtrManager {
	public:
		static PtrManager& getInstance() {
			static PtrManager instance;
			return instance;
		}
		PtrManager(PtrManager const&) = delete;
		void operator=(PtrManager const&) = delete;

		size_t inc(Type* t) {
			return ++ptrs[t];
		}

		size_t dec(Type* t) {
			size_t count = --ptrs[t];
			if (count == 0) {
				ptrs.erase(t);
			}
			return count;
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
			if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Created new nil reference\n");
		}

		Reference(T* ptr) 
            : myPtr(ptr) {
            if(myPtr) {
				if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Created new reference to %p from raw pointer\n", myPtr);
                PtrManager::getInstance().inc(myPtr);
            }
		}
		~Reference() {
			decAndDelete();
		}
		Reference(const Reference& other) {
			decAndDelete();
			myPtr = other.get();
			if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Created new reference to %p from other Reference\n", myPtr);
            if (myPtr) {
                PtrManager::getInstance().inc(myPtr);
            }
		}

		Reference& operator=(const Reference& other) {
			decAndDelete();
			myPtr = other.get();
			if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Set reference to %p\n", myPtr);
            if (myPtr) {
                PtrManager::getInstance().inc(myPtr);
            }
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
			if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Called Reference::create\n");
			return Reference(new T());
		}

	private:
		void decAndDelete() {
			if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Called decAndDelete for %p\n", myPtr);
			if (myPtr) {
				if (PtrManager::getInstance().dec(myPtr) == 0) {
					if (ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Delete %p\n", myPtr);
					delete myPtr;
					myPtr = nullptr;
				}
			}
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

	template <typename T>
	class LinkedAttributes11Mirrored {
	private:
		std::unordered_map<T*, T*> connections;

	public:
		T* get(T* t) {
			if (!t) { return nullptr; }
			auto it = connections.find(t);
			if (it != connections.end()) { return it->second; };
			return nullptr;
		}

		void set(T* a, T* b) {
			if (!a || !b) { return; }
			if (connections[a] == b) { return; }
			// if a has a connection, remove the connection from the connected b
			auto it_a = connections.find(a);
			if (it_a != connections.end()) { 
				connections.erase(it_a->second); 
			};
			// set new b
			connections[a] = b;
			// if b has a connection, remove the connection from the connected a
			auto it_b = connections.find(b);
			if (it_b != connections.end()) { 
				connections.erase(it_b->second); 
			};
			connections[b] = a;
		}

		void remove(T* t) {
			if (!t) { return; }
			auto it = connections.find(t);
			if (it != connections.end()) { connections.erase(it->second); };
			connections.erase(t);
		}
	};

	template <typename T>
	class LinkedAttributesNNMirrored
	{
	private:
		std::unordered_map<T*, ot::vector<T*>> connections;

	public:
		ot::vector<T*> getList(T* t) {
			if (!t) { return ot::vector<T*>(); }
			auto it = connections.find(t);
			if (it != connections.end()) { return it->second; };
			return ot::vector<T*>();
		}

		void setInList(T* a, T* b) {
			if (!a || !b) { return; }

			if (connections.find(a) == connections.end()) {
				connections[a] = ot::vector<T*>();
			};
			auto& vec_a = connections[a];
			auto it_b_in_a = std::find(vec_a.begin(), vec_a.end(), b);
			if (it_b_in_a == vec_a.end()) {
				vec_a.push_back(b);
			}

			if (connections.find(b) == connections.end()) {
				connections[b] = ot::vector<T*>();
			};
			auto& vec_b = connections[b];
			auto it_a_in_b = std::find(vec_b.begin(), vec_b.end(), a);
			if (it_a_in_b == vec_b.end()) {
				vec_b.push_back(a);
			}
		}

		void removeByValue(T* a, T* b) {
			if (!a || !b) { return; }
			auto removeByValueInt = [&](T* t1, T* t2) {
				auto it = connections.find(t1);
				if (it != connections.end()) {
					auto& vec = it->second;
					vec.erase(std::remove(vec.begin(), vec.end(), t2));
					if (vec.size() == 0) {
						connections.erase(t1);
					}
				}
			};
			removeByValueInt(a, b);
			removeByValueInt(b, a);
		}

		void remove(T* t) {
			if (!t) { return; }
			auto it = connections.find(t);
			if (it != connections.end()) { 
				for (auto b : it->second) {
					auto& vec = connections[b];
					vec.erase(std::remove(vec.begin(), vec.end(), t));
					if (vec.size() == 0) {
						connections.erase(b);
					}
				}
				connections.erase(t);
			};
		}
	};

	template <typename A, typename B>
	class LinkedAttributes11 {
	private:
		std::unordered_map<A*, B*> a_to_b;
		std::unordered_map<B*, A*> b_to_a;

	public:
		B* getA(A* a) {
			if (!a) { return nullptr; }
			auto it = a_to_b.find(a);
			if (it != a_to_b.end()) { return it->second; };
			return nullptr;
		}

		A* getB(B* b) {
			if (!b) { return nullptr; }
			auto it = b_to_a.find(b);
			if (it != b_to_a.end()) { return it->second; };
			return nullptr;
		}

		void setA(A* a, B* b) {
			if (!a || !b) { return; }
			if (a_to_b[a] == b) { return; }
			// if a has a connection, remove the connection from the connected b
			auto it_a = a_to_b.find(a);
			if (it_a != a_to_b.end()) { b_to_a.erase(it_a->second); };
			// set new b
			a_to_b[a] = b;
			// if b has a connection, remove the connection from the connected a
			auto it_b = b_to_a.find(b);
			if (it_b != b_to_a.end()) { a_to_b.erase(it_b->second); };
			b_to_a[b] = a;
		}

		void setB(B* b, A* a) {
			setA(a, b);
		}

		void removeA(A* a) {
			// if a has a connection, remove the connection from the connected b
			auto it_a = a_to_b.find(a);
			if (it_a != a_to_b.end()) { b_to_a.erase(it_a->second); };
			a_to_b.erase(a);
		}

		void removeB(B* b) {
			// if b has a connection, remove the connection from the connected a
			auto it_b = b_to_a.find(b);
			if (it_b != b_to_a.end()) { a_to_b.erase(it_b->second); };
			b_to_a.erase(b);
		}
	};

	template <typename A, typename B>
	class LinkedAttributes1N {
	private:
		std::unordered_map<A*, ot::vector<B*>> a_to_b;
		std::unordered_map<B*, A*> b_to_a;

	public:
		ot::vector<B*> getList(A* a) {
			if (!a) { return ot::vector<B*>(); }
			auto it = a_to_b.find(a);
			if (it != a_to_b.end()) { return it->second; };
			return ot::vector<B*>();
		}

		A* getSingle(B* b) {
			if (!b) { return nullptr; }
			auto it = b_to_a.find(b);
			if (it != b_to_a.end()) { return it->second; };
			return nullptr;
		}

		void setInList(A* a, B* b) {
			if (!a || !b) { return; }
			// Removes b from old list, and set it's connection to a
			auto it_b = b_to_a.find(b);
			if (it_b != b_to_a.end()) {
				auto& vec = a_to_b[it_b->second];
				vec.erase(std::remove(vec.begin(), vec.end(), b));
				if (vec.size() == 0) {
					a_to_b.erase(it_b->second);
				}
			};
			b_to_a[b] = a;

			if (a_to_b.find(a) == a_to_b.end()) { 
				// Create new vector and initialize with b
				a_to_b[a] = ot::vector<B*>();
			}
			a_to_b[a].push_back(b);
		}

		void setSingle(B* b, A* a) {
			setInList(a, b);
		}

		void removeA(A* a) {
			if (!a) { return;  }
			auto it_a = a_to_b.find(a);
			if (it_a != a_to_b.end()) {
				for (B* b : it_a->second) {
					b_to_a.erase(b);
				}
				a_to_b.erase(a);
			}
		}

		void removeFromAByValue(A* a, B* b) {
			if (!a || !b) { return; }
			auto it_a = a_to_b.find(a);
			if (it_a != a_to_b.end()) {
				auto& vec = it_a->second;
				for (auto it = vec.begin(); it != vec.end();) {
					if (b == *it) {
						it = vec.erase(it);
						b_to_a.erase(b);
					}
					else {
						it++;
					}
				}
				if (vec.size() == 0) {
					a_to_b.erase(a);
				}
			}
		}

		void removeB(B* b) {
			if (!b) { return; }
			auto it_b = b_to_a.find(b);
			if (it_b != b_to_a.end()) {
				auto& vec = a_to_b[it_b->second];
				vec.erase(std::remove(vec.begin(), vec.end(), b));
				if (vec.size() == 0) {
					a_to_b.erase(it_b->second);
				}
				b_to_a.erase(b);
			};
		}
	};

	template <typename A, typename B>
	class LinkedAttributesNN {
	private:
		std::unordered_map<A*, ot::vector<B*>> a_to_b;
		std::unordered_map<B*, ot::vector<A*>> b_to_a;

	public:
		ot::vector<B*> getListA(A* a) {
			if (!a) { return ot::vector<B*>(); }
			auto it = a_to_b.find(a);
			if (it != a_to_b.end()) { return it->second; }
			return ot::vector<B*>();
		}

		ot::vector<A*> getListB(B* a) {
			if (!a) { return ot::vector<A*>(); }
			auto it = b_to_a.find(a);
			if (it != b_to_a.end()) { return it->second; }
			return ot::vector<A*>();
		}

		void setInListA(A* a, B* b) {
			if (!a || !b) { return; }

			if (b_to_a.find(b) == b_to_a.end()) {
				b_to_a[b] = ot::vector<A*>();
			};
			b_to_a[b].push_back(a);

			if (a_to_b.find(a) == a_to_b.end()) {
				a_to_b[a] = ot::vector<B*>();
			}
			a_to_b[a].push_back(b);
		}

		void setInListB(B* b, A* a) {
			setInListA(a, b);
		}

		void removeFromAByValue(A* a, B* b) {
			if (!a || !b) { return; }
			auto it_a = a_to_b.find(a);
			if (it_a != a_to_b.end()) {
				auto& vec = it_a->second;
				for (auto it = vec.begin(); it != vec.end();) {
					if (b == *it) {
						it = vec.erase(it);
						auto it_b = b_to_a.find(b);
						if (it_b != b_to_a.end()) {
							auto& vec_b = it_b->second;
							vec_b.erase(std::remove(vec_b.begin(), vec_b.end(), a));
							if (vec_b.size() == 0) {
								b_to_a.erase(b);
							}
						}
					}
					else {
						it++;
					}
				}
				if (vec.size() == 0) {
					a_to_b.erase(a);
				}
			}
		}

		void removeFromBByValue(B* a, A* b) {
			if (!a || !b) { return; }
			auto it_b = b_to_a.find(b);
			if (it_b != b_to_a.end()) {
				auto& vec = it_b->second;
				for (auto it = vec.begin(); it != vec.end();) {
					if (a == *it) {
						it = vec.erase(it);
						auto it_a = a_to_b.find(a);
						if (it_a != a_to_b.end()) {
							auto& vec_a = it_a->second;
							vec_a.erase(std::remove(vec_a.begin(), vec_a.end(), b));
							if (vec_a.size() == 0) {
								a_to_b.erase(b);
							}
						}
					}
					else {
						it++;
					}
				}
				if (vec.size() == 0) {
					b_to_a.erase(b);
				}
			}
		}

		void removeA(A* a) {
			if (!a) { return; }
			auto it_a = a_to_b.find(a);
			if (it_a != a_to_b.end()) {
				for (B* b : it_a->second) {
					auto& vec = b_to_a[b];
					vec.erase(std::remove(vec.begin(), vec.end(), a));
					if (vec.size() == 0) {
						b_to_a.erase(b);
					}
				}
				a_to_b.erase(a);
			}
		}

		void removeB(B* b) {
			if (!b) { return; }
			auto it_b = b_to_a.find(b);
			if (it_b != b_to_a.end()) {
				for (A* a : it_b->second) {
					auto& vec = a_to_b[a];
					vec.erase(std::remove(vec.begin(), vec.end(), b));
					if (vec.size() == 0) {
						a_to_b.erase(a);
					}
				}
				b_to_a.erase(b);
			}
		}
	};
}

#define OT_TYPE(TypeName) \
	namespace ot_types { \
		struct TypeName : public ot::Type { \
            ~TypeName(){ if(ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Destructor of ot_types::%s called!\n", #TypeName);} \
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
        ~TypeName(){ if(ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Destructor of %s called\n", #TypeName);} \
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
			ptr->removers[#AttrName] = [](ot::Type* ptr) { \
                if(ot::SHOULD_PRINT_DEBUG) printf("DEBUG: Remover for %s::%s called\n", #TypeName, #AttrName); \
				auto& attrMap = TypeName ##_ ##AttrName(); \
                attrMap.erase(static_cast<ot_types::TypeName*>(ptr)); \
			}; \
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
			ptr->removers[#AttrName] = [](ot::Type* ptr){ \
                if(ot::SHOULD_PRINT_DEBUG) printf("DEBUG: List-Remover for %s::%s called\n", #TypeName, #AttrName); \
				auto& attrMap = TypeName ##_ ##AttrName(); \
                attrMap.erase(static_cast<ot_types::TypeName*>(ptr)); \
			}; \
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
                it->second.insert(begin(it->second) + static_cast<long>(i), std::move(value)); \
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
                values.erase(begin(values) + static_cast<long>(deleter.index)); \
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
			ptr->removers[#AttrName] = [](ot::Type* ptr){ \
				auto& attrMap = TypeNameBase ##_ ##AttrName<TemplateArgsNames>(); \
                attrMap.erase(static_cast<ot_types::TypeNameBase<TemplateArgsNames>*>(ptr)); \
			}; \
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
			ptr->removers[#AttrName] = [](ot::Type* ptr){ \
                if(ot::SHOULD_PRINT_DEBUG) printf("DEBUG: List-Remover for %s<%s>::%s called\n", #TypeNameBase, #TemplateArgsNames, #AttrName); \
				auto& attrMap = TypeName ##_ ##AttrName<TemplateArgsNames>(); \
                attrMap.erase(static_cast<ot_types::TypeNameBase<TemplateArgsNames>*>(ptr)); \
			}; \
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

#define OT_REL11_READ(AttrName, TypeName, TypeNameOther, ListName, A_Or_B) \
	inline ot_types::TypeNameOther* AttrName(ot_types::TypeName* ptr) { \
		auto& set = ListName(); \
        return set.get ##A_Or_B(ptr); \
	} \
	inline TypeNameOther AttrName(const TypeName& object) { \
		return AttrName(object.get()); \
	} 

#define OT_REL11_WRITE(AttrName, TypeName, TypeNameOther, ListName, A_Or_B) \
	inline void AttrName(ot_types::TypeName* ptr, ot_types::TypeNameOther* value) { \
		auto& set = ListName(); \
        set.set ##A_Or_B(ptr, value); \
	} \
	inline void AttrName(TypeName& object, TypeNameOther value) { \
		AttrName(object.get(), value.get()); \
	} \

#define OT_REL11_REMOVE(AttrName, TypeName, ListName, A_Or_B) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& set = ListName(); \
		set.remove ##A_Or_B(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), ot::DeleteDefault{}); \
	} \

#define OT_REL11_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, ListName) \
	inline ot::LinkedAttributes11<ot_types::TypeName1, ot_types::TypeName2>& ListName() { \
		static ot::LinkedAttributes11<ot_types::TypeName1, ot_types::TypeName2> set; \
		return set; \
	} \
	/* Read */ \
	OT_REL11_READ(AttrName1, TypeName1, TypeName2, ListName, A) \
	OT_REL11_READ(AttrName2, TypeName2, TypeName1, ListName, B) \
	/* Write */ \
	OT_REL11_WRITE(AttrName1, TypeName1, TypeName2, ListName, A) \
	OT_REL11_WRITE(AttrName2, TypeName2, TypeName1, ListName, B) \
	/* Remove */ \
	OT_REL11_REMOVE(AttrName1, TypeName1, ListName, A) \
	OT_REL11_REMOVE(AttrName2, TypeName2, ListName, B) \

#define OT_REL11(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_REL11_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, AttrName1 ##_ ##TypeName1 ##_ ##TypeName2 ##_ ##AttrName2)

#define OT_REL1N_REMOVE(AttrName, TypeName, ListName, A_Or_B) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& list = ListName(); \
		list.remove ##A_Or_B(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), {}); \
	} \

#define OT_REL1N_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, ListName) \
	inline ot::LinkedAttributes1N<ot_types::TypeName1, ot_types::TypeName2>& ListName() { \
		static ot::LinkedAttributes1N<ot_types::TypeName1, ot_types::TypeName2> list; \
		return list; \
	} \
	/* Read */ \
	inline ot::vector<ot_types::TypeName2*> AttrName1(ot_types::TypeName1* ptr) { \
		auto& list = ListName(); \
		return list.getList(ptr); \
	} \
	inline ot::vector<TypeName2> AttrName1(const TypeName1& object) { \
		ot::vector<TypeName2> vec; \
		auto ptrVec = AttrName1(object.get()); \
		for(auto ptr : ptrVec) { vec.push_back(TypeName2(ptr)); } \
		return vec; \
	} \
	inline ot_types::TypeName1* AttrName2(ot_types::TypeName2* ptr) { \
		auto& list = ListName(); \
        return list.getSingle(ptr); \
	} \
	inline TypeName1 AttrName2(const TypeName2& object) { \
		return AttrName2(object.get()); \
	} \
	/* Write */ \
	inline void AttrName1(ot_types::TypeName1* ptr, ot_types::TypeName2* value) { \
		auto& list = ListName(); \
		list.setInList(ptr, value); \
		ptr->removers[#AttrName2] = [](ot::Type* t) { \
			auto& list = ListName(); \
			list.removeA(static_cast<ot_types::TypeName1*>(t)); \
		}; \
		value->removers[#AttrName1] = [](ot::Type* t) { \
			auto& list = ListName(); \
			list.removeB(static_cast<ot_types::TypeName2*>(t)); \
		}; \
	} \
	inline void AttrName1(TypeName1& object, TypeName2 value) { \
		AttrName1(object.get(), value.get()); \
	} \
	inline void AttrName2(ot_types::TypeName2* ptr, ot_types::TypeName1* value) { \
		auto& list = ListName(); \
		list.setSingle(ptr, value); \
		ptr->removers[#AttrName1] = [](ot::Type* t) { \
			auto& list = ListName(); \
			list.removeB(static_cast<ot_types::TypeName2*>(t)); \
		}; \
		value->removers[#AttrName2] = [](ot::Type* t) { \
			auto& list = ListName(); \
			list.removeA(static_cast<ot_types::TypeName1*>(t)); \
		}; \
	} \
	inline void AttrName2(TypeName2& object, TypeName1 value) { \
		AttrName2(object.get(), value.get()); \
	} \
	/* Remove item by value */ \
	inline void AttrName1(ot_types::TypeName1* ptr, ot::DeleteByValue<ot_types::TypeName2*> deleter) { \
		auto& list = ListName(); \
        list.removeFromAByValue(ptr, deleter.value); \
	} \
	inline void AttrName1(TypeName1& object, ot::DeleteByValue<TypeName2> deleter) { \
		AttrName1(object.get(), ot::DeleteByValue<ot_types::TypeName2*>(deleter.value.get())); \
	} \
	/* Remove */ \
	OT_REL1N_REMOVE(AttrName1, TypeName1, ListName, A) \
	OT_REL1N_REMOVE(AttrName2, TypeName2, ListName, B) \

#define OT_REL1N(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_REL1N_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, AttrName1 ##_ ##TypeName1 ##_ ##TypeName2 ##_ ##AttrName2)

#define OT_RELN1(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_REL1N_IMPL(AttrName2, TypeName2, TypeName1, AttrName1, AttrName2 ##_ ##TypeName2 ##_ ##TypeName1 ##_ ##AttrName1)

#define OT_RELNN_READ(AttrName, TypeName, TypeNameOther, ListName, A_Or_B) \
	inline ot::vector<ot_types::TypeNameOther*> AttrName(ot_types::TypeName* ptr) { \
		auto& list = ListName(); \
        return list.getList ##A_Or_B(ptr); \
	} \
	inline ot::vector<TypeNameOther> AttrName(const TypeName& object) { \
		ot::vector<TypeNameOther> vec; \
		auto ptrVec = AttrName(object.get()); \
		for(auto ptr : ptrVec) { vec.push_back(TypeNameOther(ptr)); } \
		return vec; \
	} 

#define OT_RELNN_WRITE(AttrName, TypeName, TypeNameOther, ListName, A_Or_B) \
	inline void AttrName(ot_types::TypeName* ptr, ot_types::TypeNameOther* value) { \
		auto& list = ListName(); \
        list.setInList ##A_Or_B(ptr, value); \
	} \
	inline void AttrName(TypeName& object, TypeNameOther value) { \
		AttrName(object.get(), value.get()); \
	} \

#define OT_RELNN_REMOVE_VALUE(AttrName, TypeName, TypeNameOther, ListName, A_Or_B) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteByValue<ot_types::TypeNameOther*> deleter) { \
			auto& list = ListName(); \
			list.removeFrom ##A_Or_B ##ByValue(ptr, deleter.value); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteByValue<TypeNameOther> deleter) { \
			AttrName(object.get(), ot::DeleteByValue<ot_types::TypeNameOther*>(deleter.value.get())); \
	} \

#define OT_RELNN_REMOVE(AttrName, TypeName, ListName, A_Or_B) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& list = ListName(); \
		list.remove ##A_Or_B(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), ot::DeleteDefault{}); \
	} \

#define OT_RELNN_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, ListName) \
	inline ot::LinkedAttributesNN<ot_types::TypeName1, ot_types::TypeName2>& ListName() { \
		static ot::LinkedAttributesNN<ot_types::TypeName1, ot_types::TypeName2> set; \
		return set; \
	} \
	/* Read */ \
	OT_RELNN_READ(AttrName1, TypeName1, TypeName2, ListName, A) \
	OT_RELNN_READ(AttrName2, TypeName2, TypeName1, ListName, B) \
	/* Write */ \
	OT_RELNN_WRITE(AttrName1, TypeName1, TypeName2, ListName, A) \
	OT_RELNN_WRITE(AttrName2, TypeName2, TypeName1, ListName, B) \
	/* Remove item by value */ \
	OT_RELNN_REMOVE_VALUE(AttrName1, TypeName1, TypeName2, ListName, A) \
	OT_RELNN_REMOVE_VALUE(AttrName2, TypeName2, TypeName1, ListName, B) \
	/* Remove */ \
	OT_RELNN_REMOVE(AttrName1, TypeName1, ListName, A) \
	OT_RELNN_REMOVE(AttrName2, TypeName2, ListName, B) \

#define OT_RELNN(AttrName1, TypeName1, TypeName2, AttrName2) \
	OT_RELNN_IMPL(AttrName1, TypeName1, TypeName2, AttrName2, AttrName1 ##_ ##TypeName1 ##_ ##TypeName2 ##_ ##AttrName2)

#define OT_REL1_READ(AttrName, TypeName, ListName) \
	inline ot_types::TypeName* AttrName(ot_types::TypeName* ptr) { \
		auto& set = ListName(); \
        return set.get(ptr); \
	} \
	inline TypeName AttrName(const TypeName& object) { \
		return AttrName(object.get()); \
	} 

#define OT_REL1_WRITE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot_types::TypeName* value) { \
		auto& set = ListName(); \
        set.set(ptr, value); \
	} \
	inline void AttrName(TypeName& object, TypeName value) { \
		AttrName(object.get(), value.get()); \
	} \

#define OT_REL1_REMOVE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& set = ListName(); \
		set.remove(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), ot::DeleteDefault{}); \
	} \

#define OT_REL1_IMPL(AttrName, TypeName, ListName) \
	inline ot::LinkedAttributes11Mirrored<ot_types::TypeName>& ListName() { \
		static ot::LinkedAttributes11Mirrored<ot_types::TypeName> set; \
		return set; \
	} \
	/* Read */ \
	OT_REL1_READ(AttrName, TypeName, ListName) \
	/* Write */ \
	OT_REL1_WRITE(AttrName, TypeName, ListName) \
	/* Remove */ \
	OT_REL1_REMOVE(AttrName, TypeName, ListName) \

#define OT_REL1(AttrName, TypeName) \
	OT_REL1_IMPL(AttrName, TypeName, AttrName ##_ ##TypeName ##_ ##TypeName ##_ ##AttrName)

#define OT_RELN_READ(AttrName, TypeName, ListName) \
	inline ot::vector<ot_types::TypeName*> AttrName(ot_types::TypeName* ptr) { \
		auto& list = ListName(); \
        return list.getList(ptr); \
	} \
	inline ot::vector<TypeName> AttrName(const TypeName& object) { \
		ot::vector<TypeName> vec; \
		auto ptrVec = AttrName(object.get()); \
		for(auto ptr : ptrVec) { vec.push_back(TypeName(ptr)); } \
		return vec; \
	} 

#define OT_RELN_WRITE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot_types::TypeName* value) { \
		auto& list = ListName(); \
        list.setInList(ptr, value); \
	} \
	inline void AttrName(TypeName& object, TypeName value) { \
		AttrName(object.get(), value.get()); \
	} \

#define OT_RELN_REMOVE_VALUE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteByValue<ot_types::TypeName*> deleter) { \
		auto& list = ListName(); \
		list.removeByValue(ptr, deleter.value); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteByValue<TypeName> deleter) { \
		AttrName(object.get(), ot::DeleteByValue<ot_types::TypeName*>(deleter.value.get())); \
	} \

#define OT_RELN_REMOVE(AttrName, TypeName, ListName) \
	inline void AttrName(ot_types::TypeName* ptr, ot::DeleteDefault) { \
		auto& list = ListName(); \
		list.remove(ptr); \
	} \
	inline void AttrName(TypeName& object, ot::DeleteDefault) { \
		AttrName(object.get(), ot::DeleteDefault{}); \
	} \

#define OT_RELN_IMPL(AttrName, TypeName, ListName) \
	inline ot::LinkedAttributesNNMirrored<ot_types::TypeName>& ListName() { \
		static ot::LinkedAttributesNNMirrored<ot_types::TypeName> list; \
		return list; \
	} \
	/* Read */ \
	OT_RELN_READ(AttrName, TypeName, ListName) \
	/* Write */ \
	OT_RELN_WRITE(AttrName, TypeName, ListName) \
	/* Remove item by value */ \
	OT_RELN_REMOVE_VALUE(AttrName, TypeName, ListName) \
	/* Remove */ \
	OT_RELN_REMOVE(AttrName, TypeName, ListName) \

#define OT_RELN(AttrName, TypeName) \
	OT_RELN_IMPL(AttrName, TypeName, AttrName ##_ ##TypeName ##_ ##TypeName ##_ ##AttrName)

#endif // OPEN_TYPES_HPP
