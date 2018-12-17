cbuffer ConstantBuffer : register(b0)
{
	matrix World;             //����任����
	matrix View;              //�۲�任����
	matrix Projection;    //ͶӰ�任����
}

struct VS_INPUT
{
	float4 Pos : SV_POSITION;  //float4�����ͣ�SV_POSITIONΪ�����ʾλ��
	float4 Color : COLOR0;    //COLOR0��ʾ��ɫ��COLOR�������Ϊ0����Դ֧��
							 //�����֮�������
};




VS_INPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
	VS_INPUT input = (VS_INPUT)0;       //����һ������ṹ�Ķ���
	input.Pos = mul(Pos, World);           //��������任��mulΪ����˷��ĺ���
	input.Pos = mul(input.Pos, View);       //�۲�����任��mul��һ������Ϊ����
	input.Pos = mul(input.Pos, Projection);   //ͶӰ����任��mul�ڶ���
										//����Ϊ����
	input.Color = Color;                    //����VS_INPUT�������ɫ
	return input;                          //����VS_INPUT�Ķ���
}


float4 PS(VS_INPUT input) : SV_Target
{
   return input.Color;
}


technique11 TexTech
{
	//Techniqueͨ��Pass��Ӧ�ò�ͬ��Ч����ÿ��Technique�����ж��Pass
   //������ֻ��һ��Pass
	pass P0
	{
		//���ö�����ɫ��
		//CompileShader���������汾��һ����Ŀ����ɫ��������ʹ�õ���SM5.0
		//��һ������Ϊ�����Լ���һ����ɫ������
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);   //����Ϊ��	       
		SetPixelShader(CompileShader(ps_5_0, PS())); //����������ɫ��
	}
}
