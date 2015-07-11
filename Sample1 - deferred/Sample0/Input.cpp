#include "CommonManagers.h"

const float Input::moveSpeed = 1.f;
const float Input::rotateSpeed = 30.f;

XMVECTOR Input::camRotation = XMQuaternionIdentity();
XMFLOAT3 Input::camPosition = XMFLOAT3(0.f, 0.f, -5.f);
XMMATRIX* Input::viewMatrix;

bool
Input::wKey = false,
Input::sKey = false,
Input::aKey = false,
Input::dKey = false,
Input::qKey = false,
Input::eKey = false;

LRESULT CALLBACK Input::_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	XMVECTOR up = XMVectorSetY(XMVectorZero(), 1.f);
	XMVECTOR right = XMVectorSetX(XMVectorZero(), 1.f);
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		if (wParam == 0x53)
			sKey = true;
		if (wParam == 0x57)
			wKey = true;
		if (wParam == 0x41)
			aKey = true;
		if (wParam == 0x44)
			dKey = true;
		if (wParam == 0x51)
			qKey = true;
		if (wParam == 0x45)
			eKey = true;
		break;
	case WM_KEYUP:
		if (wParam == 0x53)
			sKey = false;
		if (wParam == 0x57)
			wKey = false;
		if (wParam == 0x41)
			aKey = false;
		if (wParam == 0x44)
			dKey = false;
		if (wParam == 0x51)
			qKey = false;
		if (wParam == 0x45)
			eKey = false;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Input::Update()
{
	static const XMVECTOR up = XMVectorSetY(XMVectorZero(), 1.f);
	static const XMVECTOR right = XMVectorSetX(XMVectorZero(), 1.f);

	XMVECTOR direction = XMVectorZero();
	if (sKey)	direction = XMVectorSetZ(direction, -moveSpeed * Timer::GetDeltaTime());
	if (wKey)	direction = XMVectorSetZ(direction, +moveSpeed * Timer::GetDeltaTime());
	if (aKey)	direction = XMVectorSetX(direction, -moveSpeed * Timer::GetDeltaTime());
	if (dKey)	direction = XMVectorSetX(direction, +moveSpeed * Timer::GetDeltaTime());
	if (eKey)	direction = XMVectorSetY(direction, -moveSpeed * Timer::GetDeltaTime());
	if (qKey)	direction = XMVectorSetY(direction, +moveSpeed * Timer::GetDeltaTime());

	direction = XMVector4Transform(direction, *viewMatrix);
	camPosition.x += XMVectorGetX(direction);
	camPosition.z += XMVectorGetZ(direction);
	camPosition.y += XMVectorGetY(direction);

	/*MSG junkMSG;
	POINT mousePos;

	GetCursorPos(&mousePos);
	ScreenToClient(g_mainWin, &mousePos);

	mousePos.x -= screenWidth / 2;
	mousePos.y -= screenHeight / 2;

	mousePos.x *= rotateSpeed * Timer::GetDeltaTime();
	mousePos.y *= rotateSpeed * Timer::GetDeltaTime();

	camRotation = XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(0.f, mousePos.x, 0.f), camRotation);

	mousePos.x = screenWidth / 2;
	mousePos.y = screenHeight / 2;
	ClientToScreen(g_mainWin, &mousePos);
	SetCursorPos(mousePos.x, mousePos.y);
	PeekMessage(&junkMSG, g_mainWin, 0, 0, PM_REMOVE | PM_NOYIELD);*/

	*viewMatrix = XMMatrixInverse(nullptr, XMMatrixRotationQuaternion(camRotation) * XMMatrixTranslation(camPosition.x, camPosition.y, camPosition.z));
}