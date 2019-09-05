#include "funset.cuh"
#include <stdio.h>

__global__ void add(int a, int b, int* c)
{
	*c = a + b;
}

//__global__���������ϵ��ò����豸������
__global__ void add_blockIdx(int* a, int* b, int* c)
{
	//�����������������
	//����blockIdx����һ�����ñ�������CUDA����ʱ���Ѿ�Ԥ�ȶ������������
	//�˱����а�����ֵ���ǵ�ǰִ���豸������߳̿������
	int tid = blockIdx.x;//this thread handles the data at its thread id
	if (tid < NUM)
		c[tid] = a[tid] + b[tid];
}

//__device__����ʾ���뽫��GPU���������������У�
//���ڴ˺���������Ϊ__device__���������ֻ�ܴ�����__device__��������
//��__global__�����е�������
__device__ int julia(int x, int y) 
{
	const float scale = 1.5;
	float jx = scale * (float)(DIM/2 - x)/(DIM/2);
	float jy = scale * (float)(DIM/2 - y)/(DIM/2);

	cuComplex c(-0.8, 0.156);
	cuComplex a(jx, jy);

	int i = 0;
	for (i=0; i<200; i++) {
		a = a * a + c;

		if (a.magnitude2() > 1000)
			return 0;
	}

	return 1;
}

__global__ void kernel_julia(unsigned char *ptr)
{
	//map from blockIdx to pixel position
	int x = blockIdx.x;
	int y = blockIdx.y;
	//gridDimΪ���ñ����������е��߳̿���˵��gridDim��һ�����������������̸߳�ÿһά�Ĵ�С
	//�˴�gridDim��ֵ��(DIM, DIM)
	int offset = x + y * gridDim.x;

	//now calculate the value at that position
	int juliaValue = julia(x, y);

	ptr[offset*4 + 0] = 255 * juliaValue;
	ptr[offset*4 + 1] = 0;
	ptr[offset*4 + 2] = 0;
	ptr[offset*4 + 3] = 255;
}

__global__ void add_threadIdx(int* a, int* b, int* c)
{
	//ʹ���߳������������ݽ�����������ͨ���߳̿�����(blockIdx.x)
	int tid = threadIdx.x;

	if (tid < NUM)
		c[tid] = a[tid] + b[tid];
}

__global__ void add_blockIdx_threadIdx(int* a, int* b, int* c)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;

	if (tid == 0) {
		printf("blockDim.x = %d, gridDim.x = %d\n", blockDim.x, gridDim.x);
	}

	while (tid < NUM) {
		c[tid] = a[tid] + b[tid];

		tid += blockDim.x * gridDim.x;
	}
}

__global__ void ripple_kernel(unsigned char *ptr, int ticks)
{
	// map from threadIdx/BlockIdx to pixel position
	//���̺߳��߳̿������ӳ�䵽ͼ������
	//��x��y��ֵ�������Ի��Ӷ��õ�����������е�һ��ƫ��
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	// now calculate the value at that position
	//����һ����ʱ��仯����������"����"
	float fx = x - DIM/2;
	float fy = y - DIM/2;
	float d = sqrtf(fx * fx + fy * fy);
	unsigned char grey = (unsigned char)(128.0f + 127.0f * cos(d/10.0f - ticks/7.0f) / (d/10.0f + 1.0f)); 

	ptr[offset*4 + 0] = grey;
	ptr[offset*4 + 1] = grey;
	ptr[offset*4 + 2] = grey;
	ptr[offset*4 + 3] = 255;
}

__global__ void dot_kernel(float *a, float *b, float *c)
{
	//������һ�������ڴ滺��������������ÿ���̼߳���ļӺ�ֵ
	__shared__ float cache[threadsPerBlock];
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	int cacheIndex = threadIdx.x;

	float temp = 0;
	while (tid < NUM) {
		temp += a[tid] * b[tid];
		tid += blockDim.x * gridDim.x;
	}

	//set the cache values
	cache[cacheIndex] = temp;

	//synchronize threads in this block
	//���߳̿��е��߳̽���ͬ��
	//���������ȷ���߳̿��е�ÿ���̶߳�ִ����__syncthreads()ǰ������󣬲Ż�ִ����һ�����
	__syncthreads();

	//for reductions(��Լ), threadsPerBlock must be a power of 2 because of the following code
	int i = blockDim.x/2;
	while (i != 0) {
		if (cacheIndex < i)
			cache[cacheIndex] += cache[cacheIndex + i];
		//��ѭ�������и����˹����ڴ����cache��������ѭ������һ�ε�����ʼ֮ǰ��
		//��Ҫȷ����ǰ�����������̵߳ĸ��²������Ѿ����
		__syncthreads();
		i /= 2;
	}

	if (cacheIndex == 0)
		c[blockIdx.x] = cache[0];
}

__global__ void julia_kernel(unsigned char *ptr)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	__shared__ float shared[16][16];

	//now calculate the value at that position
	const float period = 128.0f;

	shared[threadIdx.x][threadIdx.y] = 255 * (sinf(x*2.0f*PI/ period) + 1.0f) *(sinf(y*2.0f*PI/ period) + 1.0f) / 4.0f;

	//removing this syncthreads shows graphically what happens
	//when it doesn't exist.this is an example of why we need it.
	__syncthreads();

	ptr[offset*4 + 0] = 0;
	ptr[offset*4 + 1] = shared[15 - threadIdx.x][15 - threadIdx.y];
	ptr[offset*4 + 2] = 0;
	ptr[offset*4 + 3] = 255;
}

__global__ void RayTracing_kernel(Sphere *s, unsigned char *ptr)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;
	float ox = (x - DIM/2);
	float oy = (y - DIM/2);

	float r=0, g=0, b=0;
	float maxz = -INF;

	for (int i = 0; i < SPHERES; i++) {
		float n;
		float t = s[i].hit(ox, oy, &n);
		if (t > maxz) {
			float fscale = n;
			r = s[i].r * fscale;
			g = s[i].g * fscale;
			b = s[i].b * fscale;
			maxz = t;
		}
	} 

	ptr[offset*4 + 0] = (int)(r * 255);
	ptr[offset*4 + 1] = (int)(g * 255);
	ptr[offset*4 + 2] = (int)(b * 255);
	ptr[offset*4 + 3] = 255;
}

__global__ void RayTracing_kernel(unsigned char *ptr)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;
	float ox = (x - DIM/2);
	float oy = (y - DIM/2);

	float r=0, g=0, b=0;
	float maxz = -INF;

	for(int i = 0; i < SPHERES; i++) {
		float n;
		float t = s[i].hit(ox, oy, &n);
		if (t > maxz) {
			float fscale = n;
			r = s[i].r * fscale;
			g = s[i].g * fscale;
			b = s[i].b * fscale;
			maxz = t;
		}
	} 

	ptr[offset*4 + 0] = (int)(r * 255);
	ptr[offset*4 + 1] = (int)(g * 255);
	ptr[offset*4 + 2] = (int)(b * 255);
	ptr[offset*4 + 3] = 255;
}

__global__ void Heat_blend_kernel(float *dst, bool dstOut)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	int left = offset - 1;
	int right = offset + 1;
	if (x == 0) left++;
	if (x == DIM-1) right--; 

	int top = offset - DIM;
	int bottom = offset + DIM;
	if (y == 0) top += DIM;
	if (y == DIM-1) bottom -= DIM;

	float t, l, c, r, b;

	if (dstOut) {
		//tex1Dfetch�Ǳ��������ú��������豸�ڴ�ȡ����
		t = tex1Dfetch(texIn, top);
		l = tex1Dfetch(texIn, left);
		c = tex1Dfetch(texIn, offset);
		r = tex1Dfetch(texIn, right);
		b = tex1Dfetch(texIn, bottom);

	} else {
		t = tex1Dfetch(texOut, top);
		l = tex1Dfetch(texOut, left);
		c = tex1Dfetch(texOut, offset);
		r = tex1Dfetch(texOut, right);
		b = tex1Dfetch(texOut, bottom);
	}

	dst[offset] = c + SPEED * (t + b + r + l - 4 * c);
}

__global__ void blend_kernel(float *dst, bool dstOut)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float t, l, c, r, b;
	if (dstOut) {
		t = tex2D(texIn2, x, y-1);
		l = tex2D(texIn2, x-1, y);
		c = tex2D(texIn2, x, y);
		r = tex2D(texIn2, x+1, y);
		b = tex2D(texIn2, x, y+1);
	} else {
		t = tex2D(texOut2, x, y-1);
		l = tex2D(texOut2, x-1, y);
		c = tex2D(texOut2, x, y);
		r = tex2D(texOut2, x+1, y);
		b = tex2D(texOut2, x, y+1);
	}
	dst[offset] = c + SPEED * (t + b + r + l - 4 * c);
}

__global__ void Heat_copy_const_kernel(float *iptr)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float c = tex1Dfetch(texConstSrc, offset);
	if (c != 0)
		iptr[offset] = c;
}

__global__ void copy_const_kernel(float *iptr) 
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float c = tex2D(texConstSrc2, x, y);
	if (c != 0)
		iptr[offset] = c;
}

void generate_frame_opengl(uchar4 *pixels, void*, int ticks)
{
	dim3 grids(DIM / 16, DIM / 16);
	dim3 threads(16, 16);
	ripple_kernel_opengl<<<grids, threads>>>(pixels, ticks);
}

__global__ void ripple_kernel_opengl(uchar4 *ptr, int ticks)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	// now calculate the value at that position
	float fx = x - DIM / 2;
	float fy = y - DIM / 2;
	float d = sqrtf(fx * fx + fy * fy);
	unsigned char grey = (unsigned char)(128.0f + 127.0f * cos(d/10.0f - ticks/7.0f) / (d/10.0f + 1.0f));    
	ptr[offset].x = grey;
	ptr[offset].y = grey;
	ptr[offset].z = grey;
	ptr[offset].w = 255;
}

__global__ void Heat_blend_kernel_opengl(float *dst, bool dstOut)
{
	//map from threadIdx/BlockIdx to pixel position
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	int left = offset - 1;
	int right = offset + 1;
	if (x == 0) left++;
	if (x == DIM-1) right--; 

	int top = offset - DIM;
	int bottom = offset + DIM;
	if (y == 0) top += DIM;
	if (y == DIM-1) bottom -= DIM;

	float t, l, c, r, b;
	if (dstOut) {
		t = tex1Dfetch(texIn, top);
		l = tex1Dfetch(texIn, left);
		c = tex1Dfetch(texIn, offset);
		r = tex1Dfetch(texIn, right);
		b = tex1Dfetch(texIn, bottom);

	} else {
		t = tex1Dfetch(texOut, top);
		l = tex1Dfetch(texOut, left);
		c = tex1Dfetch(texOut, offset);
		r = tex1Dfetch(texOut, right);
		b = tex1Dfetch(texOut, bottom);
	}
	dst[offset] = c + SPEED * (t + b + r + l - 4 * c);
}

__global__ void Heat_copy_const_kernel_opengl(float *iptr)
{
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float c = tex1Dfetch(texConstSrc, offset);
	if (c != 0)
		iptr[offset] = c;
}

__global__ void histo_kernel(unsigned char *buffer, long size, unsigned int *histo)
{
	//clear out the accumulation buffer called temp since we are launched with 256 threads, 
	//it is easy to clear that memory with one write per thread
	__shared__  unsigned int temp[256]; //�����ڴ滺����
	temp[threadIdx.x] = 0;
	__syncthreads();

	//calculate the starting index and the offset to the next block that each thread will be processing
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	int stride = blockDim.x * gridDim.x;
	while (i < size) {
		atomicAdd(&temp[buffer[i]], 1);
		i += stride;
	}

	//sync the data from the above writes to shared memory then add the shared memory values to the values from
	//the other thread blocks using global memory atomic adds same as before, since we have 256 threads,
	//updating the global histogram is just one write per thread!
	__syncthreads();
	atomicAdd(&(histo[threadIdx.x]), temp[threadIdx.x]);
}

__global__ void singlestream_kernel(int *a, int *b, int *c)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx < NUM) {
		int idx1 = (idx + 1) % 256;
		int idx2 = (idx + 2) % 256;
		float as = (a[idx] + a[idx1] + a[idx2]) / 3.0f;
		float bs = (b[idx] + b[idx1] + b[idx2]) / 3.0f;
		c[idx] = (as + bs) / 2;
	}
}

__global__ void dot_kernel(int size, float *a, float *b, float *c)
{
	__shared__ float cache[threadsPerBlock];
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	int cacheIndex = threadIdx.x;

	float temp = 0;
	while (tid < size) {
		temp += a[tid] * b[tid];
		tid += blockDim.x * gridDim.x;
	}

	//set the cache values
	cache[cacheIndex] = temp;

	//synchronize threads in this block
	__syncthreads();

	//for reductions(��Լ), threadsPerBlock must be a power of 2 because of the following code
	int i = blockDim.x / 2;
	while (i != 0) {
		if (cacheIndex < i)
			cache[cacheIndex] += cache[cacheIndex + i];
		__syncthreads();
		i /= 2;
	}

	if (cacheIndex == 0)
		c[blockIdx.x] = cache[0];
}