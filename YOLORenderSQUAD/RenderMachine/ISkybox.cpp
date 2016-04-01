#include "ISkybox.hpp"

#include "Device.hpp"


void
ISkybox::Attach(Node* _node)
{
	Component::Attach(_node); 
	DEVICE->CurrentScene()->SetSkybox(this);
}