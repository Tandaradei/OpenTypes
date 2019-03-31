#pragma once

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>

namespace ot {
	struct AttributeNotFoundException : public std::exception
	{
		AttributeNotFoundException(char const* const Message) 
			: std::exception(Message)
		{}
	};


	std::string fallback_value = "undefined";

	struct BaseType
	{
		virtual void addAttribute(std::string name, std::string value) = 0;
		virtual void removeAttribute(const std::string& name) = 0;
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

	struct AttributeAccessor : public BaseAttribute
	{
		AttributeAccessor(BaseType* type, std::string name, std::string value)
			: type(type),
			BaseAttribute(name, value)
		{
			type->addAttribute(name, value);
		}

		~AttributeAccessor() {
			type->removeAttribute(name);
		}

		BaseType* type;
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


	struct Type : public BaseType
	{
		std::map<std::string, Attribute> attributes;
		std::vector<BaseObject*> instanced_objects;

		virtual void addAttribute(std::string name, std::string value) override {
			attributes.insert({ name, Attribute(name, value) });
			for (auto obj : instanced_objects) {
				obj->attributes.insert({ name, Attribute(name, value) });
			}
		}

		virtual void removeAttribute(const std::string& name) override {
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


	template <typename TypeName>
	struct Object : public BaseObject
	{
		Object(Type* type)
		:	BaseObject(type)
		{
			type->instanced_objects.push_back(this);
			for (auto& attr : type->attributes) {
				attributes.insert({ attr.first, attr.second });
			}
		}

		Object() {
			type->instanced_objects.erase(type->instanced_objects.find(this));
		}

		std::string& operator[](AttributeAccessor& attr) {
			return operator[](attr.name);
		}

		std::string& operator[](const std::string& name) {
			auto attribute = std::find_if(
				attributes.begin(),
				attributes.end(),
				[&name](auto key) {
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

	std::map<std::string, std::unique_ptr<Type>> types;
}

#define OT_TYPE(TypeName) \
	struct TypeName : public ot::Type \
	{ \
		\
	}; \
	ot::types.insert({ #TypeName, std::make_unique<TypeName>() })

#define OT_OBJECT(TypeName) \
	ot::Object<TypeName>(ot::types.find(#TypeName)->second.get())

#define OT_ATTR(TypeName, AttrName, DefaultValue) \
	ot::AttributeAccessor TypeName ##_ ##AttrName = ot::AttributeAccessor(ot::types.find(#TypeName)->second.get(), #AttrName, DefaultValue)