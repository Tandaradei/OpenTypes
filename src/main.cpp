#include <iostream>

#include "OpenTypes.h"


int main() {
    OT_TYPE(MyType);
    auto obj = OT_OBJECT(MyType);
    OT_ATTR(MyType, attr, "init");
	auto obj2 = OT_OBJECT(MyType);

	printf("Get attr from obj\n");
	auto value = obj[MyType_attr];
	printf("%s <-- should be 'init'\n", value.c_str());

	printf("Change attr from obj\n");
	obj[MyType_attr] = "new";
	auto value_new = obj["attr"];
	printf("%s <-- should be 'new'\n", value_new.c_str());

	auto value_obj2 = obj2[MyType_attr];
	printf("%s <-- should be 'init'\n", value_obj2.c_str());

	{
		OT_ATTR(MyType, attr2, "init2");
		auto value2 = obj[MyType_attr2];
		printf("%s <-- should be 'init2'\n", value2.c_str());
	}

	try {
		auto value2 = obj["attr2"];
		printf("%s <-- should NOT be shown\n", value2.c_str());
	}
	catch (ot::AttributeNotFoundException e) {
		printf("Exception: %s", e.what());
	}

	std::getchar();

	return 0;
}
