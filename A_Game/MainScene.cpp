#include "d3dUtility.h"
#include "GameObject.h"
ID3D11Device* device = NULL;                    //D3D11�豸ָ��
IDXGISwapChain* swapChain = NULL;               //������ָ��
ID3D11DeviceContext* immediateContext = NULL;   //�豸������ָ��
ID3D11RenderTargetView* renderTargetView = NULL;//��ȾĿ����ͼָ��  
ID3DX11EffectTechnique* gtechnique = nullptr;

ID3D11DepthStencilView* depthStencilView;  //���ģ����ͼ

//����һ����������ɫ��Ϣ��4��Ԫ�طֱ��ʾ�죬�̣����Լ�alpha
float ClearColor[4] = { 0.8f, 0.125f, 0.3f, 1.0f };





//**************����Ϊ��ܺ���******************
bool Setup(GameObject* obj)
{
	
	immediateContext->IASetInputLayout(obj->vertexLayout);
	immediateContext->IASetVertexBuffers(0, 1, &(obj->vertexBuffer), &obj->stride, &obj->offset);
	immediateContext->IASetIndexBuffer(obj->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void Cleanup()
{
	//�ͷ�ȫ��ָ��
	if (renderTargetView) renderTargetView->Release();
	if (immediateContext) immediateContext->Release();
	if (swapChain) swapChain->Release();
	if (device) device->Release();
	/*if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();

	if (vertexLayout) vertexLayout->Release();
	if (effect) effect->Release();*/
	if (depthStencilView) depthStencilView->Release();
}

bool Display(float timeDelta)
{
	if (device)
	{

		//�����ȾĿ����ͼ
		immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
		
			
		gtechnique->GetPassByIndex(0)->Apply(0, immediateContext);

		immediateContext->DrawIndexed(36, 0, 0);
		swapChain->Present(0, 0);
	}
	return true;
}
//**************��ܺ���******************


//
// �ص�����
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// ������WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	//��ʼ��
	//**ע��**:������������IDirect3DDevice9ָ�룬��������Ϊ��������InitD3D����
	if (!d3d::InitD3D(hinstance,
		800,
		600,
		&renderTargetView,
		&immediateContext,
		&swapChain,
		&device))// [out]The created device.
	{
		::MessageBox(0, L"InitD3D() - FAILED", 0, 0);
		return 0;
	}

	GameObject* cube=new GameObject(device);


	if (!Setup(cube))
	{
		::MessageBox(0, L"Setup() - FAILED", 0, 0);
		return 0;
	}
	gtechnique = cube->technique;
	d3d::EnterMsgLoop(Display);

	Cleanup();

	return 0;
}

