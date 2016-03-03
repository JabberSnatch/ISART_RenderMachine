#include "IDynamicObject.hpp"

#include "Device.hpp"

void
IDynamicObject::Attach(Node* _node)
{
	Component::Attach(_node);
	DEVICE->CurrentScene()->AddDynamicObject(this);
}

