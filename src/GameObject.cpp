#include "GameObject.h"

#include "Engine.h"
#include "GameScene.h"

void GameObject::OnDelete()
{
    GameEngine->GetGameScene().RemoveObject(this->GetEntityID());
}
