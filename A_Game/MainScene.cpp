#include "d3dUtility.h"
#include "GameObject.h"

class  MainScene : public D3DUtility
{
public:
	virtual bool InitApp() override;
	MainScene(HINSTANCE hInstance);
	int Running();
};
MainScene::MainScene(HINSTANCE hInstance) : D3DUtility(hInstance)
{

}

int MainScene::Running()
{
	// Run the message loop.
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		//�ַ���Ϣ,��Ϣ���Ըı䶯����Ⱦ�е�ĳЩ����,���������˶�,
		//����ӽǵȵ�...,��Ϣ�Ĵ�������������Ϣ����(Procedure)��˵
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			//����һ����������ɫ��Ϣ��4��Ԫ�طֱ��ʾ�죬�̣����Լ�alpha
			float ClearColor[4] = { 0.2f, 0.125f, 0.3f, 1.0f };
			//�����ȾĿ����ͼ
			immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
			swapChain->Present(0, 0);
			//OutputDebugString(L"!!!ALIVE!!\r\n");
			//��������ж����������Ⱦ
		}
	}

	return 0;
}





//
////**************����Ϊ��ܺ���******************
//bool Setup(GameObject* obj)
//{
//	
//	immediateContext->IASetInputLayout(obj->vertexLayout);
//	immediateContext->IASetVertexBuffers(0, 1, &(obj->vertexBuffer), &obj->stride, &obj->offset);
//	immediateContext->IASetIndexBuffer(obj->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
//	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	return true;
//}
//
//void Cleanup()
//{
//	//�ͷ�ȫ��ָ��
//	if (renderTargetView) renderTargetView->Release();
//	if (immediateContext) immediateContext->Release();
//	if (swapChain) swapChain->Release();
//	if (device) device->Release();
//	/*if (m_VertexShader) m_VertexShader->Release();
//	if (m_PixelShader) m_PixelShader->Release();
//
//	if (vertexLayout) vertexLayout->Release();
//	if (effect) effect->Release();*/
//	if (depthStencilView) depthStencilView->Release();
//}
//
//bool Display(float timeDelta)
//{
//	if (device)
//	{
//
//		//�����ȾĿ����ͼ
//		immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
//		
//			
//		gtechnique->GetPassByIndex(0)->Apply(0, immediateContext);
//
//		immediateContext->DrawIndexed(36, 0, 0);
//		swapChain->Present(0, 0);
//	}
//	return true;
//}
//**************��ܺ���******************

//
////
//// �ص�����
////
//LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_DESTROY:
//		::PostQuitMessage(0);
//		break;
//
//	case WM_KEYDOWN:
//		if (wParam == VK_ESCAPE)
//			::DestroyWindow(hwnd);
//		break;
//	}
//	return ::DefWindowProc(hwnd, msg, wParam, lParam);
//}

//
// ������WinMain
//
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	MainScene mainscene(hInstance);
	//TestApp *theApp = new TestApp(hInstance);
	mainscene.InitApp();
	mainscene.Running();


	return 0;
}

bool MainScene::InitApp()
{
	if (!D3DUtility::InitApp())
		return false;
}
