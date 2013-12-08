// ============== ВАЖНО ==============

// Структура программа столь мала (по сути, весь код, реализующий требуемый алгоритм, находится в функции длиной меньше 100 строк кода),
// что разделять её исходный код на несколько модулей, по меньшей мере, глупо
// Я уже сообщал Вам об этом на практическом занятии, и Вы согласились

// ===================================

// - Не реализована поддержка прямой речи (было бы довольно сложно парсить предложения в данном случае)
// - Абзацем считается последовательность из символов '\n' и '\t'

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>

// Класс, используемый для сигнализации ошибки при проведении юнит-тестов
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

// Структура, содержащая информацию о тексте
struct file_info
{
	std::size_t symbols_count;
	std::size_t words_count;
	std::size_t sentences_count;
	std::size_t lines_count;
	std::size_t paragraphs_count;
	std::map<std::size_t, std::size_t> sentences_stats; // Ключ - кол-во слов в предложении, значение - кол-во таких предложений в заданном тексте
};

// Функция, реализующая подсчёт кол-ва различных элементов текста в заданном файле
// В качестве аргумента принимает имя файла
// Возвращает объект структуру, содержащий информацию о кол-ве символов, слов, предложений, etc в указанном файле
// В случае невозможности открыть заданный файл функция выбрасывает исключение в виде объекта класса std::ios_base::failure
file_info get_file_info(const std::string& file_name)
{
	std::ifstream input_file;
	input_file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	input_file.open(file_name);

	file_info info = { 0, 0, 0, 0, 0 };

	// Переменная, обозначающая, что в данный момент мы парсим слово
	bool is_word_began = false;

	// Переменная, необходимая для обнаружения абзацев в тексте
	// Чтобы обнаруживать абзац на первой же строчке текста, предполагаем, что предыдущий символ - '\n'
	char prev_symbol = '\n';

	// Переменная, обозначающая кол-во уже встреченных слов в текущем предложении
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

// Функция, запускающая на выполнение юнит-тесты
// Ничего не принимает в качестве аргумента и не возвращает никакого значения
// Об ошибке в проведении юнит-тестирования сообщает выбрасыванием исключения в виде объекта класса unit_tests_exception
// Хотелось бы использовать для юнит-тестов что-нибудь наподобие boost test,
// но, если я правильно понимаю, по условиям задачи подобные вещи не разрешены
// В юнит-тестах, само собой разумеется, пришлось использовать магические константы, заводить для каждой из ни отдельную переменную - это, на мой взгляд, глупо
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
		file_info info_6 = get_file_info("some_file.txt"); // Такого файла нет
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

	// Хотелось бы использовать boost::filesystem::path, но, насколько я понимаю, подобные вещи запрещены
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
	// Не знаю, можно ли было использовать range-based for из C++11 - судя по методичке, никаких требований по данному поводу не было
	for (const auto& cur_stat : info.sentences_stats)
	{
		std::cout << cur_stat.first << " - " << cur_stat.second << '\n';
	}
}
