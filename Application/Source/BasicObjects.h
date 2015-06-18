#pragma once

std::vector<IActor *> &GetActorsDynamic();
std::vector<IActor *> &GetActorsStatic();

IActor *AddStaticObject(XMFLOAT3 const &vPos, XMCOLOR const &Color, E_ACTOR_TYPE eActorType, E_RIGID_BODY_FLAG const eBodyFlag);
IActor *AddStaticObject(XMVECTOR const &xvPos, XMCOLOR const &Color, E_ACTOR_TYPE eActorType, E_RIGID_BODY_FLAG const eBodyFlag);
IActor *AddDynamicObject(XMFLOAT3 const &vPos, XMCOLOR const &Color, E_ACTOR_TYPE eActorType, E_RIGID_BODY_FLAG const eBodyFlag);
IActor *AddDynamicObject(XMVECTOR const &xvPos, XMCOLOR const &Color, E_ACTOR_TYPE eActorType, E_RIGID_BODY_FLAG const eBodyFlag);