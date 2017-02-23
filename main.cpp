//csgo d3d 0.2 by n7

#include "main.h" //less important stuff & helper funcs here

typedef HRESULT(APIENTRY *SetStreamSource)(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
HRESULT APIENTRY SetStreamSource_hook(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
SetStreamSource SetStreamSource_orig = 0;

typedef HRESULT(APIENTRY *SetVertexShaderConstantF)(IDirect3DDevice9*, UINT, const float*, UINT);
HRESULT APIENTRY SetVertexShaderConstantF_hook(IDirect3DDevice9*, UINT, const float*, UINT);
SetVertexShaderConstantF SetVertexShaderConstantF_orig = 0;

typedef HRESULT(APIENTRY *DrawIndexedPrimitive)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
HRESULT APIENTRY DrawIndexedPrimitive_hook(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
DrawIndexedPrimitive DrawIndexedPrimitive_orig = 0;

typedef HRESULT(APIENTRY *SetTexture)(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
HRESULT APIENTRY SetTexture_hook(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
SetTexture SetTexture_orig = 0;

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
HRESULT APIENTRY EndScene_hook(IDirect3DDevice9*);
EndScene EndScene_orig = 0;

typedef HRESULT(APIENTRY *Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT APIENTRY Reset_hook(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset Reset_orig = 0;

typedef HRESULT(APIENTRY *Clear)(IDirect3DDevice9*, DWORD, const D3DRECT *, DWORD, D3DCOLOR, float, DWORD);
HRESULT APIENTRY Clear_hook(IDirect3DDevice9*, DWORD, const D3DRECT *, DWORD, D3DCOLOR, float, DWORD);
Clear Clear_orig = 0;

//==========================================================================================================================

HRESULT APIENTRY SetStreamSource_hook(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sStride)
{
	if (StreamNumber == 0)
	{
		Stride = sStride;

		//if (Stride == 32 && pStreamData)
		//{
			//pStreamData->GetDesc(&vdesc);
		//}
	}

	return SetStreamSource_orig(pDevice, StreamNumber, pStreamData, OffsetInBytes, sStride);
}

//=====================================================================================================================

HRESULT APIENTRY SetVertexShaderConstantF_hook(LPDIRECT3DDEVICE9 pDevice, UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	if (pConstantData != NULL)
	{
		//pConstantDataFloat = (float*)pConstantData;

		mStartRegister = StartRegister;
		mVector4fCount = Vector4fCount;
	}

	return SetVertexShaderConstantF_orig(pDevice, StartRegister, pConstantData, Vector4fCount);
}

//==========================================================================================================================

HRESULT APIENTRY DrawIndexedPrimitive_hook(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	//grab numelements
	if (pDevice->GetVertexDeclaration(&pDecl)== D3D_OK)
	pDecl->GetDeclaration(decl, &numElements);
	if (pDecl != NULL) { pDecl->Release(); pDecl = NULL; }

	//get psdata
	//if (SUCCEEDED(pDevice->GetPixelShader(&pShader)))
	//if (pShader != NULL)
	//if (SUCCEEDED(pShader->GetFunction(NULL, &pSize)))
	//if (pShader != NULL) { pShader->Release(); pShader = NULL; }

	//get vsdata
	//if (SUCCEEDED(pDevice->GetVertexShader(&vShader)))
	//if (vShader != NULL)
	//if (SUCCEEDED(vShader->GetFunction(NULL, &vSize)))
	//if (vShader != NULL) { vShader->Release(); vShader = NULL; }

	//wallhack
	if((wallhack == 1) && (CT_Models||T_Models)) //models
	{
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

		DrawIndexedPrimitive_orig(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	//get ct_legs for aimbot
	if ((aimbot == 1 && CT_LEGS && decl->Type == 2)&&(numElements == 9 || numElements == 12)) //filter out fake or wrong entities
	{
		AddLegAim(pDevice, 1);
	}
	//get t_legs for aimbot
	if ((aimbot == 2 && T_LEGS && decl->Type == 2 )&& (numElements == 9 || numElements == 12)) //filter out fake or wrong entities
	{
		AddLegAim(pDevice, 2);
	}
	//get ct_hands for aimbot & use centered aim at CT
	if (aimbot == 1 && CT_HANDS)
	{
		AddHandAim(pDevice, 1);
		AddCenteredAim(pDevice, 1);
	}
	//get t_hands for aimbot & use centered aim at T
	if (aimbot == 2 && T_HANDS)
	{
		AddHandAim(pDevice, 2);
		AddCenteredAim(pDevice, 2);
	}

	//test w2s on new models
	//if (new_sas_head || new_sas_eyes || new_sas_chest || new_sas_legs)
	//if(new_sas_legs)
		//AddAim(pDevice, 1);
	/*
	//logger
	//hold down P key until a texture changes, press I to log values of those textures
	if (GetAsyncKeyState('O') & 1) //-
		countnum--;
	if (GetAsyncKeyState('P') & 1) //+
		countnum++;
	if ((GetAsyncKeyState(VK_MENU)) && (GetAsyncKeyState('9') & 1)) //reset, set to -1
		countnum = -1;
	if (countnum == NumVertices/10)
		if ((Stride > NULL) && (GetAsyncKeyState('I') & 1)) //press I to log to log.txt
			//Log("Stride == %d && NumVertices == %d && primCount == %d", Stride, NumVertices, primCount);
			Log("Stride == %d && NumVertices == %d && primCount == %d && decl->Type == %d && numElements == %d && mStartRegister == %d && mVector4fCount == %d", Stride, NumVertices, primCount, decl->Type, numElements, mStartRegister, mVector4fCount);
			//Log("texCRC == %x && Stride == %d && NumVertices == %d && primCount == %d && decl->Type == %d && numElements == %d && vSize == %d && pSize == %d && mStartRegister == %d && mVector4fCount == %d && vdesc.Size == %d && vdesc.Type == %d && mStage == %d", texCRC, Stride, NumVertices, primCount, decl->Type, numElements, vSize, pSize, mStartRegister, mVector4fCount, vdesc.Size, vdesc.Type, mStage);
	if (countnum == NumVertices/10)
	{
		pDevice->SetPixelShader(NULL);
		return D3D_OK; //delete texture
	}
	*/

	return DrawIndexedPrimitive_orig(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

//==========================================================================================================================

HRESULT APIENTRY SetTexture_hook(IDirect3DDevice9* pDevice, DWORD Sampler, IDirect3DBaseTexture9 *pTexture)
{
	if (pDevice == nullptr) return SetTexture_orig(pDevice, Sampler, pTexture);

	//mStage = Sampler;
	
	return SetTexture_orig(pDevice, Sampler, pTexture);
}

//==========================================================================================================================

HRESULT APIENTRY Clear_hook(IDirect3DDevice9* pDevice, DWORD Count, const D3DRECT  *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	//sprite
	//PreClear(pDevice);

	return Clear_orig(pDevice, Count, pRects, Flags, Color, Z, Stencil);
}

//==========================================================================================================================

bool DoInit = true;
HRESULT APIENTRY EndScene_hook(IDirect3DDevice9* pDevice)
{
	//sprite
	//PreClear(pDevice);

	pDevice->GetViewport(&Viewport); //get viewport
	ScreenCenterX = Viewport.Width / 2.0f;
	ScreenCenterY = Viewport.Height / 2.0f;

	if (DoInit)
	{
		LoadCfg();

		//wallhack = Load("wallhack", "wallhack", wallhack, GetDirectoryFile("palaconfig.ini"));

		DoInit = false;
	}

	if (pFont == NULL)
	{
		HRESULT hr = D3DXCreateFont(pDevice, 14, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pFont);

		if (FAILED(hr)) {
			//Log("D3DXCreateFont failed");
		}
	}

	if (pFont)
	{
		Drawmenu(pDevice);
	}

	if (pFont == NULL)
	{
		HRESULT hr = D3DXCreateFont(pDevice, 14, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &pFont);

		if (FAILED(hr)) {
			//Log("D3DXCreateFont failed");
		}
	}

	//Shift|RMouse|LMouse|Ctrl|Alt|Space|X|C
	if (aimkey == 0) Daimkey = 0;
	if (aimkey == 1) Daimkey = VK_SHIFT;
	if (aimkey == 2) Daimkey = VK_RBUTTON;
	if (aimkey == 3) Daimkey = VK_LBUTTON;
	if (aimkey == 4) Daimkey = VK_CONTROL;
	if (aimkey == 5) Daimkey = VK_MENU;
	if (aimkey == 6) Daimkey = VK_SPACE;
	if (aimkey == 7) Daimkey = 0x58; //X
	if (aimkey == 8) Daimkey = 0x43; //C
	
	//esp 
	if (esp == 1 && AimCenteredInfo.size() != NULL)
	{
		for (unsigned int i = 0; i < AimCenteredInfo.size(); i++)
		{
			if (AimCenteredInfo[i].iTeam == 1 && AimCenteredInfo[i].vOutX > 1 && AimCenteredInfo[i].vOutY > 1)
			{
				//drawpoint on CT
				DrawPoint(pDevice, (int)AimCenteredInfo[i].vOutX, (int)AimCenteredInfo[i].vOutY, 6, 6, 0xFF00FF00);
				//drawpic
				//PrePresent(pDevice, (int)AimCenteredInfo[i].vOutX - 32, (int)AimCenteredInfo[i].vOutY - 20);
			}
			if (AimCenteredInfo[i].iTeam == 2 && AimCenteredInfo[i].vOutX > 1 && AimCenteredInfo[i].vOutY > 1)
			{
				//drawpoint on T
				DrawPoint(pDevice, (int)AimCenteredInfo[i].vOutX, (int)AimCenteredInfo[i].vOutY, 8, 8, 0xFFFF0000);
				//drawpic
				//PrePresent(pDevice, (int)AimCenteredInfo[i].vOutX - 32, (int)AimCenteredInfo[i].vOutY - 20);
			}
		}
	}
	
	double DistLegX, DistLegY;
	//get legs
	if (aimbot > 0 && AimLegInfo.size() != NULL && GetAsyncKeyState(Daimkey) & 0x8000)
	{
		UINT BestTarget = -1;
		DOUBLE fClosestPos = 99999;

		for (unsigned int i = 0; i < AimLegInfo.size(); i++)
		{
			//test w2s
			//if (logger)
			//DrawStrin(pFont, (int)AimLegInfo[i].vOutX, (int)AimLegInfo[i].vOutY, D3DCOLOR_ARGB(255, 0, 255, 0), "o");
			//DrawStrin(pFont, (int)AimLegInfo[i].vOutX, (int)AimLegInfo[i].vOutY, Green, "%.f", (float)Daimkey);

			//aimfov
			float radiusx = (aimfov*2.5f) * (ScreenCenterX / 100);
			float radiusy = (aimfov*2.5f) * (ScreenCenterY / 100);

			if (aimfov == 0)
			{
				radiusx = 2.0f * (ScreenCenterX / 100);
				radiusy = 2.0f * (ScreenCenterY / 100);
			}

			//get crosshairdistance
			AimLegInfo[i].CrosshairDistance = GetDistance(AimLegInfo[i].vOutX, AimLegInfo[i].vOutY, ScreenCenterX, ScreenCenterY);

			//aim at team 1 or 2 (not needed)
			if (aimbot == AimLegInfo[i].iTeam)

			//if in fov
			if (AimLegInfo[i].vOutX >= ScreenCenterX - radiusx && AimLegInfo[i].vOutX <= ScreenCenterX + radiusx && AimLegInfo[i].vOutY >= ScreenCenterY - radiusy && AimLegInfo[i].vOutY <= ScreenCenterY + radiusy)

				//get closest/nearest target to crosshair
				if (AimLegInfo[i].CrosshairDistance < fClosestPos)
				{
					fClosestPos = AimLegInfo[i].CrosshairDistance;
					BestTarget = i;
				}
		}


		//if nearest target to crosshair
		if (BestTarget != -1)
		{
			DistLegX = AimLegInfo[BestTarget].vOutX - ScreenCenterX;
			DistLegY = AimLegInfo[BestTarget].vOutY - ScreenCenterY;
		}
	}
	AimLegInfo.clear();
	
	double DistHandX, DistHandY;
	//get hands
	if (aimbot > 0 && AimHandInfo.size() != NULL && GetAsyncKeyState(Daimkey) & 0x8000)
	{
		UINT BestTarget = -1;
		DOUBLE fClosestPos = 99999;

		for (unsigned int i = 0; i < AimHandInfo.size(); i++)
		{
			//test w2s
			//if (logger)
			//DrawStrin(pFont, (int)AimHandInfo[i].vOutX, (int)AimHandInfo[i].vOutY, D3DCOLOR_ARGB(255, 0, 255, 0), "o");
			//DrawStrin(pFont, (int)AimHandInfo[i].vOutX, (int)AimHandInfo[i].vOutY, Green, "%.f", (float)Daimkey);

			//aimfov
			float radiusx = (aimfov*10.0f) * (ScreenCenterX / 100);
			float radiusy = (aimfov*10.0f) * (ScreenCenterY / 100);

			if (aimfov == 0)
			{
				radiusx = 10.0f * (ScreenCenterX / 100);
				radiusy = 10.0f * (ScreenCenterY / 100);
			}

			//get crosshairdistance
			AimHandInfo[i].CrosshairDistance = GetDistance(AimHandInfo[i].vOutX, AimHandInfo[i].vOutY, ScreenCenterX, ScreenCenterY);

			//aim at team 1 or 2 (not needed)
			if (aimbot == AimHandInfo[i].iTeam)

			//if in fov
			if (AimHandInfo[i].vOutX >= ScreenCenterX - radiusx && AimHandInfo[i].vOutX <= ScreenCenterX + radiusx && AimHandInfo[i].vOutY >= ScreenCenterY - radiusy && AimHandInfo[i].vOutY <= ScreenCenterY + radiusy)

				//get closest/nearest target to crosshair
				if (AimHandInfo[i].CrosshairDistance < fClosestPos)
				{
					fClosestPos = AimHandInfo[i].CrosshairDistance;
					BestTarget = i;
				}
		}


		//if nearest target to crosshair
		if (BestTarget != -1)
		{
			DistHandX = AimHandInfo[BestTarget].vOutX - ScreenCenterX;
			DistHandY = AimHandInfo[BestTarget].vOutY - ScreenCenterY;
		}
	}
	AimHandInfo.clear();

	//do aim (aimbot)
	if (aimbot > 0 && AimCenteredInfo.size() != NULL && GetAsyncKeyState(Daimkey) & 0x8000)
	{
		UINT BestTarget = -1;
		DOUBLE fClosestPos = 99999;

		for (unsigned int i = 0; i < AimCenteredInfo.size(); i++)
		{
			//test w2s
			//if (logger)
			//DrawStrin(pFont, (int)AimCenteredInfo[i].vOutX, (int)AimCenteredInfo[i].vOutY, D3DCOLOR_ARGB(255, 0, 255, 0), "o");
			//DrawStrin(pFont, (int)AimCenteredInfo[i].vOutX, (int)AimCenteredInfo[i].vOutY, Green, "%.f", (float)Daimkey);

			//aimfov
			float radiusx = (aimfov*10.0f) * (ScreenCenterX / 100);
			float radiusy = (aimfov*10.0f) * (ScreenCenterY / 100);

			if (aimfov == 0)
			{
				radiusx = 10.0f * (ScreenCenterX / 100);
				radiusy = 10.0f * (ScreenCenterY / 100);
			}

			//get crosshairdistance
			AimCenteredInfo[i].CrosshairDistance = GetDistance(AimCenteredInfo[i].vOutX, AimCenteredInfo[i].vOutY, ScreenCenterX, ScreenCenterY);

			//aim at team 1 or 2 (not needed)
			if (aimbot == AimCenteredInfo[i].iTeam)

			//if in fov
			if (AimCenteredInfo[i].vOutX >= ScreenCenterX - radiusx && AimCenteredInfo[i].vOutX <= ScreenCenterX + radiusx && AimCenteredInfo[i].vOutY >= ScreenCenterY - radiusy && AimCenteredInfo[i].vOutY <= ScreenCenterY + radiusy)

				//get closest/nearest target to crosshair
				if (AimCenteredInfo[i].CrosshairDistance < fClosestPos)
				{
					fClosestPos = AimCenteredInfo[i].CrosshairDistance;
					BestTarget = i;
				}
		}


		//if nearest target to crosshair
		if (BestTarget != -1)
		{
			double DistCenteredX = AimCenteredInfo[BestTarget].vOutX - ScreenCenterX;
			double DistCenteredY = AimCenteredInfo[BestTarget].vOutY - ScreenCenterY;

			DistCenteredX /= (1 + aimsens);
			DistCenteredY /= (1 + aimsens);

			//aim
			//if ((botpause==false) && (GetAsyncKeyState(Daimkey) & 0x8000))
			mouse_event(MOUSEEVENTF_MOVE, (float)DistCenteredX, (float)DistCenteredY, 0, NULL);
			//mmousemove((float)DistX, (float)DistY);

			//autoshoot on
			if ((!GetAsyncKeyState(VK_LBUTTON) && (autoshoot == 1))) //
			//if ((!GetAsyncKeyState(VK_LBUTTON) && (autoshoot == 1) && (GetAsyncKeyState(Daimkey) & 0x8000))) //
			{
				if (autoshoot == 1 && !IsPressd)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					//LLeftClickDown();
					IsPressd = true;
				}
			}
		}
	}
	AimCenteredInfo.clear();

	//autoshoot off
	if (autoshoot == 1 && IsPressd)
	{
		if (timeGetTime() - astime >= asdelay)
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			//LLeftClickUp();
			IsPressd = false;
			astime = timeGetTime();
		}
	}
	/*
	//test newaim
	if (AimInfo.size() != NULL)
	{
		for (unsigned int i = 0; i < AimInfo.size(); i++)
		{
			if (AimInfo[i].vOutX > 1 && AimInfo[i].vOutY > 1)
			{
				//drawpoint on CT
				DrawPoint(pDevice, (int)AimInfo[i].vOutX, (int)AimInfo[i].vOutY, 6, 6, 0xFFFFFFFF);
				//drawpic
				//PrePresent(pDevice, (int)AimInfo[i].vOutX - 32, (int)AimInfo[i].vOutY - 20);
			}
		}
	}
	AimInfo.clear();
	
	//draw logger
	if (pFont && countnum >= 0)
	{
		char szString[255];
		sprintf_s(szString, "countnum = %d", countnum);
		DrawString(pFont, 220, 100, 0xFFFFFFFF, (char*)&szString[0]);
	}
	*/
	return EndScene_orig(pDevice);
}


//==========================================================================================================================

HRESULT APIENTRY Reset_hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	//DeleteRenderSurfaces();

	if (pFont)
		pFont->OnLostDevice();

	HRESULT ResetReturn = Reset_orig(pDevice, pPresentationParameters);

	if (SUCCEEDED(ResetReturn))
	{
		if (pFont)
			pFont->OnResetDevice();
	}

	return ResetReturn;
}

//==========================================================================================================================

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter)
{
	while (GetModuleHandle("d3d9.dll") == 0)
	{
		Sleep(100);
	}

	IDirect3D9* d3d = NULL;
	IDirect3DDevice9* d3ddev = NULL;

	HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, dllHandle, NULL);
	if(tmpWnd == NULL)
	{
		//Log("[DirectX] Failed to create temp window");
		return 0;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(d3d == NULL)
	{
		DestroyWindow(tmpWnd);
		//Log("[DirectX] Failed to create temp Direct3D interface");
		return 0;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); 
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if(result != D3D_OK)
	{
		d3d->Release();
		DestroyWindow(tmpWnd);
		//Log("[DirectX] Failed to create temp Direct3D device");
		return 0;
	}

	// We have the device, so walk the vtable to get the address of all the dx functions in d3d9.dll
	#if defined _M_X64
	DWORD64* dVtable = (DWORD64*)d3ddev;
	dVtable = (DWORD64*)dVtable[0];
	#elif defined _M_IX86
	DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0]; // == *d3ddev
	#endif
	//Log("[DirectX] dvtable: %x", dVtable);

	//for(int i = 0; i < 95; i++)
	//{
		//Log("[DirectX] vtable[%i]: %x, pointer at %x", i, dVtable[i], &dVtable[i]);
	//}
	
	// Set EndScene_orig to the original EndScene etc.
	SetStreamSource_orig = (SetStreamSource)dVtable[100];
	SetVertexShaderConstantF_orig = (SetVertexShaderConstantF)dVtable[94];
	DrawIndexedPrimitive_orig = (DrawIndexedPrimitive)dVtable[82];
	//SetTexture_orig = (SetTexture)dVtable[65];
	//Clear_orig = (Clear)dVtable[43];
	EndScene_orig = (EndScene)dVtable[42];
	Reset_orig = (Reset)dVtable[16];
	

	// Detour functions x86 & x64
	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[100], &SetStreamSource_hook, reinterpret_cast<void**>(&SetStreamSource_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[100]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[94], &SetVertexShaderConstantF_hook, reinterpret_cast<void**>(&SetVertexShaderConstantF_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[94]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[82], &DrawIndexedPrimitive_hook, reinterpret_cast<void**>(&DrawIndexedPrimitive_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[82]) != MH_OK) { return 1; }
	//if (MH_CreateHook((DWORD_PTR*)dVtable[65], &SetTexture_hook, reinterpret_cast<void**>(&SetTexture_orig)) != MH_OK) { return 1; }
	//if (MH_EnableHook((DWORD_PTR*)dVtable[65]) != MH_OK) { return 1; }
	//if (MH_CreateHook((DWORD_PTR*)dVtable[43], &Clear_hook, reinterpret_cast<void**>(&Clear_orig)) != MH_OK) { return 1; }
	//if (MH_EnableHook((DWORD_PTR*)dVtable[43]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[42], &EndScene_hook, reinterpret_cast<void**>(&EndScene_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[42]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[16], &Reset_hook, reinterpret_cast<void**>(&Reset_orig)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[16]) != MH_OK) { return 1; }

	//Log("[Detours] EndScene detour attached\n");

	d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);
		
	return 1;
}

//==========================================================================================================================

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: // A process is loading the DLL.
		dllHandle = hinstDLL;
		DisableThreadLibraryCalls(hinstDLL); // disable unwanted thread notifications to reduce overhead
		GetModuleFileNameA(hinstDLL, dlldir, 512);
		for (int i = (int)strlen(dlldir); i > 0; i--)
		{
			if (dlldir[i] == '\\')
			{
				dlldir[i + 1] = 0;
				break;
			}
		}
		CreateThread(0, 0, DirectXInit, 0, 0, 0); //init our hooks
		break;

	case DLL_PROCESS_DETACH: // A process unloads the DLL.
		//if (MH_Uninitialize() != MH_OK) { return 1; }
		//if (MH_DisableHook((DWORD_PTR*)dVtable[42]) != MH_OK) { return 1; }
		break;
	}
	return TRUE;
}
