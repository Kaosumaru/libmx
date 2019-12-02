#include "ScriptableActor.h"
#include "utils/Time.h"
#include <iostream>
using namespace MX;

void ScriptableActor::Run()
{
    auto guard = Context<ScriptableActor>::Lock(this);
    onRun();
    Actor::Run();
}

void ScriptableActor::Unlink()
{
    auto guard = Context<ScriptableActor>::Lock(this);
    if (linked())
        onUnlink();
    Actor::Unlink();
}

void ScriptableActor::OnLinkedToScene()
{
    auto guard = Context<ScriptableActor>::Lock(this);
    onLink();
    Actor::OnLinkedToScene();
}