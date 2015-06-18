#include "EnginePCH.h"
#include "Object.h"
#include "BaseActor.h"

CObject::CObject(IActor *pActor, std::string const &sObjectName) :
	m_pActor(pActor), m_sObjectName(sObjectName)
{
};

void CObject::Init()
{
	auto pModel = m_pActor->GetModelRenderer();
	if (pModel)
	{
		auto pMesh = pModel->GetMeshByMeshName(m_sObjectName);
		if (pMesh)
		{
			m_AABB = pMesh->GetAABB();

			SetTransform(pMesh->GetLocalWorld(), true);

			UpdateTransform();
		}
	}
};

XMMATRIX CObject::GetLocalTransform() const
{
	return GetGlobalTransform() * XMMatrixInverse(nullptr, m_pActor->GetGlobalTransform());
};

void CObject::InterpolateMatrices(float fInterpolation)
{
	//m_mGlobalInterpolated = m_mGlobalCurrent;

	//for (int i = 0; i < 4; i++)
	//	m_mGlobalInterpolated.r[i] = XMVectorLerp(m_mGlobalLast.r[i], m_mGlobalCurrent.r[i], fInterpolation);

	XMVECTOR xvScale1, xvQuat1, xvPos1;
	XMVECTOR xvScale2, xvQuat2, xvPos2;

	XMMatrixDecompose(&xvScale1, &xvQuat1, &xvPos1, m_mGlobalLast);
	XMMatrixDecompose(&xvScale2, &xvQuat2, &xvPos2, m_mGlobalCurrent);

	m_mGlobalInterpolated = XMMatrixRotationQuaternion(XMQuaternionSlerp(xvQuat1, xvQuat2, fInterpolation));
	m_mGlobalInterpolated.r[3] = XMVectorLerp(xvPos1, xvPos2, fInterpolation);
	m_mGlobalInterpolated._44 = 1.0f;
};

void CObject::RecalcRenderMatrix(float fInterpolation)
{
	InterpolateMatrices(fInterpolation);
};

void CObject::UpdateTransform()
{
	XMMATRIX mTransform = XMMatrixRotationQuaternion(XMLoadFloat4(&m_vQuat));
	mTransform.r[3] = XMLoadFloat3(&m_vPos);
	mTransform._44 = 1.0f;
	
	SetTransform(mTransform, false);
};

void CObject::SetTransform(XMMATRIX const &mTransform, bool bRecalcPos)
{
	if (bRecalcPos)
	{
		XMVECTOR xvScale, xvQuat, xvPos;

		if (XMMatrixDecompose(&xvScale, &xvQuat, &xvPos, mTransform))
		{
			SetRotation(xvQuat);
			SetPosition(xvPos);

			m_bIsNeedPhysicsUpdate = true;
		}
	}

	m_mGlobalLast = m_mGlobalCurrent;
	m_mGlobalCurrent = mTransform;
};

bool CObject::Update(bool bIsActorMoved)
{
	if (bIsActorMoved)
	{
		// нужно передвинуть себя так, как передвинулся актёр
		// в начале из мира актёра идём в ноль, а затем обратно в мир уже объекта
		// получается переход из мира актёра в мир объекта
		XMMATRIX mLocalWorld = GetGlobalTransform() * XMMatrixInverse(nullptr, m_pActor->GetPrevGlobalTransform());

		// теперь нужно взять новое положение актёра и получится преобразование верное
		mLocalWorld = mLocalWorld * m_pActor->GetGlobalTransform();

		// обновляем позиции
		SetTransform(mLocalWorld);
	}
	else if (!m_bIsNeedPhysicsUpdate && m_pShape && m_pPhysActor)
	{
		auto &transform = PxShapeExt::getGlobalPose(*m_pShape, *m_pPhysActor);
		m_vPos = transform.p;
		XMStoreFloat4(&m_vQuat, XMQuaternionNormalize(XMLoadFloat4(&(XMFLOAT4)transform.q)));
	}

	UpdateTransform();

	bool bIsNeedPhysicsUpdate = m_bIsNeedPhysicsUpdate;
	m_bIsNeedPhysicsUpdate = false;

	return bIsNeedPhysicsUpdate;
}