//这是我们自己创建的"D3DUtility.h"头文件
#include "D3DUtility.h"

D3DUtility* D3DUtility::mApp = nullptr;
LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return D3DUtility::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}



D3DUtility::D3DUtility(HINSTANCE hInstance)
{
	mApp = this;
}
D3DUtility* D3DUtility::GetApp()
{
	return mApp;
}


bool D3DUtility::InitApp()
{
	InitWindow();
	InitD3D(
		mhAppInst,
		mClientWidth,
		mClientHeight, 
		renderTargetView.GetAddressOf(),
		immediateContext.GetAddressOf(),
		swapChain.GetAddressOf(),
		device.GetAddressOf());
	return true;
}

HRESULT D3DUtility::InitWindow()
{
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = mhAppInst;
	wc.lpszClassName = L"Sample Window Class";
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClass(&wc);
	mhMainWnd = CreateWindowEx(
		0,                              // Optional window styles.
		L"Sample Window Class",                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, mClientWidth, mClientHeight,

		NULL,       // Parent window    
		NULL,       // Menu
		mhAppInst,  // Instance handle
		NULL        // Additional application data
	);

	if (mhMainWnd == NULL)
	{
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	return S_OK;
}


HRESULT D3DUtility::InitD3D(HINSTANCE hInstance, int width, int height, ID3D11RenderTargetView ** renderTargetView, ID3D11DeviceContext ** immediateContext, IDXGISwapChain ** swapChain, ID3D11Device ** device)
{
	//***********第二部分：初始化D3D开始***************
	//初始化D3D设备主要为以下步骤
	//1. 描述交换链，即填充DXGI_SWAP_CHAIN_DESC结构
	//2. 使用D3D11CreateDeviceAndSwapChain创建D3D设备（ID3D11Device）
	//   设备上下文接口（ID3D11DeviceContext），交换链接口（IDXGISwapChain）
	//3. 创建目标渲染视图（ID3D11RenderTargetView）
	//4. 设置视口（View Port）   


	//第一步，描述交换链，即填充DXGI_SWAP_CHAIN_DESC结构
	DXGI_SWAP_CHAIN_DESC sd;                           //首先声明一个DXGI_SWAP_CHAIN_DESC的对象sd
	ZeroMemory(&sd, sizeof(sd));                   //用ZeroMemory对sd进行初始化，ZeroMemory的用法见实验一的补充知识
	sd.BufferCount = 1;                                //交换链中后台缓存数量，通常为1
	sd.BufferDesc.Width = width;                       //缓存区中的窗口宽
	sd.BufferDesc.Height = height;                     //缓存区中的窗口高
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //指定32位像素格式，表示红绿蓝Alpha各8位，其他格式见书P50
	sd.BufferDesc.RefreshRate.Numerator = 60;          //刷新频率的分子为60
	sd.BufferDesc.RefreshRate.Denominator = 1;         //刷新频率的分母为1，即，刷新频率为每秒6次
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //用来描述后台缓存的用法控制CPU对后台缓存的访问 
	sd.OutputWindow = mhMainWnd;                            //指向渲染目标窗口的句柄
	sd.SampleDesc.Count = 1;                           //多重采样的属性，本例中不采用多重采样即，
	sd.SampleDesc.Quality = 0;                         //所以Count=1，Quality=0，详细见书P54
	sd.Windowed = TRUE;                                //TRUE为窗口模式，FALSE为全屏模式

	//第二步，创建设备，交换链以及立即执行上下文
	//创建一个数组确定尝试创建Featurelevel的顺序
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0, //D3D11 所支持的特征，包括shader model 5
		D3D_FEATURE_LEVEL_10_1, //D3D10 所支持的特征，包括shader model 4.
		D3D_FEATURE_LEVEL_10_0,
	};

	//获取D3D_FEATURE_LEVEL数组的元素个数
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//调用D3D11CreateDeviceAndSwapChain创建交换链，设备，和设备上下文
	//分别存入swapChain，device，immediateContext
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,                       //确定显示适配器，NULL表示默认显示适配器
		D3D_DRIVER_TYPE_HARDWARE,   //选择驱动类型，这里表示使用三维硬件加速
		NULL,                       //只有上一个参数设置D3D_DRIVER_TYPE_SOFTWARE时，才使用这个参数
		0,                          //也可以设置为D3D11_CREATE_DEVICE_DEBUG开启调试模式
		featureLevels,              //前面定义的D3D_FEATURE_LEVEL数组
		numFeatureLevels,           //D3D_FEATURE_LEVEL的元素个数
		D3D11_SDK_VERSION,          //SDK的版本，这里为D3D11
		&sd,                        //前面定义的DXGI_SWAP_CHAIN_DESC对象
		swapChain,                  //返回创建好的交换链指针，InitD3D函数传递的实参
		device,                     //返回创建好的设备用指针，InitD3D函数传递的实参
		NULL,                       //返回当前设备支持的featureLevels数组中的第一个对象，一般设置为NULL
		immediateContext)))      //返回创建好的设备上下文指针，InitD3D函数传递的实参
	{
		::MessageBox(0, L"CreateDevice - FAILED", 0, 0);  //如果创建失败，弹出消息框
		return false;
	}

	//第三步，创建并设置渲染目标视图
	HRESULT hr = 0;         //COM要求所有的方法都会返回一个HRESULT类型的错误号
	ID3D11Texture2D* pBackBuffer = NULL;      //ID3D11Texture2D类型的，后台缓存指针
	//调用GetBuffer()函数得到后台缓存对象，并存入&pBackBuffer中
	hr = (*swapChain)->GetBuffer(0,                        //缓存索引，一般设置为0
		__uuidof(ID3D11Texture2D), //缓存类型
		(LPVOID*)&pBackBuffer);   //缓存指针
//判断GetBuffer是否调用成功
	if (FAILED(hr))
	{
		::MessageBox(0, L"GetBuffer - FAILED", 0, 0); //如果调用失败，弹出消息框
		return false;
	}

	//调用CreateRenderTargetView创建好渲染目标视图，创建后存入renderTargetView中
	hr = (*device)->CreateRenderTargetView(pBackBuffer,            //上面创建好的后台缓存
		NULL,                   //设置为NULL得到默认的渲染目标视图
		renderTargetView);     //返回创建好的渲染目标视图，InitD3D函数传递的实参
	pBackBuffer->Release();   //释放后台缓存
	//判断CreateRenderTargetView是否调用成功
	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateRender - FAILED", 0, 0);  //如果调用失败，弹出消息框
		return false;
	}

	//将渲染目标视图绑定到渲染管线  
	(*immediateContext)->OMSetRenderTargets(1,                   //绑定的目标视图的个数
		renderTargetView,    //渲染目标视图，InitD3D函数传递的实参 
		NULL);              //设置为NULL表示不绑定深度模板

//第四步，设置视口大小，D3D11默认不会设置视口，此步骤必须手动设置  
	D3D11_VIEWPORT vp;    //创建一个视口的对象
	vp.Width = width;     //视口的宽
	vp.Height = height;   //视口的高
	vp.MinDepth = 0.0f;   //深度值的下限，**由于深度值是[0, 1]所以下限值是0
	vp.MaxDepth = 1.0f;   //深度值的上限，上限值是1
	vp.TopLeftX = 0;      //视口左上角的横坐标
	vp.TopLeftY = 0;      //视口左上角的总坐标

	//设置视口
	(*immediateContext)->RSSetViewports(1,     //视口的个数
		&vp); //上面创建的视口对象

	return true;
	//***********第二部分：初始化D3D结束***************
	return E_NOTIMPL;
}



