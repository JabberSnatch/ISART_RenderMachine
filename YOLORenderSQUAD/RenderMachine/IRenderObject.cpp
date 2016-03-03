#include "IRenderObject.hpp"

#include "Device.hpp"

void
IRenderObject::Attach(Node* _node)
{
	Component::Attach(_node);
	DEVICE->CurrentScene()->AddRenderObject(this);
}

