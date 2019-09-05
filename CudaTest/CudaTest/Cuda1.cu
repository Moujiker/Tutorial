#include <iostream>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

using namespace std;


__global__ void add(int a, int b, int *c)
{
	*c = a + b;
}



bool InitCUDA()
{
	int count;
	cudaGetDeviceCount(&count);
	if (count == 0)
	{
		fprintf(stderr, "There is no device.\n");
		return false;
	}
	int i;
	for (i = 0; i < count; i++)
	{
		cudaDeviceProp prop;
		if (cudaGetDeviceProperties(&prop, i) == cudaSuccess)
		{
			if (prop.major >= 1)
			{
				//Ã¶¾ÙÏêÏ¸ÐÅÏ¢
				printf("Identify: %s\n", prop.name);
				printf("Host Memory: %d\n", prop.canMapHostMemory);
				printf("Clock Rate: %d khz\n", prop.clockRate);
				printf("Compute Mode: %d\n", prop.computeMode);
				printf("Device Overlap: %d\n", prop.deviceOverlap);
				printf("Integrated: %d\n", prop.integrated);
				printf("Kernel Exec Timeout Enabled: %d\n", prop.kernelExecTimeoutEnabled);
				printf("Max Grid Size: %d * %d * %d\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
				printf("Max Threads Dim: %d * %d * %d\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
				printf("Max Threads per Block: %d\n", prop.maxThreadsPerBlock);
				printf("Maximum Pitch: %d bytes\n", prop.memPitch);
				printf("Minor Compute Capability: %d\n", prop.minor);
				printf("Number of Multiprocessors: %d\n", prop.multiProcessorCount);
				printf("32bit Registers Availble per Block: %d\n", prop.regsPerBlock);
				printf("Shared Memory Available per Block: %d bytes\n", prop.sharedMemPerBlock);
				printf("Alignment Requirement for Textures: %d\n", prop.textureAlignment);
				printf("Constant Memory Available: %d bytes\n", prop.totalConstMem);
				printf("Global Memory Available: %d bytes\n", prop.totalGlobalMem);
				printf("Warp Size: %d threads\n", prop.warpSize);
				break;
			}
		}
	}
	if (i == count)
	{
		fprintf(stderr, "There is no device supporting CUDA.\n");
		return false;
	}
	cudaSetDevice(i);

	printf("CUDA initialized.\n");
	getchar();

	return true;
}

int main(){

	
	system("pause");
	return 0;
}