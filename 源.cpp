#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include <vector>
using namespace std;

int nword = 0; int times = 0; int layer = 0; int P_flag = 0; int skip_flag = 0; int atom_trigger = 0;
int killon = 0; //跳过最后三个connect
int killof = 0; //将所有的PROC转移到另一个去做处理
vector<string> zipped_two;

void zip_func(string s) //每次传进来原始的一行，尽量做压缩，得到压缩后的zipped_two
{
	if (s == "") //空字符跳过防溢出
		;
	else {
		// 把传入的每行分割，/t及空格会去除
		string s_cpoy = s;
		vector<string> helper;
		string breaker;
		stringstream zip_input;
		string zip_zoom;
		zip_input << s_cpoy;
		while (zip_input >> zip_zoom)
			helper.push_back(zip_zoom);

		//判断
		if (helper[0] == "<atom")
		{
			//隐患？
			killof = 0;
			skip_flag = 0;
			zipped_two.push_back(s);
			nword = 1;
			times = 11;
			atom_trigger = 1;
			if (helper[2] == "kind=\"GlobalStatement\"")
				killon = 1;
		} 
		if (helper[0] == "</atom>"&&killon > 0)
		{
			zipped_two.push_back(s);
			killof = 1;
		}
		else if (skip_flag > 0 ||(killon==1&&killof==1))
		{
			if (helper[0] == "<model") {
				layer++;
				cout <<"skipped "<< layer << " " << s << endl;
			}
			else if (helper[0] == "</model>") {
				if (killon > 0) {
					layer--;
					zipped_two.push_back("</project>");
					cout << "skipped " << layer << " " << s << endl;
				}
				else {
					layer--;
					cout << "skipped " << layer << " " << s << endl;
				}
			}
			else
				;
		}
		else if (nword > 0)
		{
			times--;
			if (times <= 0)
				nword = 0;
		}
		else if (helper[0] == "<project")
		{
			zipped_two.push_back("<project>");
			nword = 1;
			times = 5;
		}
		else if (helper[0] == "<regnode")
		{
			nword = 1;
			times = 8;
		}
		else if ((helper[0] == "<connection")  && helper[2] == "kind=\"PhysicalAccess\"")
		{
				times = 7;
				nword = 1;
		}
		else if((helper[0] == "<connection") ){
				
				zipped_two.push_back(s);
				times = 7;
				nword = 1;
			}
		else if ((helper[0] == "</connection")&&(P_flag > 0) && (layer == 3))
		{
			zipped_two.push_back(s);
		}
		else if (helper[0] == "<model")
		{
			if (helper[2] == "kind=\"Line\"")
			{
				nword = 1;
				times = 1;
				layer=layer+1;
				cout << layer << " " << s << endl;
			}
			else if (helper[2] == "kind=\"PROD\"" || helper[2] == "kind=\"RES\"" || helper[2] == "kind=\"PROC\"")
			{
				zipped_two.push_back(s);
				layer=layer+1;
				cout << layer << " " << s << endl;
			}
			else if (helper[2] == "kind=\"Individual\"" || helper[2] == "kind=\"AllDevices\""||helper[2]=="kind=\"Model\"")
			{
				P_flag = 1;
				layer=layer+1;
				nword = 1;
				times = 10;
				cout << layer << " " << s << endl;
			}
			else if (helper[2] == "kind=\"Inner\"")
			{
				layer=layer+1;
				nword = 1;
				times = 10;
				cout << layer << " " << s << endl;
			}
			else if (helper[2] == "kind=\"Loc\"")
			{
				zipped_two.push_back(s);
				layer=layer+1;
				cout << layer << " " << s << endl;
			}
			//else
		}
		else if (helper[0] == "</model>"&&layer == 5)
		{
			zipped_two.push_back(s);
			layer=layer-1;
			cout << layer << " " << s << endl;
		}
		else if (helper[0] == "</model>"&&layer == 4)
			{
				layer=layer-1;
				cout << layer << " " << s << endl;
			}
		else if (helper[0] == "</model>"&&layer == 3)
			{
				skip_flag = 1;
				layer=layer-1;
				cout << layer << " " << s << endl;
			}
		else if (helper[0] == "</model>" && (atom_trigger > 0))
			{
				zipped_two.push_back(s);
				atom_trigger = 0;
				P_flag = 0;
				skip_flag = 0;
				layer=layer-1;
				cout << layer << " " << s << endl;
			}
		else
			zipped_two.push_back(s);

		zip_input.clear(ios::goodbit);// 一行input输入完成后重新设置可以接受输入的状态
	}
}

int main()
{
	fstream f("MODEL.txt");
	vector<string> words;
	string line;
	while (getline(f, line))
		words.push_back(line);//xml in vector
	
	
	for (int i = 0; i < words.size(); i++) {
		string word = words[i];
		zip_func(word);
	}
	
	
	//写入文件
	for (int i = 0; i < zipped_two.size(); i++)
	{
		ofstream location_out;
		string ss;
		ss = zipped_two[i];
		location_out.open("1location_out.txt", std::ios::out | std::ios::app);  //以写入和在文件末尾添加的方式打开.txt文件，没有的话就创建该文件。
		if (!location_out.is_open())
			return 0;

		location_out << ss << endl;
		location_out.close();

	}

	system("pause");
	return 0;
}