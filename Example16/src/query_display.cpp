#include "query_display.h"

#include <dxgi1_6.h>

bool getHdrMetdata(VkHdrMetadataEXT& hdrMetadata)
{
	IDXGIFactory* factory = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIOutput* output = nullptr;

	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		factory->Release();

		return false;
	}

	result = adapter->EnumOutputs(0, &output);
	if(FAILED(result))
	{
		adapter->Release();
		factory->Release();

		return false;
	}
	IDXGIOutput6* output6 = (IDXGIOutput6*)output;

	DXGI_OUTPUT_DESC1 outputDesc1;
    output6->GetDesc1(&outputDesc1);
	if(FAILED(result))
	{
		output6->Release();
		adapter->Release();
		factory->Release();

		return false;
	}

	hdrMetadata.sType = VK_STRUCTURE_TYPE_HDR_METADATA_EXT;
	hdrMetadata.pNext = nullptr;
	hdrMetadata.displayPrimaryRed.x = outputDesc1.RedPrimary[0];
	hdrMetadata.displayPrimaryRed.y = outputDesc1.RedPrimary[1];
	hdrMetadata.displayPrimaryGreen.x = outputDesc1.GreenPrimary[0];
	hdrMetadata.displayPrimaryGreen.y = outputDesc1.GreenPrimary[1];
	hdrMetadata.displayPrimaryBlue.x = outputDesc1.BluePrimary[0];
	hdrMetadata.displayPrimaryBlue.y = outputDesc1.BluePrimary[1];
	hdrMetadata.whitePoint.x = outputDesc1.WhitePoint[0];
	hdrMetadata.whitePoint.y = outputDesc1.WhitePoint[1];
	hdrMetadata.maxLuminance = outputDesc1.MaxLuminance;
	hdrMetadata.minLuminance = outputDesc1.MinLuminance;
	// see https://professionalsupport.dolby.com/s/article/Calculation-of-MaxFALL-and-MaxCLL-metadata
	// MaxCLL
	hdrMetadata.maxContentLightLevel = hdrMetadata.maxLuminance;
	// MaxFALL
	hdrMetadata.maxFrameAverageLightLevel = (hdrMetadata.maxLuminance + hdrMetadata.minLuminance) * 0.5f;

	//

	output6->Release();
	adapter->Release();
	factory->Release();

    return true;
}
