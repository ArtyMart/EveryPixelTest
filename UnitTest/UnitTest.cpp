#include "pch.h"
#include "CppUnitTest.h"
#include "EveryPixelTest.cpp"

#define str_pair std::pair<std::string, std::string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:

		TEST_METHOD(TestReadConfig)
		{
			// ��������� ��������
			ConfigData expected_config;
			expected_config.numThreads = 3;
			expected_config.rootDir = "E:\\\\work\\\\projects\\\\EveryPixelTest\\\\test";
			expected_config.replacements = replacements_vector{ str_pair { "<%COLOR%>","BLACK" } , str_pair { "<%NAME%>","Ivanov Ivan" }, str_pair { "<%NUMBER%>","twenty five" } };

			// ���������� ��������
			ConfigData actual_config = readConfig(CONFIG_FILE_NAME);

			// ���������
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_config.numThreads, actual_config.numThreads);
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_config.rootDir, actual_config.rootDir);
			for (int i = 0; i < 3; i++)
			{
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_config.replacements[i].first, actual_config.replacements[i].first);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_config.replacements[i].second, actual_config.replacements[i].second);
			}
			// TO DO: ���� �� ������� �����, ftp-������
		}

		TEST_METHOD(TestParseDirectory)
		{
			// ��������� ��������
			std::vector<std::string> expected_files = {"E:\\work\\projects\\EveryPixelTest\\test\\file (1).txt"};
			for (int i = 10; i <= 19; i++)
				expected_files.push_back("E:\\work\\projects\\EveryPixelTest\\test\\file (" + std::to_string(i) + ").txt");
			expected_files.push_back("E:\\work\\projects\\EveryPixelTest\\test\\file (2).txt");
			for (int i = 20; i <= 27; i++)
				expected_files.push_back("E:\\work\\projects\\EveryPixelTest\\test\\file (" + std::to_string(i) + ").txt");
			for (int i = 3; i <= 9; i++)
				expected_files.push_back("E:\\work\\projects\\EveryPixelTest\\test\\file (" + std::to_string(i) + ").txt");

			// ���������� ��������
			std::vector<std::string> actual_files;
			parseDirectory("E:\\work\\projects\\EveryPixelTest\\test", actual_files);

			// ���������
			for (int i = 0; i < 27; i++)
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_files.at(i), actual_files.at(i));

			// TO DO: ����� ��� ����������, ����� ������ � ������� � ����������, ����� �� ����-��������� � ��������, ������
		}

		TEST_METHOD(TestReplaceInFile)
		{
			// ��������� ����� ����� �� ������
			std::string expected_content = "qwr <%COLOR%> asd\nzxc <%NUMBER%> lkj\njjsdf sdklf <%NAME%> asljdh"; 

			// ���������� �������� - ��������� ���������� ����� �� ������
			std::string filename = "E:\\work\\projects\\EveryPixelTest\\test\\file (1).txt";
			std::ifstream inputFile(filename);
			if (!inputFile.is_open()) {
				std::string str = "Error: Cannot open input file " + filename;
				Assert::Fail(std::wstring(str.begin(), str.end()).c_str());
				return;
			}
			std::stringstream buffer;
			buffer << inputFile.rdbuf();
			std::string actual_content = buffer.str();
			inputFile.close();

			// ��������� ����� �� ������
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_content, actual_content);

			// ��������� ����� ����� ����� ������
			expected_content = "qwr BLACK asd\nzxc twenty five lkj\njjsdf sdklf Ivanov Ivan asljdh"; 

			// ���������� �������� - ��������� ���������� ����� ����� ������
			replacements_vector replacements { str_pair { "<%COLOR%>","BLACK" } , str_pair { "<%NAME%>","Ivanov Ivan" }, str_pair { "<%NUMBER%>","twenty five" } };
			replaceInFile(filename, replacements);

			std::ifstream inputFile2(filename);
			if (!inputFile2.is_open()) {
				std::string str = "Error: Cannot open input file " + filename;
				Assert::Fail(std::wstring(str.begin(), str.end()).c_str());
				return;
			}
			std::stringstream buffer2;
			buffer2 << inputFile2.rdbuf();
			actual_content = buffer2.str();
			inputFile2.close();

			// ��������� ����� ����� ������
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_content, actual_content); 

			// ������� ���������� ����� � ��������� ���������
			replacements = { str_pair { "BLACK","<%COLOR%>" } , str_pair { "Ivanov Ivan","<%NAME%>" }, str_pair { "twenty five","<%NUMBER%>" } };
			replaceInFile(filename, replacements);
		}
	};
}