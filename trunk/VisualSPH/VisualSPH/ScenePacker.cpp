#include "SceneUnPacker.h"

#include <cstdio>

using namespace std;

char path_to_folder[] = "./frames/2/";
char file_pattern[] = "_flu.txt";


// Возвращает false, если на каком-то этапе произошла ошибка.
// Не гарантируется, что любое отклонение от входного формата данных будет отловлено, берегитесь.
bool pack_scene(char prefix[], int first, int last, char postfix[], char output[])
{
	FILE *pOut = fopen(output, "wb");
	// Количество фреймов
	{int tmp = last - first; fwrite(&tmp, sizeof(int), 1, pOut);}

	float data[MAX_PARTICLES][DIMENSIONS];
	for(int num = first; num <= last; ++num)
	{
		// Составить имя файла
		char name[50];
		sprintf(name, "%s%05d%s", prefix, num, postfix);
		//printf("%s\n", name);
		FILE *pFile = fopen(name, "r");
		
		// Пропустить первую строчку
		char junk[30];
		for(int i = 0; i < DIMENSIONS; ++i)
			fscanf(pFile, "%s", junk);

		int sz = 0;
		for(;fscanf(pFile, "%f %f %f %f %f %f %f %f", &data[sz][0], &data[sz][1], &data[sz][2], &data[sz][3], &data[sz][4], &data[sz][5], &data[sz][6], &data[sz][7]) == DIMENSIONS;++sz);

		// Количество частиц во фрейме
		fwrite(&sz, sizeof(int), 1, pOut);

		// Частицы
		for(int i = 0; i < sz; ++i)
			fwrite(&data[i], sizeof(float), DIMENSIONS, pOut);

		fclose(pFile);
	}
	fclose(pOut);
	return true;
}

int main(int argc, char** argv)
{
	pack_scene(path_to_folder, 1, 10, file_pattern, "sph.bin");
//	SceneUnPacker("sph.bin");
}