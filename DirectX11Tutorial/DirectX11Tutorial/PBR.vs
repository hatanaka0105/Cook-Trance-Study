
//Grobals

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

//TypeDefs

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

//VertexShader

PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    //�K�؂ȍs��v�Z�̂��߂Ɉʒu�x�N�g����4�P�ʂɕύX
    input.position.w = 1.0f;

    //���[���h�A�r���[�A����ѓ��e�s��ɑ΂��Ē��_�̈ʒu���v�Z
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    //�s�N�Z���V�F�[�_�̃e�N�X�`�����W���i�[
    output.tex = input.tex;

    //�@���x�N�g�������[���h�}�g���b�N�X�ɑ΂��Ă̂݌v�Z
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    //�@���x�N�g���𐳋K��
    output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}