#include "Light.hpp"

#include "Device.hpp"

void
Light::Attach(Node* _node)
{
	Component::Attach(_node);
	DEVICE->CurrentScene()->AddLight(this);
}
