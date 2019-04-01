#pragma once

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>

namespace ot {
    struct AttributeNotFoundException : public std::exception
    {
        AttributeNotFoundException(std::string message)
            : m_message(std::move(message))
        {}

#ifdef __GLIBCXX__
        virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override {
            return m_message.c_str();
        }
#else
        virtual const char* what() const override {
            return m_message.c_str();
        }
#endif


    private:
        std::string m_message;
    };

    struct BaseAttribute
    {
        BaseAttribute(std::string name, std::string value)
            : name(name),
            value(value)
        {}

        std::string name;
        std::string value;
    };

    struct Attribute : public BaseAttribute
    {
        Attribute(std::string name, std::string value)
            : BaseAttribute(name, value) {
        }
    };

    struct Type;

    struct BaseObject
    {
        BaseObject(Type* type)
            : type(type) {
        }
        Type* type;
        std::map<std::string, Attribute> attributes;
    };


    struct Type
    {
        std::map<std::string, Attribute> attributes;
        std::vector<BaseObject*> instanced_objects;

        void registerObject(BaseObject* obj) {
            instanced_objects.push_back(obj);
            for (auto& attr : attributes) {
                obj->attributes.insert({ attr.first, attr.second });
            }
        }

        void unregisterObject(BaseObject* obj) {
            instanced_objects.erase(
                std::find(
                    instanced_objects.begin(),
                    instanced_objects.end(),
                    obj
                )
            );
        }

        void addAttribute(std::string name, std::string value) {
            attributes.insert({ name, Attribute(name, value) });
            for (auto obj : instanced_objects) {
                obj->attributes.insert({ name, Attribute(name, value) });
            }
        }

        void removeAttribute(const std::string& name) {
            auto it = attributes.find(name);
            if (it != attributes.end()) {
                attributes.erase(it);
            }
            for (auto obj : instanced_objects) {
                auto obj_it = obj->attributes.find(name);
                if (obj_it != obj->attributes.end()) {
                    obj->attributes.erase(obj_it);
                }
            }
        }
    };

	struct AttributeAccessor : public BaseAttribute
	{
		AttributeAccessor(Type* type, std::string name, std::string value)
			: BaseAttribute(name, value),
			type(type) {
			type->addAttribute(name, value);
		}

		~AttributeAccessor() {
			type->removeAttribute(name);
		}

		Type* type;
	};


    template <typename TypeName>
    struct Object : public BaseObject
    {
        Object(Type* type)
        :	BaseObject(type)
        {
            type->registerObject(this);
        }

        Object() {
            type->unregisterObject(this);
        }

        std::string& operator[](AttributeAccessor& attr) {
            return operator[](attr.name);
        }

        std::string& operator[](const std::string& name) {
            auto attribute = std::find_if(
                attributes.begin(),
                attributes.end(),
                [&name](const auto& key) {
                    return key.first == name;
                }
            );
            if (attribute != attributes.end()) {
                return attribute->second.value;
            }
            else {
                throw AttributeNotFoundException(std::string("Attribute " + name + " not found\n").c_str());
            }
        }
    };

    static std::map<std::string, std::unique_ptr<Type>> types;
}

#define OT_TYPE(TypeName) \
    struct TypeName : public ot::Type \
    { \
        virtual ~TypeName(){}\
    }; \
    ot::types.insert({ #TypeName, std::make_unique<TypeName>() })

#define OT_OBJECT(TypeName) \
    ot::Object<TypeName>(ot::types.find(#TypeName)->second.get())

#define OT_ATTR(TypeName, AttrName, DefaultValue) \
    ot::AttributeAccessor TypeName ##_ ##AttrName = ot::AttributeAccessor(ot::types.find(#TypeName)->second.get(), #AttrName, DefaultValue)
