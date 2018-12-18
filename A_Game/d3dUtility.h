#pragma once
#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include <Windows.h>
//��ѧ�����ͷ�ļ�
#include <DirectXMath.h>
using namespace DirectX;

//DirectX11���ͷ�ļ�
#include <d3d11.h>
#include <d3dcompiler.h>

//Effect11 ͷ�ļ�
#include <d3dx11effect.h>


//DirectX11��ؿ�
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

class d3dUtility
{
public:

protected:
	HRESULT InitApp();
private:
	//��ʼ������
	HRESULT InitWindow();

	//��ʼ��D3D
	HRESULT InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView** renderTargetView,        //Ŀ����Ⱦ��ͼ�ӿ�
		ID3D11DeviceContext** immediateContext,           //�豸�����Ľӿڣ��豸�����İ����豸��ʹ�û���������
		IDXGISwapChain** swapChain,                       //�������ӿڣ���������������������
		ID3D11Device** device);                           //�豸�ýӿڣ�ÿ��D3D����������һ���豸


	//�ص�����
	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);
};
#endif