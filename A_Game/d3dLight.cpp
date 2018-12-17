#include "d3dUtility.h"
#include "d3dLight.h"


ID3D11Device* device = NULL;                    //D3D11�豸ָ��
IDXGISwapChain* swapChain = NULL;               //������ָ��
ID3D11DeviceContext* immediateContext = NULL;   //�豸������ָ��
ID3D11RenderTargetView* renderTargetView = NULL;//��ȾĿ����ͼָ��  

//��ɫ��
ID3D11VertexShader* m_VertexShader;
ID3D11PixelShader* m_PixelShader;

//Effect���ȫ��ָ��
ID3D11InputLayout* vertexLayout;
ID3DX11Effect* effect;
ID3DX11EffectTechnique* technique;

//������������ϵ����
XMMATRIX world;         //��������任�ľ���
XMMATRIX view;          //���ڹ۲�任�ľ���
XMMATRIX projection;    //����ͶӰ�任�ľ���

//�������ʺ͹��յ�ȫ�ֶ���
Material		material;      //����
Light			light[3];      //��Դ����
int             lightType = 0;  //��Դ����

ID3D11DepthStencilView* depthStencilView;  //���ģ����ͼ
XMVECTOR		eyePositin;                //�ӵ�λ��

//����һ������ṹ����������������ͷ�����
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

void SetLightEffect(Light light);

//**************����Ϊ��ܺ���******************
bool Setup()
{
	//������Ҫ����3����Ҫ����
	//��һ����.fx�ļ�����ID3DEffect����
	//�ڶ����������㻺���Լ�������������
	//���������ñ任����ϵ
	//���Ĳ����ò��ʺ͹���
	//*************��һ����.fx�ļ�����ID3DEffect����****************************
	HRESULT hr = S_OK;              //����HRESULT�Ķ������ڼ�¼���������Ƿ�ɹ�
	ID3DBlob* pTechBlob = NULL;     //����ID3DBlob�Ķ������ڴ�Ŵ��ļ���ȡ����Ϣ
	ID3DBlob* compilationMsgs = nullptr;
	//������֮ǰ������.fx�ļ���ȡ��ɫ�������Ϣ
	//hr = D3DX11CompileFromFile(L"lightShader.fx", NULL, NULL, NULL, "fx_5_0",
	//	D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &pTechBlob, NULL, NULL);
	//if (FAILED(hr))
	//{
	//	::MessageBox(NULL, L"fx�ļ�����ʧ��", L"Error", MB_OK); //�����ȡʧ�ܣ�����������Ϣ
	//	return hr;
	//}
	////����D3DX11CreateEffectFromMemory����ID3DEffect����
	//hr = D3DX11CreateEffectFromMemory(pTechBlob->GetBufferPointer(),
	//	pTechBlob->GetBufferSize(), 0, device, &effect);
	hr = D3DX11CompileEffectFromFile(
		L"Shader/LightShader.fx",
		nullptr, nullptr,
		D3DCOMPILE_ENABLE_STRICTNESS, 0, device, &effect, &compilationMsgs);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"����Effectʧ��", L"Error", MB_OK);  //����ʧ�ܣ�����������Ϣ
		return hr;
	}
	//��GetTechniqueByName��ȡID3DX11EffectTechnique�Ķ���
	//������Ĭ�ϵ�technique��Effect
	technique = effect->GetTechniqueByName("T0");                //Ĭ��Technique

	//D3DX11_PASS_DESC�ṹ��������һ��Effect Pass
	D3DX11_PASS_DESC PassDesc;
	//����GetPassByIndex��ȡEffect Pass
	//������GetDesc��ȡEffect Pass��������������PassDesc������
	technique->GetPassByIndex(0)->GetDesc(&PassDesc);

	//�������������벼��
	//�������Ƕ���һ��D3D11_INPUT_ELEMENT_DESC���飬
	//�������Ƕ���Ķ���ṹ����λ������ͷ��������������������������Ԫ��
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//layoutԪ�ظ���
	UINT numElements = ARRAYSIZE(layout);
	//����CreateInputLayout�������벼��
	hr = device->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &vertexLayout);
	//�������ɵ����벼�ֵ��豸��������
	immediateContext->IASetInputLayout(vertexLayout);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"����Input Layoutʧ��", L"Error", MB_OK);
		return hr;
	}
	//*************��һ����.fx�ļ�����ID3DEffect����****************************

	//*************�ڶ����������㻺���Լ�������������***************************
	//��ʵ��4һ���������������飬����ÿ�������а������������ɫ
	//�������ڶ���ķ�������ͬ�����Լ�ʹ��ͬһ��λ�õ���������Ҳ���뵥������
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	UINT vertexCount = ARRAYSIZE(vertices);
	//�������㻺�棬����ͬʵ��4һ��
	//��������һ��D3D11_BUFFER_DESC�Ķ���bd
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * 24;      //ע�⣺�������ﶨ����24����������Ҫ����24
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;  //ע�⣺�����ʾ�������Ƕ��㻺��
	bd.CPUAccessFlags = 0;

	//����һ��D3D11_SUBRESOURCE_DATA�������ڳ�ʼ������Դ
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;         //������Ҫ��ʼ�������ݣ���������ݾ��Ƕ�������

	//����һ��ID3D11Buffer������Ϊ���㻺��
	ID3D11Buffer* vertexBuffer;
	//����CreateBuffer�������㻺��
	hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"����VertexBufferʧ��", L"Error", MB_OK);
		return hr;
	}
	//������������
	//ע�⣺�������ÿһ�����ֱ�ʾ��������Ķ�Ӧ�±�Ķ��㡣
	//      ������������12����������ɣ����Թ���Ҫ36������
	//      ����ÿ�������ֹ���һ��������
	WORD indices[] =
	{
		3,1,0,    2,1,3,    6,4,5,    7,4,6,    11,9,8,    10,9,11,
		14,12,13, 15,12,14, 19,17,16, 18,17,19, 22,20,21,  23,20,22
	};
	UINT indexCount = ARRAYSIZE(indices);

	//������������
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 36;        // ������36��������������Ҫ����36
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;    // ע�⣺�����ʾ����������������
	bd.CPUAccessFlags = 0;

	InitData.pSysMem = indices;                //������Ҫ��ʼ�������ݣ���������ݾ�����������
	ID3D11Buffer* indexBuffer;                 //����һ��ID3D11Buffer������Ϊ��������
	//����CreateBuffer������������
	hr = device->CreateBuffer(&bd, &InitData, &indexBuffer);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"����IndexBufferʧ��", L"Error", MB_OK);
		return hr;
	}
	UINT stride = sizeof(Vertex);                 //��ȡVertex�Ĵ�С��Ϊ���
	UINT offset = 0;                              //����ƫ����Ϊ0
	//���ö��㻺�棬�����Ľ��ͼ�ʵ��4
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//������������
	immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//ָ��ͼԪ���ͣ�D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST��ʾͼԪΪ������
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//*************�ڶ����������㻺���Լ�������������****************************

	//*************���������ñ任����ϵ***********************
	//��ʼ���������
	world = XMMatrixIdentity();

	//��ʼ���۲����
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);//���λ��
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);  //Ŀ��λ��
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //up
	view = XMMatrixLookAtLH(Eye, At, Up);   //���ù۲�����ϵ

	//����ͶӰ����
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);
	//*************���������ñ任����ϵ***********************

	//*************���Ĳ����ò��ʺ͹���***********************
	// ���ò��ʣ�3�й��յķ������Լ�����ⷴ��ϵ��
	//��������ǰ��λ��ʾ��������ķ����ʣ�1��ʾ��ȫ���䣬0��ʾ��ȫ����
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //ǰ��λ�ֱ��ʾ��������ķ�����
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //ͬ��
	material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//ͬ��
	material.power = 5.0f;

	// ���ù�Դ
	Light dirLight, pointLight, spotLight;
	// �����ֻ��Ҫ���ã�����3�ֹ���ǿ��
	dirLight.type = 0;
	dirLight.direction = XMFLOAT4(-1.0f, 0.0f, 1.0f, 1.0f); //���շ���
	dirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //ǰ��λ�ֱ��ʾ���������ǿ��
	dirLight.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);   //ͬ��
	dirLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  //ͬ��

	// ���Դ��Ҫ���ã�λ�á�3�й���ǿ�ȡ�3��˥������
	pointLight.type = 1;
	pointLight.position = XMFLOAT4(0.0f, 2.0f, -5.0f, 1.0f); //��Դλ��
	pointLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //ǰ��λ�ֱ��ʾ���������ǿ��
	pointLight.diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);   //ͬ��
	pointLight.specular = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);  //ͬ��
	pointLight.attenuation0 = 0;      //����˥������
	pointLight.attenuation1 = 0.1f;   //һ��˥������
	pointLight.attenuation2 = 0;      //����˥������

	// �۹����Ҫ����Light�ṹ�����еĳ�Ա
	spotLight.type = 2;
	spotLight.position = XMFLOAT4(0.0f, 0.0f, -3.0f, 1.0f); //��Դλ��
	spotLight.direction = XMFLOAT4(0.4f, 0.0f, 1.0f, 1.0f); //���շ���
	spotLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);   //ǰ��λ�ֱ��ʾ���������ǿ��
	spotLight.diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);   //ͬ��
	spotLight.specular = XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f);  //ͬ��
	spotLight.attenuation0 = 0;    //����˥������
	spotLight.attenuation1 = 0.1f; //һ��˥������
	spotLight.attenuation2 = 0;    //����˥������
	spotLight.alpha = XM_PI / 6;   //��׶�Ƕ�
	spotLight.beta = XM_PI / 3;    //��׶�Ƕ�
	spotLight.fallOff = 1.0f;      //˥��ϵ����һ��Ϊ1.0

	light[0] = dirLight;
	light[1] = pointLight;
	light[2] = spotLight;
	//*************���Ĳ����ò��ʺ͹���***********************
	return true;
}

void Cleanup()
{
	//�ͷ�ȫ��ָ��
	if (renderTargetView) renderTargetView->Release();
	if (immediateContext) immediateContext->Release();
	if (swapChain) swapChain->Release();
	if (device) device->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_PixelShader) m_PixelShader->Release();

	if (vertexLayout) vertexLayout->Release();
	if (effect) effect->Release();
	if (depthStencilView) depthStencilView->Release();
}

bool Display(float timeDelta)
{
	if (device)
	{
		//����һ����������ɫ��Ϣ��4��Ԫ�طֱ��ʾ�죬�̣����Լ�alpha
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
		immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
		//�����ǰ�󶨵����ģ����ͼ
		immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// ÿ��һ��ʱ�����һ�γ�������ʵ�����������ת.
		static float angle = 0.0f;   //����һ����̬�������ڼ�¼�Ƕ�
		angle += timeDelta;          //����ǰ�Ƕȼ���һ��ʱ���
		if (angle >= 6.28f)           //�����ǰ�Ƕȴ���2PI�������
			angle = 0.0f;
		world = XMMatrixRotationY(angle);   //���ݵ�ǰ�Ƕ���Y����ת��������������ϵ

		//������任����ĳ��������еľ�����������õ�Effect�����---------------------
		//ע�⣺�����"World"��"View"��"Projection"��"EyePosition"����.fx�ļ��ж����
		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float*)&world);  //������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float*)&view);    //���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float*)&projection); //����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float*)&eyePositin); //�����ӵ�

		//������Ϣ�ĳ��������еĲ�����Ϣ���õ�Effect�����-----------------------------
		effect->GetVariableByName("MatAmbient")->AsVector()->SetFloatVector((float*)&(material.ambient));
		effect->GetVariableByName("MatDiffuse")->AsVector()->SetFloatVector((float*)&(material.diffuse));
		effect->GetVariableByName("MatSpecular")->AsVector()->SetFloatVector((float*)&(material.specular));
		effect->GetVariableByName("MatPower")->AsScalar()->SetFloat(material.power);

		//��Դ�ĳ��������еĹ�Դ��Ϣ���õ�Effect�����---------------------------------
		SetLightEffect(light[lightType]);

		//����һ��D3DX11_TECHNIQUE_DESC����������technique
		D3DX11_TECHNIQUE_DESC techDesc;
		technique->GetDesc(&techDesc);    //��ȡtechnique������
		//��ȡͨ����PASS���������õ��豸�������С�
		//��������ֻ��һ��ͨ������������Ϊ0
		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(36, 0, 0);   //����������

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

		if (wParam == VK_F1)  //��F1������Դ���͸�Ϊ�����
			lightType = 0;
		if (wParam == VK_F2)  //��F2������Դ���͸�Ϊ���Դ
			lightType = 1;
		if (wParam == VK_F3)  //��F3������Դ���͸�Ϊ�۹�ƹ�Դ
			lightType = 2;

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

	if (!Setup())
	{
		::MessageBox(0, L"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	return 0;
}

//��Դ�ĳ����������õ�Effect�����
//���ڹ������ñȽϸ��ӣ�������һ����������������
void SetLightEffect(Light light)
{
	//���Ƚ��������ͣ�������ǿ�ȣ������ǿ�ȣ������ǿ�����õ�Effect��
	effect->GetVariableByName("type")->AsScalar()->SetInt(light.type);
	effect->GetVariableByName("LightAmbient")->AsVector()->SetFloatVector((float*)&(light.ambient));
	effect->GetVariableByName("LightDiffuse")->AsVector()->SetFloatVector((float*)&(light.diffuse));
	effect->GetVariableByName("LightSpecular")->AsVector()->SetFloatVector((float*)&(light.specular));

	//������ݹ������͵Ĳ�ͬ���ò�ͬ������
	if (light.type == 0)  //�����
	{
		//�����ֻ��Ҫ������������Լ���
		effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));
		//��������Tectnique���õ�Effect
		technique = effect->GetTechniqueByName("T_DirLight");
	}
	else if (light.type == 1)  //���Դ
	{
		//���Դ��Ҫ��λ�á���������˥�����ӡ�����һ��˥�����ӡ���������˥�����ӡ�
		effect->GetVariableByName("LightPosition")->AsVector()->SetFloatVector((float*)&(light.position));
		effect->GetVariableByName("LightAtt0")->AsScalar()->SetFloat(light.attenuation0);
		effect->GetVariableByName("LightAtt1")->AsScalar()->SetFloat(light.attenuation1);
		effect->GetVariableByName("LightAtt2")->AsScalar()->SetFloat(light.attenuation2);

		//�����Դ��Tectnique���õ�Effect
		technique = effect->GetTechniqueByName("T_PointLight");
	}
	else if (light.type == 2) //�۹�ƹ�Դ
	{
		//���Դ��Ҫ�����򡱣������򡱣�������˥�����ӡ�����һ��˥�����ӡ���������˥�����ӡ�
		//����׶�Ƕȡ�������׶�Ƕȡ������۹��˥��ϵ����
		effect->GetVariableByName("LightPosition")->AsVector()->SetFloatVector((float*)&(light.position));
		effect->GetVariableByName("LightDirection")->AsVector()->SetFloatVector((float*)&(light.direction));

		effect->GetVariableByName("LightAtt0")->AsScalar()->SetFloat(light.attenuation0);
		effect->GetVariableByName("LightAtt1")->AsScalar()->SetFloat(light.attenuation1);
		effect->GetVariableByName("LightAtt2")->AsScalar()->SetFloat(light.attenuation2);

		effect->GetVariableByName("LightAlpha")->AsScalar()->SetFloat(light.alpha);
		effect->GetVariableByName("LightBeta")->AsScalar()->SetFloat(light.beta);
		effect->GetVariableByName("LightFallOff")->AsScalar()->SetFloat(light.fallOff);

		//���۹�ƹ�Դ��Tectnique���õ�Effect
		technique = effect->GetTechniqueByName("T_SpotLight");
	}
}

