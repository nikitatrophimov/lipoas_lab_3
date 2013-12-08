// ============== ����� ==============

// ��������� ��������� ����� ���� (�� ����, ���� ���, ����������� ��������� ��������, ��������� � ������� ������ ������ 100 ����� ����),
// ��� ��������� � �������� ��� �� ��������� �������, �� ������� ����, �����
// � ��� ������� ��� �� ���� �� ������������ �������, � �� �����������

// ===================================

// - �� ����������� ��������� ������ ���� (���� �� �������� ������ ������� ����������� � ������ ������)
// - ������� ��������� ������������������ �� �������� '\n' � '\t'

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>

// �����, ������������ ��� ������������ ������ ��� ���������� ����-������
class unit_tests_exception : public std::exception
{
public:
	unit_tests_exception(const std::string& msg) : _msg("Unit tests exception: " + msg) {}

	/* virtual */ const char* what() const override
	{
		return _msg.c_str();
	}

private:
	const std::string _msg;
};

// ���������, ���������� ���������� � ������
struct file_info
{
	std::size_t symbols_count;
	std::size_t words_count;
	std::size_t sentences_count;
	std::size_t lines_count;
	std::size_t paragraphs_count;
	std::map<std::size_t, std::size_t> sentences_stats; // ���� - ���-�� ���� � �����������, �������� - ���-�� ����� ����������� � �������� ������
};

// �������, ����������� ������� ���-�� ��������� ��������� ������ � �������� �����
// � �������� ��������� ��������� ��� �����
// ���������� ������ ���������, ���������� ���������� � ���-�� ��������, ����, �����������, etc � ��������� �����
// � ������ ������������� ������� �������� ���� ������� ����������� ���������� � ���� ������� ������ std::ios_base::failure
file_info get_file_info(const std::string& file_name)
{
	std::ifstream input_file;
	input_file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	input_file.open(file_name);

	file_info info = { 0, 0, 0, 0, 0 };

	// ����������, ������������, ��� � ������ ������ �� ������ �����
	bool is_word_began = false;

	// ����������, ����������� ��� ����������� ������� � ������
	// ����� ������������ ����� �� ������ �� ������� ������, ������������, ��� ���������� ������ - '\n'
	char prev_symbol = '\n';

	// ����������, ������������ ���-�� ��� ����������� ���� � ������� �����������
	std::size_t words_count_in_cur_sentence = 0;

	std::for_each(
		std::istreambuf_iterator<char>(input_file)
		, std::istreambuf_iterator<char>()
		, [&](unsigned char c
		)
		{
			++info.symbols_count;

			if (std::isalpha(c))
			{
				is_word_began = true;
			}
			else if (std::isspace(c))
			{
				if (is_word_began == true)
				{
					++words_count_in_cur_sentence;
					++info.words_count;
					is_word_began = false;
				}

				if (c == '\n')
				{
					++info.lines_count;
				}
				else if (c == '\t' && prev_symbol == '\n')
				{
					++info.paragraphs_count;
				}
			}
			else if ((c == '.' || c == '!' || c == '?') && std::isalpha(prev_symbol))
			{
				++info.sentences_stats[++words_count_in_cur_sentence];
				words_count_in_cur_sentence = 0;
				is_word_began = false;
				++info.sentences_count;

				++info.words_count;
			}

			prev_symbol = c;
		}
	);

	return info;
}

// �������, ����������� �� ���������� ����-�����
// ������ �� ��������� � �������� ��������� � �� ���������� �������� ��������
// �� ������ � ���������� ����-������������ �������� ������������� ���������� � ���� ������� ������ unit_tests_exception
// �������� �� ������������ ��� ����-������ ���-������ ��������� boost test,
// ��, ���� � ��������� �������, �� �������� ������ �������� ���� �� ���������
// � ����-������, ���� ����� ����������, �������� ������������ ���������� ���������, �������� ��� ������ �� �� ��������� ���������� - ���, �� ��� ������, �����
void run_unit_tests()
{
	std::cout << "Trying to run 1st unit test... ";
	file_info info_1 = get_file_info("file_1.txt");
	if (
		info_1.symbols_count != 84
		|| info_1.words_count != 19
		|| info_1.sentences_count != 6
		|| info_1.lines_count != 3
		|| info_1.paragraphs_count != 0
		|| info_1.sentences_stats[1] != 2
		|| info_1.sentences_stats[2] != 1
		|| info_1.sentences_stats[3] != 1
		|| info_1.sentences_stats[5] != 1
		|| info_1.sentences_stats[7] != 1
	)
	{
		throw unit_tests_exception("An error occurred while running 1st unit test");
	}

	std::cout << "Success \n";

	std::cout << "Trying to run 2nd unit test... ";
	file_info info_2 = get_file_info("file_2.txt");
	if (
		info_2.symbols_count != 83
		|| info_2.words_count != 9
		|| info_2.sentences_count != 5
		|| info_2.lines_count != 4
		|| info_2.paragraphs_count != 2
		|| info_2.sentences_stats[1] != 1
		|| info_2.sentences_stats[2] != 4
	)
	{
		throw unit_tests_exception("An error occurred while running 2nd unit test");
	}
	std::cout << "Success \n";

	std::cout << "Trying to run 3rd unit test... ";
	file_info info_3 = get_file_info("file_3.txt");
	if (
		info_3.symbols_count != 44
		|| info_3.words_count != 7
		|| info_3.sentences_count != 4
		|| info_3.lines_count != 1
		|| info_3.paragraphs_count != 0
		|| info_3.sentences_stats[1] != 1
		|| info_3.sentences_stats[2] != 3
	)
	{
		throw unit_tests_exception("An error occurred while running 3rd unit test");
	}
	std::cout << "Success \n";

	std::cout << "Trying to run 4th unit test... ";
	file_info info_4 = get_file_info("file_4.txt");
	if (
		info_4.symbols_count != 27
		|| info_4.words_count != 5
		|| info_4.sentences_count != 3
		|| info_4.lines_count != 1
		|| info_4.paragraphs_count != 0
		|| info_4.sentences_stats[1] != 1
		|| info_4.sentences_stats[2] != 2
	)
	{
		throw unit_tests_exception("An error occurred while running 4th unit test");
	}
	std::cout << "Success \n";

	std::cout << "Trying to run 5th unit test... ";
	file_info info_5 = get_file_info("file_5.txt");
	if (
		info_5.symbols_count != 17
		|| info_5.words_count != 3
		|| info_5.sentences_count != 2
		|| info_5.lines_count != 1
		|| info_5.paragraphs_count != 0
		|| info_5.sentences_stats[1] != 1
		|| info_5.sentences_stats[2] != 1
	)
	{
		throw unit_tests_exception("An error occurred while running 5th unit test");
	}
	std::cout << "Success \n";

	std::cout << "Trying to run 6th unit test... ";
	try
	{
		file_info info_6 = get_file_info("some_file.txt"); // ������ ����� ���
		throw unit_tests_exception("An error occurred while running 6th unit test");
	}
	catch (const std::ios_base::failure& ex) {}
	std::cout << "Success \n";
}

int main()
{
	std::cout << "This program counts the amount of characters, words, lines, sentences and paragraphs \n"
			  << "It also shows the statistics about words count in sentences \n"
			  << "Author: Trophimov N. A. \n\n";

	try
	{
		std::cout << "Trying to run unit tests... \n";
		run_unit_tests();
		std::cout << "Success \n\n";
	}
	catch (const unit_tests_exception& ex)
	{
		std::cerr << ex.what() << '\n';
		return EXIT_FAILURE;
	}

	// �������� �� ������������ boost::filesystem::path, ��, ��������� � �������, �������� ���� ���������
	std::cout << "Please enter the file name: \n";
	std::string file_name;
	std::getline(std::cin, file_name);

	file_info info = get_file_info(file_name);

	std::cout << "symbols_count: " << info.symbols_count
			  << " | words_count: " << info.words_count
			  << " | sentences_count: " << info.sentences_count
			  << " | lines_count: " << info.lines_count
			  << " | paragraphs_count: " << info.paragraphs_count
			  << "\n\n";

	std::cout << "Sentences stats: \n"
			  << "Format: amount of words in sentence - amount of sentences with this amount of words \n";
	// �� ����, ����� �� ���� ������������ range-based for �� C++11 - ���� �� ���������, ������� ���������� �� ������� ������ �� ����
	for (const auto& cur_stat : info.sentences_stats)
	{
		std::cout << cur_stat.first << " - " << cur_stat.second << '\n';
	}
}
