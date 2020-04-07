/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2020 Panda Team
*/

#if defined(__linux__)
	#include <dirent.h>
#endif

#include <vector>
#include <any>

#include <iostream>
#include <fstream>

#if defined(_WIN64)
#include <filesystem>
#endif

#include <chrono>

#include "../../modules/utils/ThreadPool.cpp"
#include "../../modules/utils/Semaphore.h"

#include "assets/json.hpp"
#include "../../modules/mapping.hpp"
#include "../../modules/utils/poor_mans_quantum.hpp"


using json = nlohmann::json;


template <typename T>
void matrix_print(const std::vector<std::vector<T>> &mat)
{

    std::cout << "[";
    std::cout << std::endl;
	for (int i = 0; i < mat.size(); i++)
	{
		std::cout << "  [ ";
		if (mat[i].size() > 0)
		{
			for (int j = 0; j < mat[i].size() - 1; j++)
			{
				std::cout << mat[i][j] << ", ";
			}
			std::cout << mat[i][mat[i].size() - 1];
		}
			
		std::cout << " ]" << std::endl;
    }
    std::cout << std::endl;
    std::cout << "]" << std::endl;
}

template <typename T>
void vector_print(const std::vector<T> &vec)
{

    std::cout << "[ ";
    for (int i = 0; i < vec.size() - 1; i++)
    {
        std::cout << vec[i] << ", ";
    }
    std::cout << vec[vec.size() - 1] << " ]" << std::endl;
}

template <typename T>
void vector_print(const std::vector<T> &vec,const size_t width, const size_t height)
{
	if ((width * height) != vec.size()) {
		std::cout << "width * height != vector.size()" << std::endl;
		return;
	}
	
	int max_digits = 1;
	for (auto index = 0; index < vec.size(); ++index) {
		int pos = 10;
		int digits_num = 1;
		while (vec[index] / pos >= 1)
		{
			digits_num++;
			pos *= 10;
		}
		if (digits_num > max_digits)
		{
			max_digits = digits_num;
		}
	}

	for (auto index = 0; index < vec.size(); ++index) {
		
		int pos = 10;
		int digits_num = 1;
		while (vec[index] / pos >= 1)
		{
			digits_num++;
			pos *= 10;
		}
		for (auto i = 0; i < max_digits - digits_num; ++i) {
			std::cout << " ";
		}
		std::cout << vec[index] << " ";

		if ((index + 1) % width == 0) {
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
}

template <typename T>
void image_print(const std::vector<T> &vec, const size_t width, const size_t height)
{
	if ((width * height) != vec.size()) {
		std::cout << "width * height != vector.size()" << std::endl;
		return;
	}
	
	int max_digits = 3;

	for (auto index = 0; index < vec.size(); ++index) {
		
		int pos = 10;
		int digits_num = 1;
		if (vec[index] >= 10)
		{
			digits_num++;
		}
		if (vec[index] >= 100)
		{
			digits_num++;
		}
		for (auto i = 0; i < max_digits - digits_num; ++i) {
			std::cout << " ";
		}
		std::cout << static_cast<unsigned int>(vec[index]) << " ";

		if ((index + 1) % width == 0) {
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
}

std::tuple<std::vector<std::vector<int>>, std::vector<int>> readCsvData(std::string filename, char delimeter)
{
	std::fstream fin;

	fin.open(filename, std::ios::in);
	
	std::vector<int> row;
	std::string line, word, w;

	std::vector<std::vector<int>> rows;
	std::vector<int> labels;

	// omit header
	getline(fin, line);

	int i = 0;
	while (getline(fin, line))
	{
		i++;
		std::stringstream s(line);

		// get label
		getline(s, word, delimeter);
		labels.push_back(std::stoi(word));

		row.clear();
		while (getline(s, word, delimeter))
		{			
			row.push_back(std::stoi(word));
		}

		rows.push_back(row);
	}

	return { rows, labels };
}

std::tuple<std::vector<std::vector<int>>, std::vector<int>> readMnist(std::string filename, char delimeter, int max_rows)
{
	std::fstream fin;

	fin.open(filename, std::ios::in);
	
	std::vector<int> row;
	std::string line, word, w;

	std::vector<std::vector<int>> rows;
	std::vector<int> labels;

	// omit header
	getline(fin, line);

	int i = 0;
	while (getline(fin, line))
	{
		i++;
		std::stringstream s(line);

		// get label
		getline(s, word, delimeter);
		labels.push_back(std::stoi(word));

		row.clear();
		while (getline(s, word, delimeter))
		{			
			row.push_back(std::stoi(word));
		}

		rows.push_back(row);

		if (i >= max_rows)
		{
			break;
		}
	}

	return { rows, labels };
}

std::tuple<std::vector<std::string>, std::vector<std::vector<double>>> readCsvData2(std::string filename, char delimeter)
{
	std::fstream fin;

	fin.open(filename, std::ios::in);
	
	std::vector<std::string> dates;
	std::vector<double> row;
	std::string line, word, w;

	std::vector<std::vector<double>> rows;
	
	int day, month, year, hour, minute, second;
	int added_days = 0;
	bool was_yesterday = false;

	// omit headers 
	getline(fin, line);

	int i = 0;
	while (getline(fin, line))
	{
		i++;
		std::stringstream s(line);

		getline(s, word, delimeter);
		sscanf(word.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second);
		if (was_yesterday && hour * 60 + minute >= 4 * 60)
		{
			dates.push_back(word);
			rows.push_back(row);
			row.clear();
		}

		if (hour * 60 + minute < 4 * 60)
		{
			was_yesterday = true;
		}
		else
		{
			was_yesterday = false;
		}

		while (getline(s, word, delimeter))
		{			
			row.push_back(std::stod(word));
		}

	}

	// erase first element with partial data
	rows.erase(rows.begin());
	dates.erase(dates.end() - 1);

	return { dates, rows };
}

std::vector<int> generate_image(size_t size, size_t min, size_t max)
{	
	std::vector<int> result;
		
	auto random_seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine random_generator(random_seed);
    std::exponential_distribution<double> generator(3.5);
	
    for (int i = 0; i < size; ++i) {
        result.push_back(static_cast<int>(generator(random_generator) * (max - min) + min));
    }

	return result;
}

template <typename T>
std::vector<T> noise_image(std::vector<T> image, int min, int max)
{	
	std::vector<T> result;
		
	auto random_seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine random_generator(random_seed);	
    std::uniform_int_distribution<int> generator(min, max);

    for (int i = 0; i < image.size(); ++i) {
        image[i] += generator(random_generator);
		if (image[i] > 255)
		{
			image[i] = 255;
		}
    }

	return image;
}

template <class ContainerType>
void write_csv(ContainerType data, std::string filename, std::string sep=",")  // container of containers expected, TODO add check
{
    std::ofstream outputFile;
    outputFile.open(filename);
        for (auto i = 0; i < data.size(); ++i) {
            for (auto j = 0; j < data[i].size(); j++) {
                outputFile << std::to_string(static_cast<int>(data[i][j])) << sep;
            }
            outputFile << std::endl;
        }
        outputFile.close();
}

///

int main(int argc, char *argv[])
{
	std::cout << "KOC for MNIST example have started" << std::endl;
	std::cout << std::endl;

	using Record = std::vector<int>;

	size_t best_w_grid_size = 4;
	size_t best_h_grid_size = 4;

	// if overrided from arguments

	if (argc > 3)
	{
		best_w_grid_size = std::stod(argv[2]);
		best_h_grid_size = std::stod(argv[3]);
	}

	std::vector<Record> dataset;
	std::vector<int> labels;
	std::vector<Record> test_set;
	std::vector<int> test_labels;

	std::tie(dataset, labels) = readMnist("assets/mnist_train.csv", ',', 1000);

	//std::cout << std::endl;
	//std::cout << "labels:" << std::endl;
	//vector_print(labels);

	std::tie(test_set, test_labels) = readCsvData("assets/MNIST_anomalies.csv", ',');
	test_set.push_back(dataset[0]);
	test_set.push_back(dataset[1]);
	test_set.push_back(dataset[4]);
	test_set.push_back(generate_image(28 * 28, 0, 255));
	test_set.push_back(generate_image(28 * 28, 0, 255));
	test_set.push_back(generate_image(28 * 28, 0, 1));
	test_set.push_back(noise_image(dataset[0], 0, 150));
	test_set.push_back(noise_image(dataset[1], 0, 150));

	///
	//

	;
	;
	std::vector<int> i1 = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 8, 6, 4, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 8, 16, 22, 25, 28, 19, 21, 72, 12, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 11, 6, 4, 4, 4, 12, 19, 33, 45, 58, 70, 63, 103, 170, 193, 101, 16, 8, 4, 2, 1, 1, 1, 1, 1, 1, 1, 49, 94, 45, 6, 11, 61, 47, 51, 66, 65, 62, 64, 67, 147, 172, 231, 194, 54, 23, 8, 4, 3, 2, 1, 1, 1, 1, 1, 50, 95, 93, 48, 51, 113, 130, 68, 62, 57, 59, 59, 62, 149, 183, 241, 212, 64, 43, 17, 4, 4, 4, 1, 1, 1, 1, 1, 53, 97, 99, 116, 131, 119, 134, 60, 50, 46, 44, 43, 42, 133, 213, 239, 216, 101, 47, 18, 6, 4, 4, 1, 1, 1, 3, 1, 38, 95, 97, 120, 164, 119, 114, 28, 23, 24, 30, 22, 31, 138, 210, 235, 215, 103, 46, 19, 6, 3, 3, 1, 1, 1, 2, 1, 5, 50, 96, 118, 183, 144, 111, 21, 19, 21, 25, 23, 25, 127, 201, 229, 213, 73, 39, 11, 9, 2, 1, 1, 1, 1, 1, 1, 6, 11, 85, 116, 187, 172, 116, 44, 21, 26, 27, 26, 35, 142, 212, 213, 198, 66, 26, 14, 9, 4, 1, 1, 1, 1, 1, 1, 6, 8, 64, 140, 189, 189, 126, 89, 28, 24, 57, 115, 144, 189, 207, 207, 183, 48, 24, 20, 11, 7, 1, 1, 1, 1, 1, 1, 6, 8, 43, 163, 189, 189, 147, 114, 133, 153, 170, 216, 214, 206, 206, 205, 117, 30, 23, 23, 13, 8, 1, 1, 1, 1, 1, 1, 6, 8, 29, 158, 190, 194, 197, 192, 206, 212, 217, 218, 217, 213, 203, 195, 125, 49, 23, 23, 14, 9, 2, 1, 1, 1, 1, 1, 6, 8, 16, 101, 193, 198, 206, 204, 207, 209, 209, 207, 206, 204, 206, 199, 135, 68, 17, 23, 14, 9, 2, 1, 1, 1, 1, 1, 1, 6, 8, 45, 147, 190, 189, 190, 186, 183, 135, 124, 144, 155, 206, 198, 167, 70, 19, 23, 14, 9, 1, 1, 1, 1, 1, 1, 1, 3, 4, 35, 117, 120, 118, 104, 92, 42, 11, 10, 58, 117, 206, 208, 174, 74, 21, 20, 15, 9, 1, 1, 1, 1, 1, 1, 1, 3, 4, 24, 91, 102, 86, 35, 12, 8, 8, 6, 31, 118, 206, 209, 182, 104, 38, 28, 24, 10, 1, 1, 1, 1, 1, 1, 1, 3, 6, 22, 41, 41, 42, 28, 15, 10, 6, 11, 64, 137, 217, 219, 188, 118, 40, 28, 18, 6, 1, 1, 1, 1, 1, 1, 1, 6, 12, 28, 54, 51, 43, 41, 34, 29, 24, 31, 90, 144, 213, 223, 185, 119, 83, 28, 13, 6, 1, 1, 1, 1, 1, 1, 1, 6, 22, 33, 42, 54, 55, 60, 56, 55, 54, 59, 128, 137, 158, 209, 187, 120, 115, 21, 9, 9, 1, 1, 1, 1, 1, 1, 1, 6, 21, 28, 35, 45, 58, 67, 68, 68, 69, 81, 150, 144, 132, 142, 169, 108, 98, 27, 3, 1, 1, 1, 1, 1, 1, 1, 1, 4, 11, 19, 31, 33, 42, 46, 55, 58, 54, 54, 121, 118, 115, 93, 73, 81, 50, 6, 4, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 6, 17, 27, 32, 24, 20, 10, 12, 16, 38, 104, 106, 50, 10, 4, 4, 2, 4, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 3, 4, 3, 2, 5, 4, 4, 4, 4, 2, 3, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 2, 2, 2, 3, 3, 2, 2, 4, 4, 4, 4, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	std::vector<int> i2 = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 9, 8, 6, 4, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 10, 21, 22, 31, 51, 43, 27, 21, 6, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 5, 3, 3, 3, 3, 10, 17, 59, 117, 156, 219, 231, 241, 211, 196, 127, 66, 8, 3, 3, 1, 1, 1, 1, 1, 1, 1, 7, 12, 6, 13, 45, 57, 80, 136, 198, 206, 226, 233, 237, 241, 242, 247, 233, 205, 97, 21, 3, 3, 2, 1, 1, 1, 1, 1, 12, 12, 12, 57, 78, 104, 159, 207, 218, 221, 229, 230, 229, 237, 241, 244, 245, 227, 211, 89, 3, 3, 3, 1, 1, 1, 1, 1, 27, 13, 12, 42, 82, 91, 140, 183, 180, 176, 168, 165, 160, 146, 199, 241, 243, 234, 209, 89, 13, 2, 2, 1, 1, 1, 3, 1, 6, 12, 12, 19, 23, 19, 28, 41, 57, 58, 86, 51, 105, 137, 158, 233, 243, 231, 153, 71, 17, 1, 1, 1, 1, 1, 2, 1, 1, 7, 12, 30, 14, 12, 20, 28, 30, 46, 54, 77, 87, 142, 192, 238, 225, 206, 120, 23, 2, 1, 1, 1, 1, 1, 1, 1, 2, 4, 11, 12, 18, 17, 24, 32, 55, 89, 122, 128, 145, 185, 205, 206, 189, 176, 47, 12, 2, 2, 1, 1, 1, 1, 1, 1, 3, 3, 9, 12, 19, 23, 32, 69, 117, 134, 146, 183, 194, 180, 177, 177, 164, 105, 23, 12, 6, 2, 1, 1, 1, 1, 1, 1, 3, 3, 6, 16, 21, 29, 41, 103, 186, 196, 206, 221, 200, 172, 166, 162, 79, 43, 23, 14, 6, 2, 1, 1, 1, 1, 1, 1, 3, 3, 6, 14, 26, 44, 85, 106, 185, 191, 201, 218, 218, 213, 160, 122, 83, 45, 22, 12, 4, 2, 2, 1, 1, 1, 1, 1, 3, 3, 6, 14, 30, 51, 105, 131, 129, 134, 123, 138, 176, 181, 185, 164, 109, 65, 20, 12, 6, 2, 2, 1, 1, 1, 1, 1, 1, 3, 10, 15, 36, 50, 78, 100, 75, 57, 28, 24, 36, 116, 183, 193, 115, 88, 33, 12, 6, 2, 1, 1, 1, 1, 1, 1, 1, 6, 12, 15, 50, 76, 73, 67, 49, 13, 10, 10, 12, 35, 179, 198, 142, 84, 31, 9, 4, 2, 1, 1, 1, 1, 1, 1, 1, 6, 13, 23, 70, 73, 62, 44, 12, 9, 6, 4, 8, 45, 190, 214, 159, 89, 45, 39, 33, 12, 1, 1, 1, 1, 1, 1, 1, 6, 20, 52, 87, 70, 58, 34, 14, 6, 6, 26, 61, 166, 242, 246, 181, 88, 49, 46, 19, 2, 1, 1, 1, 1, 1, 1, 1, 18, 73, 147, 224, 140, 75, 64, 56, 67, 84, 137, 216, 241, 247, 230, 135, 83, 47, 44, 23, 1, 1, 1, 1, 1, 1, 1, 1, 22, 125, 177, 221, 213, 175, 170, 172, 191, 218, 233, 241, 241, 223, 158, 90, 65, 28, 21, 3, 1, 1, 1, 1, 1, 1, 1, 1, 22, 115, 156, 198, 228, 237, 237, 237, 237, 237, 237, 229, 194, 96, 68, 24, 12, 12, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 14, 49, 104, 177, 187, 206, 213, 232, 217, 198, 149, 95, 56, 16, 11, 8, 11, 7, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 31, 103, 156, 177, 140, 123, 55, 34, 31, 11, 11, 11, 6, 3, 2, 3, 2, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 };
	
    auto cost_mat = metric::EMD_details::ground_distance_matrix_of_2dgrid<int>(28, 28);
    auto maxCost = metric::EMD_details::max_in_distance_matrix(cost_mat);

    std::cout << "" << std::endl;

    metric::EMD<int> distance(cost_mat, maxCost);

    // assumes that i1 and i2 are serialized vectors of the image matrices, and cost_mat contains a distance matrix that takes into account the original pixel locations.
    auto t1 = std::chrono::steady_clock::now();
    auto result = distance(i1, i2);
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "result: " << result
              << " (Time = " << double(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000
              << " s)" << std::endl;
    std::cout << "" << std::endl;

	///////////////////////////////////////////////////////////////////

	int num_clusters = 10;

	// random seed for repeateable results
	long long random_seed = 777;

	// extra deviation of the clusters from original in the KOC
	double sigma = 1.75;

	//metric::KOC_factory<
	//	Record, 
	//	metric::Grid4, 
	//	metric::RandomEMD<Record, double>,
	//	std::uniform_int_distribution<int>
	//> 
	//simple_koc_factory(best_w_grid_size, best_h_grid_size, sigma, 0.8, 0.0, 10, 0, 255, 4, 2.0, random_seed);   

	//std::cout << "#1 " << std::endl;
	//auto simple_koc = simple_koc_factory(dataset, num_clusters); 

	auto distribution = std::uniform_int_distribution<int>(0, 255);
	auto graph = metric::Grid4(best_w_grid_size, best_h_grid_size);
	
	metric::KOC_details::KOC<
		Record, 
		metric::Grid4, 
		metric::EMD<int>,
		std::uniform_int_distribution<int>
	> 
	simple_koc(graph, distance, sigma, 0.8, 0.0, 200, distribution, 4, 2.0, random_seed); 
	std::cout << "#3" << std::endl;

	simple_koc.train(dataset, num_clusters);

	std::cout << "#2" << std::endl;
	
	std::vector<int> grid(best_w_grid_size * best_h_grid_size, -1);
	//std::vector<double> grid(best_w_grid_size * best_h_grid_size, 999999999.9);
	auto nodes = simple_koc.som_.get_weights();
	//matrix_print(nodes);
	write_csv(nodes, "nodes.csv");

	std::cout << std::endl;
	std::cout << "clusters:" << std::endl;
	vector_print(simple_koc.clusters);
	std::vector<std::vector<int>> nodes_clusters;
	nodes_clusters.push_back(simple_koc.clusters);
	write_csv(nodes_clusters, "clusters.csv");
	
	//std::cout << std::endl;
	//std::cout << "train dataset:" << std::endl;

	//auto anomalies = simple_koc.check_if_anomaly(dataset);	
	//std::cout << std::endl;
	//std::cout << "anomalies:" << std::endl;
	//vector_print(anomalies);
	//
	auto assignments = simple_koc.assign_to_clusters(dataset);	
	//std::cout << std::endl;
	//std::cout << "assignments:" << std::endl;
	//vector_print(assignments);


	// accuracy
	
	std::vector<std::vector<int>> clusters(11);	
	for (auto i = 0; i < assignments.size(); ++i)
	{
		clusters[assignments[i]].push_back(labels[i]);
	}
	
	std::vector<int> assignment_to_label(11, -1);
	for (auto i = 1; i < clusters.size(); ++i)
	{
		std::vector<int> labels_in_cluster(10);
		for (auto lbl : clusters[i])
		{
			labels_in_cluster[lbl]++;
		}		
		std::vector<int>::iterator result = std::max_element(labels_in_cluster.begin(), labels_in_cluster.end());
		assignment_to_label[i] = std::distance(labels_in_cluster.begin(), result);	
	}
	
	int num_matched = 0;
	for (auto i = 0; i < assignments.size(); ++i)
	{
		if (assignment_to_label[assignments[i]] == labels[i])
		{
			num_matched++;
		}
	}
	std::cout << std::endl;
	std::cout << "num_matched: " << num_matched << std::endl;
	std::cout << "accuracy: " << (double)num_matched / assignments.size() << std::endl;


	// test dataset	
	
	//std::cout << std::endl;
	//std::cout << std::endl;
	//std::cout << "test dataset:" << std::endl;

	//anomalies = simple_koc.check_if_anomaly(test_set);	
	//std::cout << std::endl;
	//std::cout << "anomalies:" << std::endl;
	//vector_print(anomalies);
	//
	//assignments = simple_koc.assign_to_clusters(test_set);	
	//std::cout << std::endl;
	//std::cout << "assignments:" << std::endl;
	//vector_print(assignments);

	///
	//
	//image_print(test_set[0], 28, 28);	
	//image_print(test_set[1], 28, 28);	
	//image_print(test_set[2], 28, 28);	
	//image_print(test_set[3], 28, 28);	
	//image_print(test_set[4], 28, 28);	
	//image_print(test_set[5], 28, 28);	
	//image_print(test_set[6], 28, 28);	
	//image_print(test_set[7], 28, 28);	

	//metric::CosineInverted<double> distance;
	//
	//std::cout << std::endl;
	//std::cout << distance(test_set[0], test_set[1]) << std::endl; // 4 and 3
	//std::cout << distance(test_set[0], test_set[2]) << std::endl; // 4 and 4
	//std::cout << std::endl;
	//std::cout << distance(test_set[0], test_set[3]) << std::endl; // 4 and noise(0..255)
	//std::cout << distance(test_set[0], test_set[4]) << std::endl; // 4 and noise(0..255)
	//std::cout << distance(test_set[0], test_set[5]) << std::endl; // 4 and noise(0..1)
	//std::cout << std::endl;
	//std::cout << distance(test_set[0], test_set[6]) << std::endl; // 4 and 4 (noised)
	//std::cout << distance(test_set[0], test_set[7]) << std::endl; // 4 and 3 (noised)
	//std::cout << std::endl;
	//std::cout << distance(test_set[3], test_set[4]) << std::endl; // noise(0..255) and noise(0..255)
	//std::cout << distance(test_set[3], test_set[5]) << std::endl; // noise(0..255) and noise(0..1)
	


    return 0;
}
