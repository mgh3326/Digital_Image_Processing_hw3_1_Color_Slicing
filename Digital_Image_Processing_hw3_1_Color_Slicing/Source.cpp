#include<iostream>
#include<string>
#include<algorithm>//for sort
using namespace std;

#define X_MAX 256
#define Y_MAX 256


unsigned char** mem_alloc2_d(int n_height, int n_width, unsigned char nlnit_val);

unsigned char** mem_alloc2_d(int n_height, int n_width, unsigned char nlnit_val)
{
	unsigned char** rtn = new unsigned char*[n_height];
	for (int n = 0; n < n_height; n++)
	{
		rtn[n] = new unsigned char[n_width];
		memset(rtn[n], nlnit_val, sizeof(unsigned char)*n_width);
	}
	return rtn;
}

unsigned char** padding(unsigned char** in, int n_height, int n_width, int n_filter_size)
{
	int n_pad_size = (int)(n_filter_size / 2);
	unsigned char** pad = mem_alloc2_d(n_height + 2 * n_pad_size, n_width + 2 * n_pad_size, 0);

	for (int h = 0; h < n_height; h++)
	{
		for (int w = 0; w < n_width; w++)
		{
			pad[h + n_pad_size][w + n_pad_size] = in[h][w];
		}
	}

	for (int h = 0; h < n_pad_size; h++)
	{
		for (int w = 0; w < n_width; w++)
		{
			pad[h][w + n_pad_size] = in[0][w];
			pad[h + (n_height - 1)][w + n_pad_size] = in[n_height - 1][w];
		}
	}

	for (int h = 0; h < n_height; h++)
	{
		for (int w = 0; w < n_pad_size; w++)
		{
			pad[h + n_pad_size][w] = in[h][0];
			pad[h + n_pad_size][w + (n_width - 1)] = in[h][n_width - 1];
		}
	}

	for (int h = 0; h < n_pad_size; h++)
	{
		for (int w = 0; w < n_pad_size; w++)
		{
			pad[h][w] = in[0][0];
			pad[h + (n_height - 1)][w] = in[n_height - 1][0];
			pad[h][w + (n_width - 1)] = in[0][n_width - 1];
			pad[h + (n_height - 1)][w + (n_width - 1)] = in[n_height - 1][n_width - 1];
		}
	}

	return pad;
}

void smooth_filter(unsigned char **in)
{
	unsigned char** pad = padding(in, Y_MAX, X_MAX, 3);// 패딩 생성
	unsigned char** out = mem_alloc2_d(Y_MAX, X_MAX, 0);//출력용 배열
	int pad_size = 1;
	double mask[3][3] = {//filter 행렬
	{ (double)1 / 16, (double)2 / 16, (double)1 / 16 },
	{ (double)2 / 16 , (double)4 / 16 , (double)2 / 16 },
	{ (double)1 / 16, (double)2 / 16 , (double)1 / 16}
	};
	for (int h = 0; h < Y_MAX; h++)
	{
		for (int w = 0; w < X_MAX; w++)
		{
			double pixel_value = 0;

			for (int i = -pad_size; i <= pad_size; i++)
				for (int j = -pad_size; j <= pad_size; j++)
					pixel_value += pad[h + (i + pad_size)][w + (j + pad_size)] * ((double)mask[i + pad_size][j + pad_size]);
			out[h][w] = (int)pixel_value;
		}
	}
	string output = "filter3";
	output.append(".raw");
	FILE* outfile = fopen(output.c_str(), "w+b");
	for (int i = 0; i < Y_MAX; i++)
	{
		fwrite(out[i], sizeof(char), X_MAX, outfile);
	}
	fclose(outfile);
}

int main(void)
{
	unsigned char** in_data = mem_alloc2_d(X_MAX, Y_MAX, 0);
	// 이미지를 읽어온다

	FILE* in = fopen("lena256.raw", "rb");
	if (in == NULL)
	{
		printf("File not found!!\n");
		return 0;
	}
	for (int i = 0; i < Y_MAX; i++)
	{
		fread(in_data[i], sizeof(char), X_MAX, in);
	}
	fclose(in);
	smooth_filter(in_data);


	return 0;
}